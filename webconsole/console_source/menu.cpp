/*
  menu.cpp
  ===================================
  CGI menu from modules  
    
*/
#include "cgiClass.h"
#include "cgiTemplates.h"
#include "ocDynLib.hpp"
#include "ocFileSys.h"
#include "cgiCookie.h"
#include "connectstring"
#include "../metasite_source/siteLimit.h"

using namespace std;

typedef map< int, string> console_modules;

// global instances
console_modules cmodules;
string relPath = "./modules";

int pickOption( string & module, string & module_name_filter, ostream & client )
{

  if( module_name_filter == "module_base.so" || module_name_filter == module )
  {
    string path = relPath;
    path += "/";
    path += module;  
    // load the library
    ocDynamicLibrary mod1(path);

    if(mod1.IsLibraryLoaded()==false) 
    {
      client << mod1.LastError() << endl;
      return 1;    
    }  

    // load the ordinal function 
    mod1.loadFunction("ordinal");
    if( mod1.IsFunctionLoaded()==false) 
    {
      client << "couldn't find library function. " << mod1.LastError() << mod1.function() << endl;
      return 1;    
    } 
    // invoke it! 
    int ordinal_pos = ((int (*)())mod1.function())();  

    cmodules[ordinal_pos] = module;  
    // libary unloads as mod1 falls out of scope.
  } // end if we want to even mess with this module
}
int runOption( string module, ostream & client )
{

  string path = relPath;
  path += "/";
  path += module;
  // load the library
  ocDynamicLibrary mod1(path);

  if(mod1.IsLibraryLoaded()==false) 
  {
    cout << mod1.LastError() << endl;
    return 1;    
  }  

  // load the presentation function 
  mod1.loadFunction("presentation");
  if( mod1.IsFunctionLoaded()==false) 
  {
    client << "couldn't find library function. " << mod1.LastError() << mod1.function() << endl;
    return 1;    
  } 
  // invoke it! 
  ((void (*)(ostream &))mod1.function())(client);  

  // libary unloads as mod1 falls out of scope.
}
int main( int argc, char ** argv )
{
  cgiScript sc("text/html", false);
  // Insure that the current site is selected
  currentSite( sc.ClientArguments() );
  sc.closeHeader();
  
  cgiCookie cookie;
  cgiTemplates tmplt;
  tmplt.load("./Templates/menu.htmp");  
  string module_name_filter = "module_base.so";
  string cookie_filter = cookie.get("module_name");
  if( sc.ClientArguments().count("module_name") )
  {
    module_name_filter =  sc.ClientArguments()["module_name"].c_str();
  }
  else if( cookie_filter.length() )
  {
    module_name_filter = cookie_filter;
  }
  cgiHtml html;
  {
    cgiHead head;
    head << tmplt.getParagraph("headguts");    
  }
  {
    cgiBody body;
    body << tmplt.getParagraph("heading"); 
    ocFileSys fs;
    if( fs.openDir(relPath.c_str()) )
    {
      ocDirectory & entries = fs.getDirectoryEntries();
      for(int i = 0; i < entries.size(); i++ )
      {
        ocString sotest = entries[i].name;
        if( sotest.regExMatch(".so$") )
        {
          pickOption( sotest, module_name_filter, body );
        }
      }
    }
    console_modules::iterator pos;    
    for( pos=cmodules.begin(); pos != cmodules.end(); ++pos )
    {
      runOption( pos->second, body );
    }
  } // end body
  return 0;
};
