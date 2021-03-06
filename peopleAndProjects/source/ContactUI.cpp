/*
  ContactUI.cpp

  User Interface Definition and Implementation 
  for Contact.
  
  Copyright (c) 2006 by D.K. McCombs.
  
  davidmc@w3sys.com
  
  W3 Systems Design Inc.  
  
*/
#include <string>
//#define OPEN_LOGGING_FILE
#include "openLogger.h"
using namespace std;


#include "w3intranet.h"
#include "Contact.hpp"

class Contact_List:  public list_base
{
public: 
  // Constructor
  Contact_List(cgiScript&sc,Session_Obj & session):list_base(sc,session){;}   
  ~Contact_List(){;}
  
  bool list_display( void )
  {
    bool breturn = true;

    hotCol=-2;    
    editLink = listTemplate.getParagraph("hotcolumn");
    editLink = editLink.replace("$prog$","ContactUI.meta");
    // TODO:  Make sure you set the filter by name
    emitFilter( "ContactUI.meta",
                "    <B>FILTER (by Last Name)</B>" );  
    string heading =
      "<a class='sortcol' href='ContactUI.meta?sort=Id'>Id</a>|"                     
      "<a class='sortcol' href='ContactUI.meta?sort=First'>First</a>|"                     
      "<a class='sortcol' href='ContactUI.meta?sort=Last'>Last</a>|"                     
      "<a class='sortcol' href='ContactUI.meta?sort=Prefix'>Prefix</a>|"                     
      "<a class='sortcol' href='ContactUI.meta?sort=Phone'>Phone</a>|"                     
      "<a class='sortcol' href='ContactUI.meta?sort=Mobile'>Mobile</a>|"                     
      "<a class='sortcol' href='ContactUI.meta?sort=Email'>Email</a>|"                     
      "<a class='sortcol' href='ContactUI.meta?sort=Fax'>Fax</a>|"                     
      "<a class='sortcol' href='ContactUI.meta?sort=Home_Phone'>Home_Phone</a>|"                     
      "<a class='sortcol' href='ContactUI.meta?sort=Company'>Company</a>|"                     
      "<a class='sortcol' href='ContactUI.meta?sort=Password'>Password</a>"                     
                    ;
    emitHeadings(heading);
    getFilteredData(
                     "Id, " 
                     "First, " 
                     "Last, " 
                     "Prefix, " 
                     "Phone, " 
                     "Mobile, " 
                     "Email, " 
                     "Fax, " 
                     "Home_Phone, " 
                     "Company, " 
                     "Password " 
                     ,"Contact", 
                     // TODO: Make sure you set the filter by field                    
                     "Last like '$filter$%'" );   
    emitData();
    emitNavigation("ContactUI.meta");         
    emitEnd();
    return breturn;
  }      
}; 

class Contact_form:  public Contact_Obj, public forms_base
{
public: 
  Contact_form(cgiScript & script):Contact_Obj(),forms_base(script){setKey(*this);} 
  virtual ~Contact_form(){;}
  
  void form_id_transfer( void )
  {
    llongFXfer( "Id", Id );
  }
  void form_data_transfer( void )
  {
    stringFXfer( "First", First);
    stringFXfer( "Last", Last);
    stringFXfer( "Prefix", Prefix);
    stringFXfer( "Phone", Phone);
    stringFXfer( "Mobile", Mobile);
    stringFXfer( "Email", Email);
    stringFXfer( "Fax", Fax);
    stringFXfer( "Home_Phone", Home_Phone);
    llongFXfer( "Company", Company);
    stringFXfer( "Password", Password);
 
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
    script << makeTop("ContactUI.meta", "Contact")
           << formTemplate.getParagraph("advanced_begin");
    script << makeStaticBox("Id", "Id", Id ,"8");
    script << "<br class='clearall'>" << endl;
    script << formTemplate.getParagraph("advanced_end");
    
    script << makeTextBox("First", "First", First ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Last", "Last", Last ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Prefix", "Prefix", Prefix ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Phone", "Phone", Phone ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Mobile", "Mobile", Mobile ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Email", "Email", Email ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Fax", "Fax", Fax ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Home_Phone", "Home_Phone", Home_Phone ,"125","35");
    script << "<br class='clearall'>" << endl; 
    sql = "select Id, Name from Company order by Name ";
    
    script << makeComboBox("Company", "Company", Company ,sql);
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Password", "Password", Password ,"125","35");
    script << "<br class='clearall'>" << endl; 
  
    script << makeButtons( key() );        
    script << makeBottom( m_result ) << endl; 
    return breturn;
  }
};

bool intraMain(page & pg ,cgiScript & script)
{

  Contact_form myFrm(script); 
  Contact_List mylist(script,oLogin.Session());  

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
  baseUrl = "ContactUI.meta";  // this is my compiled name!
  // because we are checking
  cgiScript script("text/html",false);
  isSignedOn = check(script);
  script.closeHeader();
  
  /* TODO: Optionally replace intranet.meta with anything you please, or not,
     make sure an associated page entry exists in the metasite database. 
  script.ScriptName() =
    ocString( script.ScriptName().c_str()).replace("ContactUI.meta","intranet.meta").c_str();  
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

