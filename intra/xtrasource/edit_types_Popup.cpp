/*
  edit_types_Popup.cpp

  User Interface Definition and Popup Implementation for Pop up Dialogs with Data Lists 
  for edit_types.
  
  Copyright (C) 2007 by D.K. McCombs.
  W3 Systems Design

  NEEDS:
  
   Templates/spawninglist.htmp
   Templates/childPane.htmp
   Templates/childdivform.htmp
*/

#include <iostream>
#include <iomanip>
#include "cgiTemplates.h"
#include "connectstring"
#include "forms.h"
#include "ocTypes.h"
#include "ocString.h"
#include "read_write_base.hpp"
#include "forms_base.hpp"
#include "cgiTemplates.h"
#include "edit_types.hpp"
#include "openLogin.h" 


using namespace std;
openLogin oLogin;
 // Include file
#include "sublist_base.hpp"

// Definition

class edit_types_List:  public sublist_base
{

  public:
  // Constructor
    edit_types_List(cgiScript&sc):sublist_base(sc){;}
    ~edit_types_List(){;}
  
    bool list_display( void )
    {
      bool breturn = true;
      string sql =  "select "
          "id, "
          "name, "
          "sequence "
          "from edit_types";

      hotCol=-2;
      editLink = listTemplate.getParagraph("peerlink");
      editLink = editLink.replace("$prog$","edit_types_Popup.cgi");
      string heading = 
          "id| "
          "name| "
          "sequence "
          ;
      emitHeadings(heading,"<h4>edit_types List</h4>");

      getData( sql );
      emitData();
      ocString list_new = listTemplate.getParagraph("peernew");
      ocString url = "edit_types_Popup.cgi";
// TODO: Match the column count to the number of fields
      webIO << list_new.replace("$location",url.c_str()).replace("$cols","3").replace("Item","edit_types");
      emitEnd();
      return breturn;
    }
};
class edit_types_form:  public edit_types_Obj, public forms_base
{
  public:
    edit_types_form(cgiScript & script):edit_types_Obj(),forms_base(script)
    {
      setKey(*this);
    }
    virtual ~edit_types_form(){;}
  
    void form_id_transfer( void )
    {
      llongFXfer( "id", id );
    }
    void form_data_transfer( void )
    {
      stringFXfer( "name", name);
      stringFXfer( "handler", handler);
      intFXfer( "sequence", sequence);
    }
  
    bool dbf_action( string mode, changeMap & changes )
    {
      return db_action( mode, changes );
    }
  
  // implement pure virtual of form display
    bool form_display( void )
    {
      bool breturn = true;
      ocString sql;
      script << makeTop("edit_types_Popup.cgi", "edit_types")
             << formTemplate.getParagraph("advanced_begin");
      script << makeStaticBox("id", "id", id ,"8");
      script << "<br class='clearall'>" << endl;
      script << formTemplate.getParagraph("advanced_end");

      script << makeTextBox("Name", "name", name ,"125","35");
      script << "<br class='clearall'>" << endl;
      script << makeTextArea("Handler", "handler", handler ,"8","80");
      script << "<br class='clearall'>" << endl;
      script << makeTextBox("Sequence", "sequence", sequence ,"8","8");
      script << "<br class='clearall'>" << endl;
      script << makeButtons( key() );
      
      script << makeBottom( m_result ) << endl;
      return breturn;
    }
};


int main( int argcount, char ** args )
{
  cgiScript script( "text/html", false );
  edit_types_form myFrm(script);

  if( oLogin.testLoginStatus() )
  {
    script.closeHeader();
    cgiTemplates pgTemplate;
    pgTemplate.load("Templates/childPane.htmp");
    
    script << ocString(pgTemplate.getParagraph("top"))
        .replaceAll("$heading$","edit types");

    myFrm.loadControlTemplates("Templates/childdivform.htmp");
    myFrm.form_action();
    myFrm.form_display();
    script << "<br class='clearall'>" << endl;
    edit_types_List edit_typesList(script);
    edit_typesList.loadListTemplates("Templates/list.htmp");
    edit_typesList.list_display();
    ocString end = pgTemplate.getParagraph("bottom");
    script << end;
  }
  else
  {
    script.closeHeader();
    script << "<html><body>"
           << "<a href='javascript:window.close()'><h1>You are not logged in</h1></a>"
           << "</body></html>" << endl;
  }
}
// compile implementations here
#include "read_write_base.cpp"
#include "forms_base.cpp"


