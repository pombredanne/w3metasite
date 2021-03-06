/*
  WorkUI.cpp

  User Interface Definition and Implementation 
  for Work.
  
  Worflow Setup Screen
  
  Copyright (c) 2005 by D.K. McCombs.
  
  davidmc@w3sys.com
  
  W3 Systems Design Inc.  
  
*/
#include <string>
//#define OPEN_LOGGING_FILE
#include "openLogger.h"
using namespace std;
 // added to resolve compatibility issues where this may be a symbolic link to another project
#include "compatible.hpp"

#include "connectstring"
#include "w3intranet.h"
#include "Work.hpp"

class Work_List:  public list_base
{
public: 
  // Constructor
  Work_List(cgiScript&sc,Session_Obj & session):list_base(sc,session){;}   
  ~Work_List(){;}
  
  bool list_display( void )
  {
    bool breturn = true;

    hotCol=-2;    
    editLink = listTemplate.getParagraph("hotcolumn");
    editLink = editLink.replace("$prog$","WorkUI.meta");
    emitFilter( "WorkUI.meta",
                "    <B>FILTER (by Name)</B>" );
    string heading =
                     "<a class='sortcol' href='WorkUI.meta?sort=Id'>Id</a>|"                     
                     "<a class='sortcol' href='WorkUI.meta?sort=Name'>Name</a>|"                     
                     "<a class='sortcol' href='WorkUI.meta?sort=Description'>Description</a>|"                     
                     "<a class='sortcol' href='WorkUI.meta?sort=Query'>Query</a>|"                     
                     "<a class='sortcol' href='WorkUI.meta?sort=User'>User</a>|"                     
                     "<a class='sortcol' href='WorkUI.meta?sort=User_Group'>User_Group</a>|"                     
                     "<a class='sortcol' href='WorkUI.meta?sort=User_Role'>User_Role</a>|"                     
                     "<a class='sortcol' href='WorkUI.meta?sort=URL'>URL</a>|"                     
                     "<a class='sortcol' href='WorkUI.meta?sort=Search'>Search</a>|"                     
                     "<a class='sortcol' href='WorkUI.meta?sort=Sequence'>Sequence</a>" ;
    emitHeadings(heading);
    getFilteredData(
                     "Id, " 
                     "Name, " 
                     "Description, " 
                     "Query, " 
                     "User, " 
                     "User_Group, " 
                     "User_Role, " 
                     "URL, " 
                     "Search, "
                     "Sequence "
                     ,"Work", 
                     "Name like '$filter$%'", " Sequence " );
    emitData();
    emitNavigation("WorkUI.meta");         
    emitEnd();
    return breturn;
  }      
}; 

class Work_form:  public Work_Obj, public forms_base
{
public: 
  Work_form(cgiScript & script):Work_Obj(),forms_base(script){setKey(*this);} 
  virtual ~Work_form(){;}
  
  void form_id_transfer( void )
  {
    llongFXfer( "Id", Id );
  }
  void form_data_transfer( void )
  {
    stringFXfer( "Name", Name);
    stringFXfer( "Description", Description);
    llongFXfer( "Query", Query);
    llongFXfer( "User", User);
    llongFXfer( "User_Group", User_Group);
    llongFXfer( "User_Role", User_Role);
    stringFXfer( "URL", URL);
    stringFXfer( "Search", Search);
    intFXfer( "Sequence", Sequence);
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
    script << makeTop("WorkUI.meta", "Work")
           << formTemplate.getParagraph("advanced_begin");
    script << makeStaticBox("Id", "Id", Id ,"8");
    script << "<br class='clearall'>" << endl;
    script << formTemplate.getParagraph("advanced_end");
    
    script << makeTextBox("Name", "Name", Name ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Description", "Description", Description ,"125","35");
    script << "<br class='clearall'>" << endl; 
    sql = "select Id, Name from Composite_Query where Is_Workflow > 0";
    script << makeComboBox("Query", "Query", Query ,sql, "Choose");

    script << "<a href=\"javascript:jopen('Composite_Query_Popup.cgi?Is_Workflow=1',"
           << "'scrollbars,resizable,width=500,height=300','_blank')\">Add Query</a>";   

    if( Query > 0 )    
    {
      script << "&nbsp; &nbsp; <a href=\"javascript:jopen('Composite_Query_Popup.cgi?Is_Workflow=1&Id=" 
             << Query << "','scrollbars,resizable,width=500,height=300','_blank')\">Update Query</a>";   
    }
    script << "<br class='clearall'>" << endl;
#ifdef ALT_USER_QUERY
    sql = ALT_USER_QUERY;
#else
    sql = "select  id, concat( first, ' ', last ) as Name from users";
#endif
    
    script << makeComboBox("User", "User", User ,sql, "Choose");
    script << "<br class='clearall'>" << endl;
     
    sql = "select id, name from groups";
    script << makeComboBox("User_Group", "User_Group", User_Group ,sql, "Choose");
    script << "<br class='clearall'>" << endl; 
    
    sql = "select id, name from roles";    
    script << makeComboBox("User_Role", "User_Role", User_Role ,sql, "Choose");
    script << "<br class='clearall'>" << endl; 
    
    script << makeTextBox("URL", "URL", URL ,"250","55");
    script << "<br class='clearall'>" << endl;
    
    script << makeTextBox("Search", "Search", Search ,"250","55");
    script << "<br class='clearall'>" << endl;
    
    script << makeTextBox("Sequence", "Sequence", Sequence ,"8","8");
    script << "<br class='clearall'>" << endl;
    
    script << makeButtons( key() );        
    script << makeBottom( m_result ) << endl; 
    return breturn;
  }
};

bool intraMain(page & pg ,cgiScript & script)
{

  Work_form myFrm(script); 
  Work_List mylist(script,oLogin.Session());  

  script.closeHeader();
  cgiTemplates pgTemplate;    
  pgTemplate.load("Templates/adminPane.htmp");

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
  baseUrl = "WorkUI.meta";  // this is my compiled name!
  // because we are checking
  cgiScript script("text/html",false);
  bool isSignedOn = check(script);
  script.closeHeader();
  
  /* TODO: Optionally replace intranet.meta with anything you please, or not,
     make sure an associated page entry exists in the metasite database. */
  script.ScriptName() =
    ocString( script.ScriptName().c_str()).replace("WorkUI.meta","reports").c_str();  
    
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

