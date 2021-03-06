
/*
  cgiTemplates implementation file

members
  ocString unparsed;
  paragraphMap parsed;

*/
#include "cgiTemplates.h"

// default and only constructor
cgiTemplates::cgiTemplates():comments(false)
{
  ;
}

void cgiTemplates::commentsOn(void)
{
  comments=true;
}
void cgiTemplates::commentsOff(void)
{
  comments=false;
}

// non virtual destructor
cgiTemplates::~cgiTemplates()
{
  ;
}

// load and parse the file */
bool cgiTemplates::load ( const char * filename )
{
  bool bRet = false;
  unparsed = "";
  ifstream templateFile( filename );
  if( templateFile )
  {
    char buffer[1024];
    templateFile.getline( buffer, sizeof(buffer) );
    while( templateFile.rdstate() == ios::goodbit )
    {
      if( strlen(buffer) )
      {
        unparsed += buffer;
        unparsed += "\n";
      }
      templateFile.getline( buffer, sizeof(buffer) );
    }
    if( unparsed.length() )
    {
      bRet = true;
      parse();
    }
  }
  return bRet;
}
cgiTemplates & cgiTemplates::operator = ( const cgiTemplates & in )
{
  parsed = in.parsed;
  return *this;
}
bool cgiTemplates::parse( void )
{
  bool bRet = false;
  bool bFirstToken = true;
  unparsed.parseInit();
  ocString element;

  for( element=unparsed.parse("<!--@");
       unparsed.endOfParse() == false;
       element=unparsed.parse("<!--@") )
  {
    /* we dont care about stuff between <!--/@elem1--> and <!--@elem2-->
       we just want the name for the paragraph
    */
    string name = unparsed.parse("-->");

    // now build the closing tag for the next parse
    string close = "<!--/@";
    close += name;
    close += "-->";

    /* now use the built tag to get the paragraph value
       to place in the associatie array
    */
    string value = "";
    if( comments )
    {
      value += "<!-- begin ";
      value += name;
      value += " -->";
    }

    // check first that we have a close marker
    if( unparsed.find(close) != string::npos )
    {
      value += unparsed.parse(close.c_str());
    }
    
    if( comments )
    {
      value += "<!-- end ";
      value += name;
      value += " -->";
    }
    // If no <!--@top--> marker, the default name of the top of the page.
    if( name.length() == 0 &&  bFirstToken ) name = "top";
    
    // add to paragraphs
    parsed[name] = value;
    
    bFirstToken = false;
    bRet = true;
  }
  return bRet;
}


// return the paragraph labeled by 'key
string & cgiTemplates::getParagraph( const string & key )
{
  return parsed[key];
}

string & cgiTemplates::getParagraph( const char * key )
{
  string strKey(key);
  return parsed[strKey];
}

// return the content of the html file unparsed
string & cgiTemplates::getUnparsedHtml( void )
{
  return unparsed;
}
