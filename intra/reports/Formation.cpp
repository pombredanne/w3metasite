/*
  Formation.cpp

  Web Implementation for Form Reports
  
  Copyright (c) 2006 & 1/2 by D.K. McCombs.

  davidmc@w3sys.com

  W3 Systems Design Inc.  

*/
#include <string>
//#define OPEN_LOGGING_FILE
#include "openLogger.h"
using namespace std;
 // added to resolve compatibility issues where this may be a symbolic link to another project
#include "compatible.hpp"   
 #define NO_INSERT_BUTTON  
#define NO_DELETE_BUTTON
#include "connectstring"
#include "w3intranet.h"
#include "reform_base.hpp"
 // Set static value
bool Report_Parameters_form::hasParamValues = false;

void paramForm(cgiScript & script, Report_Parameters_form & param_form, string reportName )
{

  cgiTemplates pgTemplate;
  
  if( ! pgTemplate.load("Templates/reportPane.htmp") )
  {
    script << "<h2>could not load Templates/reportPane.htmp</h2>" << endl;
  }
  
  script << ocString(pgTemplate.getParagraph("top"))
      .replaceAll("$heading$","Reports");
  
  if( !param_form.loadControlTemplates("Templates/repform.htmp") )
  {
    {
      script << "<h2>could not load Templates/repform.htmp</h2>" << endl;
    }
  }
  param_form.reportName = reportName;
  param_form.form_display();
  ocString end = pgTemplate.getParagraph("bottom");
  script << end;
}
// if the mimetype is application/pdf then ...
bool redirectToHTMLDoc( cgiScript & script, string mimeType )
{
  bool doRedirect = false;
  ocString qs = script.QueryString().c_str();
  doRedirect = ( mimeType ==  "application/pdf" && qs.regExMatch("isPDF") == false );
  if( doRedirect )
  { 
    cgiInput & args = script.ClientArguments();
    // build the redirect string
    string redir = "/htmldoc.cgi";
    redir += script.ScriptName().c_str();
    redir += "?isPDF=1";
    if(  args.TheMap().size() > 0 )
    {
              
      for(  queryStringMap::iterator pos = args.TheMap().begin();
            pos != args.TheMap().end();
        )
      {
        redir += "&";
        redir += pos->first;
        redir += "=";
        redir += pos->second.c_str();
        ++pos;
        if( pos != args.TheMap().end() )
        {
          ocString paramTest = pos->first;
          if( paramTest == "xmldirections" )
          {
            break; // done
          }
        }
      }
    }
    script.Redirect( redir.c_str() );
    script.closeHeader();
  }
  return doRedirect;
}
void showErr( cgiScript & script, string err )
{
  script.closeHeader();
  script << err << endl;
}
// Standard main function
int main(int argc, char ** argv)
{  
  baseUrl = "Formation.meta";  // this is my compiled name!

  // because we are checking
  cgiScript script("text/html",false);
  bool isSignedOn = check(script);


  if( isSignedOn )
  {
    // there must be a report id
    if (script.ClientArguments().count("Id")==1)
    {
      string strId = script.ClientArguments()["Id"].c_str();
      if( strId.length() )
      {
        Report_Obj report;
        report.Id =  atoll(strId.c_str());
        report.key(report.Id);
        if( report.get_data() )
        {

          Report_Parameters_form param_form(script);
          param_form.Report_Id = report.Id;
          param_form.get();
          if( param_form.haveFormValues() == false )
          {
            script.closeHeader();
            paramForm(script,param_form,report.Name);
          }
          else if( ! redirectToHTMLDoc( script, report.getMediaType() ) )
          {
            string path = "reportTemplates/" + report.getTemplatePath();
            string sql = report.getCompositeQuery();
            sql = param_form.paramFilters( sql );
            reform_base basis(script);
            
            // Unbelievably after 2 years we finally add column type formatting
            basis.addColumnHandlers( report.Id );
            
            basis.loadTemplate(path);
            if( basis.getData(sql) )
            {
              script.closeHeader();
              basis.emitData();
            } else showErr( script, "could not get report data" );
          }  // no error here
        } else showErr(script, "could not get report");
      } else showErr(script, "no report id val" );
    } else showErr(script, "no report id var" );
  } else showErr(script, "not signed in" );
  return 0;
}

// compile implementations here
#include "read_write_base.cpp"
#include "forms_base.cpp"
