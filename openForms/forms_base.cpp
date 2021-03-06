/*

forms_base.cpp
================
Forms Base Implementation

*/
#include "forms_base.hpp"
#include "ocXML.h"

forms_base::forms_base(cgiScript & in):script(in)
{
  m_warn = "<br>";
  getInstructions();  
}

forms_base::~forms_base()
{
  ;
} 

bool forms_base::getInstructions( void )
{
  bool breturn = false;
  
  if( script.ClientArguments().count("xmldirections") )
  {     
    string xml = script.ClientArguments()["xmldirections"].c_str();
    xmlParser parser(xml);
    parser.parse();
    node_vector & xnodes = parser.nodeList();
    int i;
    for(i=0;i<xnodes.size();i++)
    {
      xmlNode & node = xnodes[i];
      if( node.name == "change" )
      {
        changes[node.data]=node.data;
      }
      else if( node.name == "mode" )
      {
        // the last one wins here
        mode = node.data;
        breturn = true;
      } 
    }
  }
  // error handling:
  //  cookie mode and mode should always agree, 
  //  if not set to safe action
  cgiCookie cookie;
  string cookie_mode = cookie.get("mode");
  if( mode != cookie_mode )
  {    
    mode = "s";    
  }
  return breturn;
}

// implement pure virtual of form action
bool forms_base::form_action( void )
{  
  // if not reset or new mode, move id   
  if( mode != "r" && mode != "n" )
  {
    form_id_transfer();
  }
  // if not reset mode, move 
  if( mode != "r" )
  {
    form_data_transfer();
  }      
  return dbf_action( mode, changes );
}
     
// load template for forms controls
bool forms_base::loadControlTemplates( string path )
{
  return formTemplate.load(path.c_str());
}
string forms_base::makeButtons( long long key,
                                string iLabel,
                                string uLabel, 
                                string dLabel )
{
  ocString ret = formTemplate.getParagraph("button_group");  
  return ret.replace("$buttons$",formButtons( key,iLabel,uLabel,dLabel ).c_str());
} 
string forms_base::makeTop(string uri, string name )
{
  ocString ret = formTemplate.getParagraph("form_begin");  
  return ret.replace("$action$",uri.c_str()).replaceAll("$name$",name.c_str());
} 
string forms_base::makeBottom( string status  )
{
  ocString ret = formTemplate.getParagraph("form_end");
  if( m_warn != "<br>" )  status += m_warn; 
  return ret.replace("$status$",status.c_str());
} 
  
// form to class property exchange methods
void forms_base::stringFXfer( string in, string & out )
{
  if( script.ClientArguments().count(in.c_str()) )
  {
    out=script.ClientArguments()[in.c_str()].c_str();
  }
}
void forms_base::llongFXfer( string in, long long & out )
{ 
  if( script.ClientArguments().count(in.c_str()) )
  {
    out=atoll(script.ClientArguments()[in.c_str()].c_str());
  }  
}
void forms_base::longFXfer( string in, long & out )
{ 
  if( script.ClientArguments().count(in.c_str()) )
  {
    out=atol(script.ClientArguments()[in.c_str()].c_str());
  }  
}
void forms_base::unfmtLongFXfer( string in, long & out )
{ 
  if( script.ClientArguments().count(in.c_str()) )
  {
    ocString fixUp = script.ClientArguments()[in.c_str()].c_str();
    out=atol(fixUp.replaceAll(",","").c_str());
  }  
}
void forms_base::intFXfer( string in, int & out )
{ 
  if( script.ClientArguments().count(in.c_str()) )
  {
    out=atoi(script.ClientArguments()[in.c_str()].c_str());
  }  
}
void forms_base::shortFXfer( string in, short & out )
{ 
  if( script.ClientArguments().count(in.c_str()) )
  {
    out=atoi(script.ClientArguments()[in.c_str()].c_str());
  }  
}
void forms_base::doubleFXfer( string in, double & out )
{
  if( script.ClientArguments().count(in.c_str()) )
  {     
    out=atof( script.ClientArguments()[in.c_str()].c_str());
  }
}
void forms_base::moneyFXfer( string in, money & out )
{
  if( script.ClientArguments().count(in.c_str()) )
  {
    ocString tmpVal = script.ClientArguments()[in.c_str()].c_str();
    double val = atof( tmpVal.replaceAll("$","").replaceAll(",","").c_str() );
    out=val;
  }
}
void forms_base::boolFXfer( string in, bool & out )
{
  out = false;
  if( script.ClientArguments().count(in.c_str()) )
  {
    out=true;
  }
}

void forms_base::dateFXfer( string in, oc_date & out )
{
  if( script.ClientArguments().count(in.c_str())&&  script.ClientArguments()[in.c_str()].length() )
  out.parse( script.ClientArguments()[in.c_str()].c_str() ); // date using locale
}

void forms_base::dateFXfer( string in, time_date & out )
{
  if( script.ClientArguments().count(in.c_str()) &&  script.ClientArguments()[in.c_str()].length())
  { // time_date using locale
    out.parse( script.ClientArguments()[in.c_str()].c_str() ,"%m/%d/%Y  %H:%M" );
  }
}

void forms_base::timeFXfer( string in, oc_time & out )
{
  if( script.ClientArguments().count(in.c_str()) &&  script.ClientArguments()[in.c_str()].length() )
  {
    out.setFormat("%I:%M %p");
    out.parse( script.ClientArguments()[in.c_str()].c_str() );
  }
}

string forms_base::makeEmailBox( string label, string name, string value, 
                                 string size, string length)
{
  ocString ret = formTemplate.getParagraph("control_group");    
  ocTextbox * pText = emailEntry(  name,  size, length );    
  if( pText )
  {
    pText->setContent(value);      
    ret = ret.replace("$label$",label.c_str()).replace("$name$",name.c_str())
             .replace("$control$", pText->getHtml().c_str());
    delete pText;
  }
  return ret;
}


string forms_base::makePasswordBox( string label, string name, 
                                    string value, string size)
{
  ocString ret = formTemplate.getParagraph("control_group");    
  ocPassword * pText = pswdEntry(  name,  size );    
  if( pText )
  {
    pText->setContent(value);      
    ret = ret.replace("$label$",label.c_str()).replace("$name$",name.c_str())
             .replace("$control$", pText->getHtml().c_str());
    delete pText;
  }
  return ret;
}                          
   
string forms_base::makeZipBox( string label, string name, 
                  string value, string size)  
{
  ocString ret = formTemplate.getParagraph("control_group");    
  ocTextbox * pText = zipEntry(  name,  size );    
  if( pText )
  {
    pText->setContent(value);      
    ret = ret.replace("$label$",label.c_str()).replace("$name$",name.c_str())
             .replace("$control$", pText->getHtml().c_str());
    delete pText;
  }
  return ret;
} 
string forms_base::makePhoneBox( string label, string name, string value, string size)
{
  ocString ret = formTemplate.getParagraph("control_group");    
  ocTextbox * pText = phoneEntry(  name,  size );    
  if( pText )
  {
    pText->setContent(value);      
    ret = ret.replace("$label$",label.c_str()).replace("$name$",name.c_str())
             .replace("$control$", pText->getHtml().c_str());
    delete pText;
  }
  return ret;
}

// a text box in the file group
string forms_base::makeFileBox( string label, string name, string value, string size) 
{
  ocString ret = formTemplate.getParagraph("file_group");    
  ocTextbox * pText = textEntry( name, size, size );      
  if( pText )
  {
    pText->setContent(value);      
    ret = ret.replace("$label$",label.c_str()).replace("$name$",name.c_str())
             .replace("$control$", pText->getHtml().c_str())
             .replace("$id$",name.c_str());
    delete pText;
  }
  return ret;
}

// a real file control
string forms_base::makeFileBox( string label, string name, string value, string size, string maxlen, string attr)
{
  ocString ret = formTemplate.getParagraph("control_group");
  string allAttr = " onChange='ctrlChanged(this)' ";
  allAttr += attr;
  ocFilebox * pText = new ocFilebox( name, size, maxlen, allAttr );
  if( pText )
  {
    string ctrlText = pText->getHtml();
    ctrlText += " : ";
    ctrlText += value;
    ret = ret.replace("$label$",label.c_str()).replace("$name$",name.c_str())
             .replace("$control$", ctrlText.c_str())
             .replace("$id$",name.c_str());
    delete pText;
  }
  return ret;
}

string forms_base::makeMetaBox( string label, string name, 
                                string value, string size,
                                string metaFile, // defaulted
                                string context ) // defaulted
{
  ocString ret = formTemplate.getParagraph("meta_group");    
  ocTextbox * pText = textEntry( name, size, size );      
  ocString jopen_url="metatagPick.cgi";
  if( metaFile.length() )
  {
    jopen_url += "?template_path=";
    jopen_url += metaFile;
    if( context.length() )
    {
      jopen_url += "&context=";
      jopen_url += context;
    }
  }
  else if(context.length() )
  {
    jopen_url += "?context=";
    jopen_url += context;
  }
  if( pText )
  {
    pText->setContent(value);      
    ret = ret.replace("$label$",label.c_str()).replace("$name$",name.c_str())
             .replace("$control$", pText->getHtml().c_str())
             .replace("$id$",name.c_str())
             .replace("metatagPick.cgi",jopen_url.c_str());
    delete pText;
  }
  return ret;  
}                                                                     
string forms_base::makeImageBox( string label, string name, 
                                 string value, string size)
{
  ocString ret = formTemplate.getParagraph("image_group");    
  ocTextbox * pText = textEntry( name, "125", size );      
  if( pText )
  {
    pText->setContent(value);      
    ret = ret.replace("$label$",label.c_str()).replace("$name$",name.c_str())
             .replace("$control$", pText->getHtml().c_str())
             .replace("$id$",name.c_str());
    delete pText;
  }
  return ret;
}                       
string forms_base::makeTextArea( string label, string name, 
                  string value, string rows, string cols, string altTemplate)  
{
  ocString ret = altTemplate;
  if( ret.length() == 0 ) ret = formTemplate.getParagraph("wide_group");
  ocTextarea * pText = areaEntry(  name,  rows, cols );    
  if( pText )
  {
    pText->setContent(value);      
    ret = ret.replace("$label$",label.c_str()).replace("$name$",name.c_str())
             .replace("$control$", pText->getHtml().c_str());
    delete pText;
  }
  return ret;
} 
string forms_base::makeStateBox( string label, string name, string value)
{
    ocString ret = formTemplate.getParagraph("control_group");
    ocSelect * pCombo = cmboManualEntry( name,  
      "Alabama=AL,Alaska=AK,Arizona=AZ,Arkansas=AR,California=CA,Colorado=CO,Connecticut=CT,Washington DC=DC,"
      "Delaware=DE,Florida=FL,Georgia=GA,Hawaii=HI,Idaho=ID,Illinois=IL,Indiana=IN,Iowa=IA,Kansas=KS,Kentucky=KY,"
      "Louisiana=LA,Maine=ME,Maryland=MD,Massachusetts=MA,Michigan=MI,Minnesota=MN,Mississippi=MS,Missouri=MO,"
      "Montana=MT,Nebraska=NE,Nevada=NV,New Hampshire=NH,New Jersey=NJ,New Mexico=NM,New York=NY,North Carolina=NC,"
      "North Dakota=ND,Ohio=OH,Oklahoma=OK,Oregon=OR,Pennsylvania=PA,Puerto Rico=PR,Rhode Island=RI,South Carolina=SC,"
      "South Dakota=SD,Tennessee=TN,Texas=TX,Utah=UT,Vermont=VT,Virginia=VA,Washington=WA,West Virginia=WV,Wisconsin=WI,"
      "Wyoming=WY","",value);    
    if( pCombo )
    {
      pCombo->setContent(value);      
      ret = ret.replace("$label$",label.c_str()).replace("$name$",name.c_str())
               .replace("$control$", pCombo->getHtml().c_str());
      delete pCombo;
    }
    return ret;
}
     
string forms_base::makeBoolBox( string label, string name, bool value )
{
  ocString ret = formTemplate.getParagraph("control_group");
  ocBoolbox * pbBox = boolBox( name, name );
  if( pbBox )
  {
    string strValue = value?"1":"0";
    pbBox->setContent(strValue);
    ret = ret.replace("$label$",label.c_str()).replace("$name$",name.c_str())
             .replace("$control$", pbBox->getHtml().c_str());
  } 
  return ret; 
}


