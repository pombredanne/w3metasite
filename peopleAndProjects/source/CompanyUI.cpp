/*
  CompanyUI.cpp

  User Interface Definition and Implementation 
  for Company.
  
  Copyright (c) 2006 by D.K. McCombs.
  
  davidmc@w3sys.com
  
  W3 Systems Design Inc.  
  
*/
#include <string>
//#define OPEN_LOGGING_FILE
#include "openLogger.h"
using namespace std;

#include "w3intranet.h"
#include "Company.hpp"
#include "sublist_base.hpp"

// Definition

class Project_List:  public sublist_base
{

  public:
    ocString Parent_Id;
  // Constructor
    Project_List(cgiScript&sc):sublist_base(sc){;}
    ~Project_List(){;}
  
    bool list_display( void )
    {
      bool breturn = true;
      string sql =  "select Id, "
          "Name,  "
          "Estimated_Start,  "
          "Actual_Start,  "
          "Notes,  "
          "Rate,  "
          "PO,  "
          "Estimate  "
          "from Project where Company = " ;
      sql +=  Parent_Id;
      hotCol=-2;
      editLink = listTemplate.getParagraph("peerlink");
      editLink = editLink.replace("$prog$","ProjectUI.meta");
      string heading = "Id|"
          "Name|"
          "Estimated_Start|"
          "Actual_Start|"
          "Notes|"
          "Rate|"
          "PO|"
          "Estimate"
          ;
    
      emitHeadings(heading);
    
      getData( sql );
      emitData();
      ocString list_new = listTemplate.getParagraph("peernew");
      ocString url = "ProjectUI.meta?Company=";
      url += Parent_Id;
      webIO << list_new.replace("$location",url.c_str()).replace("$cols","7");
      emitEnd();
      return breturn;
    }
}; 
class Company_List:  public list_base
{
public: 
  // Constructor
  Company_List(cgiScript&sc,Session_Obj & session):list_base(sc,session){;}   
  ~Company_List(){;}
  
  bool list_display( void )
  {
    bool breturn = true;

    hotCol=-2;    
    editLink = listTemplate.getParagraph("hotcolumn");
    editLink = editLink.replace("$prog$","CompanyUI.meta");
    emitFilter( "CompanyUI.meta",
                "    <B>FILTER (by Name)</B>" );  
    string heading =
      "<a class='sortcol' href='CompanyUI.meta?sort=Id'>Id</a>|"                     
      "<a class='sortcol' href='CompanyUI.meta?sort=Name'>Name</a>|"                     
      "<a class='sortcol' href='CompanyUI.meta?sort=Address1'>Address1</a>|"                     
      "<a class='sortcol' href='CompanyUI.meta?sort=Address2'>Address2</a>|"                     
      "<a class='sortcol' href='CompanyUI.meta?sort=City'>City</a>|"                     
      "<a class='sortcol' href='CompanyUI.meta?sort=State'>State</a>|"                     
      "<a class='sortcol' href='CompanyUI.meta?sort=Zip'>Zip</a>|"                     
      "<a class='sortcol' href='CompanyUI.meta?sort=Country'>Country</a>|"                     
      "<a class='sortcol' href='CompanyUI.meta?sort=Notes'>Notes</a>|"                     
      "<a class='sortcol' href='CompanyUI.meta?sort=Last_Contacted'>Last_Contacted</a>|"                     
      "<a class='sortcol' href='CompanyUI.meta?sort=Open_PO'>Open_PO</a>"                     
                    ;
    emitHeadings(heading);
    getFilteredData(
                     "Id, " 
                     "Name, " 
                     "Address1, " 
                     "Address2, " 
                     "City, " 
                     "State, " 
                     "Zip, " 
                     "Country, " 
                     "Notes, " 
                     "Last_Contacted, " 
                     "Open_PO " 
                     ,"Company", 
                     "Name like '$filter$%'", "Name" );
    emitData();
    emitNavigation("CompanyUI.meta");         
    emitEnd();
    return breturn;
  }

  
}; 

class Company_form:  public Company_Obj, public forms_base
{
public: 
  Company_form(cgiScript & script):Company_Obj(),forms_base(script){setKey(*this);} 
  virtual ~Company_form(){;}
  
  void form_id_transfer( void )
  {
    llongFXfer( "Id", Id );
  }
  void form_data_transfer( void )
  {
    stringFXfer( "Name", Name);
    llongFXfer( "Key_Contact", Key_Contact);
    stringFXfer( "Address1", Address1);
    stringFXfer( "Address2", Address2);
    stringFXfer( "City", City);
    stringFXfer( "State", State);
    stringFXfer( "Zip", Zip);
    stringFXfer( "Country", Country);
    stringFXfer( "Notes", Notes);
    dateFXfer( "Last_Contacted", Last_Contacted);
    llongFXfer( "Status", Status);
    stringFXfer( "Open_PO", Open_PO);
 
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
    script << makeTop("CompanyUI.meta", "Company")
           << formTemplate.getParagraph("advanced_begin");
    script << makeStaticBox("Id", "Id", Id ,"8");
    script << "<br class='clearall'>" << endl;
    script << formTemplate.getParagraph("advanced_end");
    
    script << makeTextBox("Name", "Name", Name ,"125","35");
    script << "<br class='clearall'>" << endl; 
    
    sql = "select Id, concat(Last, ', ', First) as Name from Contact";
    if( Id )
    {
      sql += " where Company = ";
      sql.append(Id);
    }
    sql += " order by Last ";
    script << makeComboBox("Key_Contact", "Key_Contact", Key_Contact ,sql);
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Address1", "Address1", Address1 ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Address2", "Address2", Address2 ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("City", "City", City ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("State", "State", State ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Zip", "Zip", Zip ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Country", "Country", Country ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Notes", "Notes", Notes ,"125","35");
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Last_Contacted", "Last_Contacted", Last_Contacted ,"24","24");
    script << "<br class='clearall'>" << endl;
    ocString links;
    if( Status )
    {
      links = "   <a href='javascript:jopen(\"Status_Popup.cgi?Id=";
      links.append(Status);
      links += "\",\"resizable,width=400,height=200\",\"stages\")'>Edit</a>";
    }
    links += "   <a  href='javascript:jopen(\"Status_Popup.cgi?Class=Company\""
        ",\"resizable,width=400,height=200\",\"stages\")'>Add</a>";
    sql = "select Id, Name from Status  where Class='Company' order by Sequence";
    script << ocString(makeComboBox("Status", "Status", Status ,sql))
        .replace("<!--eogc-->",links);
    script << "<br class='clearall'>" << endl;

    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Open_PO", "Open_PO", Open_PO ,"125","35");
    script << "<br class='clearall'>" << endl; 
  
    script << makeButtons( key() );
    if( key() )
    {
      script << "<br class='clearall'>" << endl;
      Project_List ProjectList(script);
      ProjectList.Parent_Id.append(key());
      ProjectList.loadListTemplates("Templates/spawninglist.htmp");
      script << "<div class='label'>Projects:</div>" << endl;
      ProjectList.list_display();
    }         
    script << makeBottom( m_result ) << endl; 
    return breturn;
  }
};

bool intraMain(page & pg ,cgiScript & script)
{

  Company_form myFrm(script); 
  Company_List mylist(script,oLogin.Session());  

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
  baseUrl = "CompanyUI.meta";  // this is my compiled name!
  // because we are checking
  cgiScript script("text/html",false);
  isSignedOn = check(script);
  script.closeHeader();
  
  /* TODO: Optionally replace intranet.meta with anything you please, or not,
     make sure an associated page entry exists in the metasite database. 
  script.ScriptName() =
    ocString( script.ScriptName().c_str()).replace("CompanyUI.meta","intranet.meta").c_str();  
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

