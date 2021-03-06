/*

ocXML.h
=======

Quick and dirty XML parser.
Copyright (C) 2004 - 2007, David McCombs w3sys.com

TODO: 
Add '<!CDATA[...]]>' handling and XPATH functionality
CDATA is lower priority then XPATH, since XML escape is implemented.
I really like XPATH!
(Implemetation will probably use a useXPATH boolean, 
 defaulted to off for backward compatibility.)
 
 4/16/07 XPATH DONE!
 
*/
#include <string>
#include <iomanip>
#include <algorithm>
#include <map>
#include <vector>
// #include <stack> - use vector so I can iterate through the els
#include <fstream>
#include "ocString.h"

#ifndef OC_XML_H
#define OC_XML_H

using namespace std;

// define a base function object to be used by the parser
class parseFobject
{
public:
  // ctor
  parseFobject()
  {;}
  // the function operator itself
  virtual void operator () ( ocString & in )
  {;}// default base method does nothing
};

// define the base parser
class ocGenericParser
{
protected:
  istream * iStream;
  ocString  input;
  parseFobject * pCurrentFunction;
public:
  ocGenericParser( istream * in ):iStream(in),pCurrentFunction(NULL)
  {
    ;
  }
  ocGenericParser( string & in ):iStream(NULL),pCurrentFunction(NULL)
  {
    input = in;
  }
  virtual ~ocGenericParser()
  {;}
  bool parse( void )
  {
    // the object will be entered with 1 of 2 constructors
    bool bGood=true;
    do
    {
      // IS a function, input to parse, input is not finished parsing
      while( pCurrentFunction && input.length() && !input.endOfParse() )
      {
        curFunc()(input);
      }
      if(iStream)
      {
        iStream->clear();
        getline(*iStream,input);
        input.parseInit();
        bGood = iStream->good();
      }
    } while( pCurrentFunction && iStream && !iStream->eof() );
    return bGood;
  }
  parseFobject & curFunc(void)
  {
    return *pCurrentFunction;
  }
  virtual void callback( void )
  {
    ;
  }
};

// Define a derived function object to overload
class baseFunc: public parseFobject
{
protected:
  ocGenericParser & rParser; // reference to parser
public:
  // ctor
  baseFunc(ocGenericParser & irParser):parseFobject(),rParser(irParser){;}
  // v dtor
  virtual ~baseFunc(){;}
};

/*
  Escape these values:
  int [34]  hex [22] = ["] = &#x22;
  int [37]  hex [25] = [%] = &#x25;
  int [38]  hex [26] = [&] = &#x26;
  int [39]  hex [27] = ['] = &#x27;
  int [43]  hex [2b] = [+] = &#x2b;
  int [60]  hex [3c] = [<] = &#x3c;
  int [62]  hex [3e] = [>] = &#x3e;

  in hex the escape would be &#xNN;
  The values will be unescaped by the xml parser.
  see W3C XML spec section 4.1 Character and Entity References
*/
string xmlEscape( string in )
{
  ocString temp ( in );
  return temp.replaceAll( "\"","&#x22;" )
             .replaceAll( "%", "&#x25;" )
             .replaceAll( "&", "&#x26;" )
             .replaceAll( "'", "&#x27;" )
             .replaceAll( "+", "&#x2b;" )
             .replaceAll( "<", "&#x3c;" )
             .replaceAll( ">", "&#x3e;" );
}
string xmlUnescape( string in )
{
  ocString temp ( in );
  return temp.replaceAll(  "&#x22;", "\"")
             .replaceAll(  "&#x25;", "%" )
             .replaceAll(  "&#x26;", "&" )
             .replaceAll(  "&#x27;", "'" )
             .replaceAll(  "&#x2b;", "+" )
             .replaceAll(  "&#x3c;", "<" )
             .replaceAll(  "&#x3e;", ">" );
}

/*
  Abstraction of a (flat) xml node  
*/
typedef class map<string,string> node_attr;
class xmlNode
{
public:
  string name;
  string xpath;
  string data;
  node_attr attr;
  bool monopole;
  xmlNode()
  :name(""),data(""),attr(),monopole(false)
  {;}
  xmlNode(const xmlNode & in)
  :name(in.name),data(in.data),attr(in.attr),monopole(in.monopole)
  {;}
  xmlNode & operator = (const xmlNode & in)
  {
    name=in.name;
    data=in.data;
    attr=in.attr;
    monopole=in.monopole;
    return *this;
  }
  string emit( void )
  {
    string ret = "<";
    ret += name;
    if( attr.size() )
    {      
      node_attr::iterator it;
      for( it = attr.begin(); it != attr.end(); ++it )
      {
        ret += " ";
        ret += it->first;
        ret += "='";
        ret += xmlEscape(it->second);
        ret += "'";
      }      
    }
    if( monopole )
    {
      ret += "/>";
    }
    else
    {
      ret += ">";
      ret += xmlEscape(data);
      ret += "</";
      ret += name;
      ret += ">";
    }
    return ret;
  }
};

/*
  This particular implementaion flattens the tree into a multimap.
*/
typedef class vector<xmlNode> node_vector;
typedef class multimap<string,size_t> node_map;
typedef class vector<size_t>node_stack;
struct stateMachine
{
  node_vector nodes;
  node_stack nodestack;
  node_map   nodemap;
  xmlNode & topNode( void )
  {
    return nodes[nodestack.back()];
  }
};

// start with the XML operators
class findStart: public baseFunc
{
public:
  ocString data;
  bool foundStart;
  findStart(ocGenericParser & irParser):baseFunc(irParser)
  {
    foundStart = false;
  }
  void operator () ( ocString & in )
  {
    data += in.parse("<");
    foundStart = (in.lastPos() > 0);
    if( foundStart )
    {
      rParser.callback();
      data = "";
    }
  }
};


class findEnd: public baseFunc
{
public:
  ocString data;
  bool foundEnd;

  findEnd(ocGenericParser & irParser):baseFunc(irParser)
  {
    foundEnd = false;
  }

  void operator () ( ocString & in )
  {
    data += in.parse(">");
    foundEnd = (in.lastPos() > 0);
    if( foundEnd )
    {
      rParser.callback();
      data = "";
    }
  }
};


// Now make the special xml flavor of the parser
class xmlParser : public ocGenericParser
{
private:
  // function objects
  findStart    start;
  findEnd      end;  
  node_vector::iterator xnode_it;
  
  void startCallback(void)
  {
    size_t len = start.data.length();
    if( len && !states.nodestack.empty() )
    {
      // Add data to the last known top node
      xmlNode & rNode =  states.topNode();
      rNode.data += xmlUnescape(start.data);
    }
    pCurrentFunction = &end;
  }
  void endCallback(void)
  {
    ocString parseableData = end.data;
    size_t len = end.data.length();
    if( len == 0 ) return;

    // see if this is the end ex: </tag>
    bool isEndTag = end.data[0] == '/';
    // see if this is a monopole ex: <tag/>
    bool isMonoTag = end.data[len-1] == '/';
    if( isMonoTag )
    {
      len--;
      parseableData.resize(len);
    }
    if( !isEndTag || isMonoTag )
    {
      // This is the beginning container tag, or a monotag
      // EX: [name attr1='x' attr2="y z" ...]
      //   so parse the name and attributes:
      xmlNode node;
      // Get the name
      node.name = parseableData.tokenParse( " \t\n\r" );
      if( isMonoTag ) node.monopole = true;
      // Get the attributes (if any)
      do
      {
        // EX: [attr1='x' attr2="y z" ...]
        string attrCandidate = parseableData.parse( "=" );
        // for attributes after the first attribute, there will be a space...
        while( attrCandidate.size() && (
               attrCandidate[0] == ' ' ||
               attrCandidate[0] == '\n' || 
               attrCandidate[0] == '\t' ) ) attrCandidate.erase(0,1);
        if(attrCandidate.length())
        {
          string attrName = attrCandidate;
          // EX: ['x' attr2="y z" ...]
          // build token on valid chars ' or "
          char pTok[2];
          pTok[0] = parseableData.remainder()[0];
          pTok[1] = '\0';
          parseableData.parse( pTok ); // strip the first quote
          string attrValue = parseableData.parse( pTok ); // get the value
          // EX: [ attr2="y z" ...] NOTE THE SPACE
          if( attrName.length() && attrValue.length() )
          {
            node.attr.insert(make_pair(attrName,xmlUnescape(attrValue)));
          }
        }
      } while(!parseableData.endOfParse());
      // add the node
      addNode(node);
            
      if( !isMonoTag )
      {
        // Push the last item on the stack
        //   so data can be added to the parent
        //   node in a nested doc.
        size_t nTop = states.nodes.size()-1;
        states.nodestack.push_back(nTop);
      }
    } // End if start of container
    if(isEndTag)
    {
      // This contained item is finished so
      //   pop the last item off the stack
      states.nodestack.pop_back();
    }
    pCurrentFunction = &start;
  }
public:

  bool useXPATH;
  // state machine
  stateMachine states;

  xmlParser( istream * in ):ocGenericParser(in),start(*this),end(*this),useXPATH(false)
  {
    pCurrentFunction = &start;
  }
  xmlParser(string & in):ocGenericParser(in),start(*this),end(*this),useXPATH(false)
  {
    pCurrentFunction = &start;
  }
  virtual ~xmlParser(){;}

  virtual void callback( void )
  {
    if( pCurrentFunction == &start )
    {
      startCallback();
    }
    else if(pCurrentFunction == &end)
    {
      endCallback();
    }
  }
  // Return the node list
  node_vector & nodeList( void )
  {
    return states.nodes;
  }
  // Add a new node, outside of the parse
  void addNode( xmlNode &node )
  {
    states.nodes.push_back(node);
    string nodeName;
    if( useXPATH )
    {
      nodeName = "/";
      for( int i=0; i<states.nodestack.size(); i++ )
      {
        size_t sz = states.nodestack[i];
        nodeName += states.nodes[sz].name;
        nodeName += "/";
      }      
    }
    nodeName += node.name;
    if( useXPATH ) node.xpath = nodeName;
    states.nodemap.insert(make_pair(nodeName,states.nodes.size()-1));
  }
  
  // Re-emit as XML
  string emit( void )
  {
    string ret;
    for( int i=0; i<states.nodes.size(); i++ )
    {
      ret += states.nodes[i].emit();
    }
    return ret;
  }
  
  /// Find the first instance of a node named 'name' 
  node_vector::iterator & findFirstNodeByName( string name )
  {
    xnode_it = states.nodes.begin();
    return seekNodeByName( name );
    
  }  
  /// Find the next instance of a node named 'name' 
  node_vector::iterator & findNextNodeByName( string name )
  {  
    ++xnode_it; // presume we found one, go to next
    return seekNodeByName( name );
  } 
  
  /// Find the first instance of a node that has an attribute 'name' having 'value'
  node_vector::iterator & findFirstNodeByAttribute( string name, string value )
  {
    xnode_it = states.nodes.begin();
    return seekNodeByAttribute(  name,  value );    
  }  
  /// Find the next instance of a node that has an attribute 'name' having 'value'
  node_vector::iterator & findNextNodeByAttribute( string name, string value )
  {  
    ++xnode_it; // presume we found one, go to next
    return seekNodeByAttribute(  name,  value );
  } 
  
protected:
  // Find the next instance of a node that has an attribute 'name' having 'value'
  node_vector::iterator & seekNodeByAttribute( string name, string value )
  {        
    for( ;xnode_it!=states.nodes.end();++xnode_it)
    {
      node_attr::iterator it = (*xnode_it).attr.find(name);
      if( it != (*xnode_it).attr.end() )
      {
        if( it->second == value ) 
        {  
          break;
        }  
      }    
    }
    return xnode_it;
  } 
  node_vector::iterator & seekNodeByName( string name )
  {        
    for( ;xnode_it!=states.nodes.end();++xnode_it)
    {
      if( (*xnode_it).name == name ) 
      {  
         break;
      }            
    }
    return xnode_it;
  } 
};

#endif

#ifdef IN_T2_TESTHARNESS
  xmlParser parser( &cin );
  parser.useXPATH = true;
  parser.parse();
  node_vector & xnodes = parser.nodeList();
  int i;

  for(i=0;i<xnodes.size();i++)
  {
    xmlNode & node = xnodes[i];
    cout << "Node: " << node.name << endl
         << " contains: " << node.data << endl;
    cout << "node attributes" << endl << "=================" << endl;
    node_attr::iterator x;
    for( x=node.attr.begin(); x!=node.attr.end(); ++x)
    {
      cout << x->first << " = " << x->second << endl;
    }
    cout << "=================" << endl;
  }
  node_map::iterator idx = parser.states.nodemap.begin();
  while( idx != parser.states.nodemap.end() )
  {
    cout << "PATH: " << idx->first <<  " has " << xnodes[idx->second].name << endl;
    idx++; 
  }
  /*
  
    // -- you could cinputing try something like this...

    <this><is><a>E</a><big>BIG</big></is>
    <was><rude>TRULY</rude>
    </was>
    </this>
  */
#endif
