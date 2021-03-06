/*
  
  Copyright 2002 - 2007 by D.K. McCombs.
  ======================================

  Needs: (all relative to invocation)
     Templates/adminPane.htmp,
     Templates/divform.htmp,
     scripts/menumanip.js
     styles/ctrlpanel.js
     Templates/divlist.htmp
     Templates/map.htm
     in pg_images:
     mi_down.png  mi_left.png  mi_mov.png  mi_right.png  mi_up.png  mnu_item_move_background.png
 
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
#include "list_base.hpp"
#include "menu.hpp"
#include "page_menus.h"
#include "openLogin.h"
#include "siteLimit.h"
#include "ocFileSys.h"
#include "flexButtons.hpp"

using namespace std;
openLogin oLogin;


class menu_item_List:  public list_base
{
public: 
  long long page_menu_id;
  long long menu_id;
  long long page_id;
  
  string id, place_order;
  long place_level;
  cgiTemplates movemap;
  
  // Constructor
  menu_item_List(cgiScript&sc,
                 llong id, llong menu_id, llong page_id)
                 :list_base(sc),page_menu_id(id),menu_id(menu_id),page_id(page_id)
  {
    movemap.load("Templates/map.htm");                   
  }  
  ~menu_item_List(){;}
  
  bool list_display( void )
  {
    bool breturn = true;
    editLink = listTemplate.getParagraph("hotcolumn");
    editLink = editLink.replace("$prog$","menu_item_ui.cgi");
    hotCol=3; 
    ocString  sql = "select mi.id, mi.place_order, mi.place_level, l.name "
        "from menu_items mi "
        "inner join links l on l.id = mi.link_id "
                    "where mi.menu_id = ";
    sql.append(menu_id);
    sql += " order by mi.place_order";
    // emitHeadings("Item Name"); 
    getData(sql);
    emitData();    
    emitEnd();
    return breturn;
  }
  void derived_complete_event( void )
  {  
    ocString loco = "menu_item_ui.cgi";
    loco += "?page_menu_id=";
    loco.append(page_menu_id);
    if(menu_id)
    {
      loco += "&menu_id=";
      loco.append(menu_id);
    }
    if(page_id)
    {
      loco += "&page_id=";
      loco.append(page_id);
    }
    ocString adder = listTemplate.getParagraph("list_new");
    webIO << adder.replace("$location",loco.c_str())
                  .replace("$item","Menu Item");
  } 
  
  virtual void sendField( int iField, ocString & td )
  {
     // cache 
     if(iField==0)
     {
       id=rs.getField(iField).format();
     }
     if(iField==1)
     {
       place_order=rs.getField(iField).format();
     }
     if(iField==2)
     {
       place_level=atol(rs.getField(iField).format().c_str());
     }
  }
  
  // Override this to get the group_id in the link
  virtual void sendHotField( int iField, ocString & td )
  {
    ocString linkId = id;
    ocString moveMap =  movemap.getParagraph("movemap");
    ocString level, order;
    ocString FieldName = rs.getField(iField).format();

    level.append(place_level*30);
    order.append(place_order);
    if(menu_id)
    {
      linkId += "&menu_id=";
      linkId.append(menu_id);
    }
    if(page_id)
    {
      linkId += "&page_id=";
      linkId.append(page_id);
    }
    webIO << moveMap.replaceAll("miid",id.c_str())
                    .replace( "$LEVEL$", level.c_str() )
                    .replace( "$ORDER$", order.c_str() )
          << td.replace( "$data$", 
                          editLink.replaceAll( "$key$", linkId.c_str() )
                         .replaceAll( "$col$", FieldName.c_str() ).c_str() );
  }
};



class page_menus_form:  public page_menus_Obj, public forms_base
{
  string name;
public: 
  page_menus_form(cgiScript & script):page_menus_Obj(),forms_base(script)
  {
    setKey(*this);
    if( key() < 1 )
    {
     if( script.ClientArguments().count("page_menu_id") > 0 )
     {
       id =  atoll(script.ClientArguments()["page_menu_id"].c_str());
       key(id);
     }
    }
  } 
  
  void form_id_transfer( void )
  {
    llongFXfer( "id", id );
  }
  void form_data_transfer( void )
  {
    llongFXfer( "site_id", site_id);
    llongFXfer( "menu_id", menu_id);
    llongFXfer( "page_id", page_id);
    intFXfer( "place_order", place_order);
    boolFXfer( "collapsible", collapsible);
    stringFXfer( "name", name);
  } 
  
  bool dbf_action( string mode, changeMap & changes )
  {
    return db_action( mode, changes );
  } 
  
  // implement pure virtual of form display
  bool form_display( void )
  {
    bool breturn = true;
    if ( site_id < 1 )   site_id = atoll(currentSite().c_str());
    script << makeTop("", "Menu Manager")
           << formTemplate.getParagraph("advanced_begin") << endl;
           
    script << makeStaticBox("id", "id", id ,"8");
    script << makeStaticBox("site_id", "site_id", site_id ,"8");
    script << makeStaticBox("menu_id", "menu_id", menu_id ,"8");
    script << makeStaticBox("page_id", "page_id", page_id ,"8");
    script << makeHiddenBox( "place_order", place_order );

    script << "<br class='clearall'>" << endl
           << formTemplate.getParagraph("advanced_end")
           << makeStaticBox("Name","anothername",name,"50")
           << "<br class='clearall'>" << endl;
    script << makeBoolBox("collapsible", "collapsible", collapsible );
    script << "<br class='clearall'>" << endl;   
    script << flexButtons( key(), "", "&nbsp;Update&nbsp;", "", "", "", false, true, false, false, false );
    if (menu_id != 0 )
    {
      script << "<br class='clearall'>" << endl;
      script << "<b>Menu Items:</b><br>";
      menu_item_List list(script,id,menu_id,page_id);
      list.loadListTemplates("Templates/divlist.htmp");  
      list.list_display();
      script << "<br class='clearall'>" << endl;
    }
    script << makeBottom( m_result ) << endl; 
    return breturn;
  }
};

int main( int argcount, char ** args )
{
  cgiScript script( "text/html", false );
  page_menus_form myFrm(script);

  if( oLogin.testLoginStatus() )
  {
    script.closeHeader();
    cgiTemplates pgoo;    
    pgoo.load("Templates/childPane.htmp");
    
    script << ocString(pgoo.getParagraph("top"))
              .replace( "// More Functions?",
                        "</SCRIPT><SCRIPT type=\"text/javascript\" src=\"/scripts/menumanip.js\"></SCRIPT>" );

    myFrm.loadControlTemplates("Templates/childdivform.htmp");  
    myFrm.form_action();  
    myFrm.form_display();
    
    ocString end = pgoo.getParagraph("bottom");   
    if( myFrm.getMode() != "s" )
    {
      script << end.replace("/*_extra_js_*/","  remenu();");
    }
    else
    {
      script << end;
    } 
  }
  else
  {
    script.closeHeader();
    script << oLogin.getLastError() << endl;
    // script.Redirect("signIn.html"); 
  } 
}
// compile implementations here
#include "read_write_base.cpp"
#include "forms_base.cpp"
