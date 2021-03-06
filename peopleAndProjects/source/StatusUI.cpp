/*
  StatusUI.cpp

  User Interface Definition and Implementation 
  for Status.
  
  Copyright (c) 2006 by D.K. McCombs.
  
  davidmc@w3sys.com
  
  W3 Systems Design Inc.  
  
*/
#include <string>
//#define OPEN_LOGGING_FILE
#include "openLogger.h"
using namespace std;


#include "w3intranet.h"
#include "Status.hpp"

class Status_List:  public list_base
{
public: 
  // Constructor
  Status_List(cgiScript&sc,Session_Obj & session):list_base(sc,session){;}   
  ~Status_List(){;}
  
  bool list_display( void )
  {
    bool breturn = true;

    hotCol=-2;    
    editLink = listTemplate.getParagraph("hotcolumn");
    editLink = editLink.replace("$prog$","StatusUI.meta");
    // TODO:  Make sure you set the filter by name
    emitFilter( "StatusUI.meta",
                "    <B>FILTER (by Class)</B>" );  
    string heading =
                     "<a class='sortcol' href='StatusUI.meta?sort=Id'>Id</a>|"                     
                     "<a class='sortcol' href='StatusUI.meta?sort=Sequence'>Sequence</a>|"                     
                     "<a class='sortcol' href='StatusUI.meta?sort=Class'>Class</a>|"                     
                     "<a class='sortcol' href='StatusUI.meta?sort=Name'>Name</a>|"                     
                     "<a class='sortcol' href='StatusUI.meta?sort=Prog_Enum'>Prog_Enum</a>"                     
                    ;
    emitHeadings(heading);
    getFilteredData(
                     "Id, " 
                     "Sequence, " 
                     "Class, " 
                     "Name, " 
                     "Prog_Enum " 
                     ,"Status", 
                     // TODO: Make sure you set the filter by field                    
                     "Class like '$filter$%'" );   
    emitData();
    emitNavigation("StatusUI.meta");         
    emitEnd();
    return breturn;
  }      
}; 

class Status_form:  public Status_Obj, public forms_base
{
public: 
  Status_form(cgiScript & script):Status_Obj(),forms_base(script){setKey(*this);} 
  virtual ~Status_form(){;}
  
  void form_id_transfer( void )
  {
    llongFXfer( "Id", Id );
  }
  void form_data_transfer( void )
  {
    longFXfer( "Sequence", Sequence);
    stringFXfer( "Class", Class);
    stringFXfer( "Name", Name);
    longFXfer( "Prog_Enum", Prog_Enum);
 
  } 
  
  bool dbf_action( string mode, changeMap & changes )
  {
    return db_action( mode, changes );
  } 
  
  // implement pure virtual of form display
  bool form_display( void )
  {
    bool breturn = true;
    ocString sql; // for combo boxes
    // TODO: Set the sql for combos, replace any fields with special controls, properly set lengths.
    script << makeTop("StatusUI.meta", "Status")
           << formTemplate.getParagraph("advanced_begin");
    script << makeStaticBox("Id", "Id", Id ,"8");
    script << "<br class='clearall'>" << endl;
    script << makeTextBox("Prog_Enum", "Prog_Enum", Prog_Enum ,"8","8");
    script << "<br class='clearall'>" << endl;
    
    script << formTemplate.getParagraph("advanced_end");
    
    script << makeTextBox("Sequence", "Sequence", Sequence ,"8","8");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Class", "Class", Class ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Name", "Name", Name ,"125","35");
    script << "<br class='clearall'>" << endl; 
 
  
    script << makeButtons( key() );        
    script << makeBottom( m_result ) << endl; 
    return breturn;
  }
};

bool intraMain(page & pg ,cgiScript & script)
{

  Status_form myFrm(script); 
  Status_List mylist(script,oLogin.Session());  

  script.closeHeader();
  
  myFrm.loadControlTemplates("Templates/divform.htmp");  
  myFrm.form_action();  
  myFrm.form_display();

  mylist.loadListTemplates("Templates/navlist.htmp");  
  mylist.list_display();
 
  return true;
}

// Standard main function
int main(int argc, char ** argv)
{  
  baseUrl = "StatusUI.meta";  // this is my compiled name!
  // because we are checking
  cgiScript script("text/html",false);
  isSignedOn = check(script);
  script.closeHeader();
  
  /* TODO: Optionally replace intranet.meta with anything you please, or not,
     make sure an associated page entry exists in the metasite database. 
  script.ScriptName() =
    ocString( script.ScriptName().c_str()).replace("StatusUI.meta","intranet.meta").c_str();  
  */  
  page pg(script);  
  
  writelog( "load page" );  
  if( pg.load() )
  {    
    writelog( "instance of page control" );
    page_control ctrl(pg);     
    ctrl.addOp ( "servicelist",  new servicelist_functor(pg,script,isSignedOn) );    
    ctrl.addOp ( "userinterface",  new form_functor(pg,script,isSignedOn) );    
    writelog( "page control emit" );
    ctrl.emit();    
  }
  else
  {
    script << "<html><head><title>Page Unavailable</title></head>" << endl
           << "<body><h1>Sorry</h1>" << endl
           << "<p>The page you requested is not available</p></body></html>";
  } 
  return 0;
}

// compile implementations here
#include "read_write_base.cpp"
#include "forms_base.cpp"

