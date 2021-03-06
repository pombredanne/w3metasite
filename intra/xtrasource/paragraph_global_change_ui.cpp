/*
  Copyright 2004 by D.K. McCombs.
  =============================== 
  paragraph_global_change_ui.cpp 
  
  webpage that edits paragraphs in html format
     
*/

#include <iostream>
#include <iomanip>
#include "cgiTemplates.h"
#include "paragraph.hpp"
#include "forms.h"

#include "forms_base.hpp"
#include "list_base.hpp"
#include "cgiTemplates.h"
#include "openLogin.h"
#include "../../webconsole/metasite_source/site.hpp"
using namespace std;
openLogin oLogin;



class paragraph_form:  public paragraph, public forms_base
{
  string searchText;
  string replaceText;
  

public: 
  paragraph_form(cgiScript & script):paragraph(),forms_base(script){}
  virtual ~paragraph_form(){;}
  
  void form_id_transfer( void )
  {
    ;
  }
  
  void form_data_transfer( void )
  {
    llongFXfer( "site_id", site_id );
    stringFXfer( "searchText", searchText);
    stringFXfer( "replaceText", replaceText);

  } 
  
  bool dbf_action( string mode, changeMap & changes )
  {
    ocString where = " site_id = ";
    where.append(site_id);
    // do we have a site id and replace and search?
    if(  site_id > 0 && searchText.length() > 0 &&  replaceText.length() > 0 )
    {
      bool hasRecord =  get_data( where );
      while( hasRecord )
      {
        changes["content"] = "content";
        ocString newContent = content;
        content = newContent.replaceAll( searchText, replaceText );
        db_update(changes);
        hasRecord = next();
      }
    }
    return true;
  } 
  
  // Implement pure virtual of form display
  bool form_display( void )
  {
    bool breturn = true;

    script << makeTop("paragraph_ui.cgi", "Global Content Replace:");
    ocString sql = "select id, name from sites";
    script << makeComboBox( "Site Id", "site_id", site_id, sql );
    script << "<br class='clearall'>" << endl;
    script << makeTextBox("Search text","searchText",searchText,"50");
    script << "<br class='clearall'>" << endl

           << makeTextBox("Replacement text","replaceText",replaceText,"50")
           << "<br class='clearall'>" << endl;

      script << "<input type='button' onclick='doUpdate()' name='action' class='action' value='&nbsp;Search and Replace &nbsp;'>";


    script << makeBottom( m_result ) << endl;
    return breturn;
  }

};

int main( int argcount, char ** args )
{
  cgiScript script( "text/html", false );
  paragraph_form myFrm(script); 

  
  if( oLogin.testLoginStatus() )
  {
    script.closeHeader();
    cgiInput & args = script.ClientArguments();
    myFrm.template_tag = args["template_tag"].c_str();
    cgiTemplates pgTemplate;    
    pgTemplate.load("Templates/childPane.htmp");
    
    script << ocString(pgTemplate.getParagraph("top"))
            .replaceAll("$heading$","SITES");

    myFrm.loadControlTemplates("Templates/divform.htmp");
    myFrm.form_action();  
    myFrm.form_display();
    

    ocString bottom = pgTemplate.getParagraph("bottom");
    script << bottom;

  }
  else
  {
    script.Redirect("signIn.html"); 
  } 
}

// compile implementations here
#include "read_write_base.cpp"
#include "forms_base.cpp"
