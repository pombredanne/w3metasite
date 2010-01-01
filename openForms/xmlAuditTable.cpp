/*
  xmlAuditTable.cpp
  filterable xml listig service for AuditTable
  generated by w3sys RAD tools

  Note: the xml prefix is now a misnomer for this script.
  Not enough time to do an xslt like I would have liked to.
  Worse, this is an html document fragment.
  Intent is to place this content inside a full document

  David Mc. 7/2007

*/
#include <iostream>
#include <iomanip>
#include "cgiClass.h"
#include "ocTypes.h"
#include "cgiTemplates.h"
#include "connectstring"
#include "pcdata.h"
#include "AuditTable.hpp"
#include "FieldChange.hpp"
#include "Deletion.hpp"

using namespace std;

typedef map < string, string > clookup;
typedef map < string, clookup > clookups;

clookups lookups;

void loadLookups( ocString lookupCtrl )
{
  quickQuery qry;
  openRS & rs = qry.getRS();
  // parse pipes
  do
  {
    ocString lookStr = lookupCtrl.parse("|");
    if( lookStr.length() )
    {
      string name = lookStr.parse(":");
      if( name.length() )
      {
        clookup & tmpLook = lookups[name];
        string sql = lookStr.remainder();
        if( sql.length() )
        {
          if( rs.open(sql) )
          {
            do {
              tmpLook[rs.getField(0).format()]=rs.getField(1).format();
            }while( rs.next());
          }
        }
      }
    }
  } while( lookupCtrl.endOfParse() == false );
}

string lookFirst( string name, string valIn )
{
  string val = valIn;
  clookups::iterator testPos = lookups.find( name );
  if( testPos != lookups.end() )
  {
    clookup & tmpLook = testPos->second;
    clookup::iterator valPos =  tmpLook.find(valIn);
    if( valPos != tmpLook.end() )
    {
      val = valPos->second;
    }
  }
  return val;
}

void emitChangedColumns( llong auditId, cgiScript & script )
{
 FieldChange_Obj FieldChange;
 ocString filter = "auditTable = ";
 filter.append(auditId);
 if( FieldChange.get_data( filter ) )
  {
    script << "<div>Fields Changed: "  << endl;    do {
      script << "<div  id=\"FieldChange" << FieldChange.key() << "\">" << endl;
      script << "  <!-- Table:" << FieldChange.auditTable << " -->" << endl;
      script << "  <div>" << FieldChange.Name << " - <span class='morphed'>" << endl;
      script << "  <em>was</em>: " << lookFirst( FieldChange.Name, FieldChange.Value ) << "</span></div>" << endl;
      script << "</div>" << endl;
    } while( FieldChange.next() );
    script << "</div>"  << endl;
  }
}

void emitDeletedColumns( llong auditId , cgiScript & script )
{
  Deletion_Obj Deletion;
  ocString filter = "auditTable = ";
  filter.append(auditId);
  if( Deletion.get_data( filter ) )
  {
    script << "<div>Fields Deleted: "  << endl;
    do {
      script << "<div id=\"Deletion" << Deletion.key() << "\">" << endl;
      script << "  <!-- Table: " << Deletion.auditTable << " -->" << endl;
      script << "  <div>" << Deletion.Name << " - <span class='morphed'>" << endl;
      script << "  <em>was</em>:" <<lookFirst( Deletion.Name, Deletion.Value )  << "</span></div>" << endl;
      script << "</div>" << endl;
    } while( Deletion.next() );
    script << "</div>"  << endl;
  }
}

int main( int argc, char ** argv)
{
  cgiScript script;
  AuditTable_Obj AuditTable;
  cgiInput & args = script.ClientArguments();
  
  string filter = "";
  if( args.count("filter") ) filter = args["filter"].c_str();
  
  ocString sqlAuthor = "select concat( first,' ',last) from user where id = ";
  if(args.count("authorQuery") ) sqlAuthor = args["authorQuery"].c_str();
  if( args.count("lookups") ) loadLookups(args["lookups"].c_str());

  if( AuditTable.get_data( filter ) )
  {
    script << "<div>Audit: "  << endl;
    try {
    do {
      script << "<div id=\"AuditTable" << AuditTable.key() << "\">Changes: " << endl;
      script << "  <!-- for aDatabase "  << AuditTable.aDatabase << " -->" << endl;
      script << "  <!--  Table: " << endl;
      script << "    " << AuditTable.aTable << endl;
      script << "  Row ID: " << endl;
      script << "    " << AuditTable.aRow << endl;
      script << "   -->" << endl;
      script << "  <div class='Actor_" << AuditTable.Actor << "'>" << endl;
      script << "    " << AuditTable.Author(sqlAuthor) << endl;
      script << "  </div>" << endl;
      script << "  <div>Time: " << endl;
      script << "    " << AuditTable.Timestamp << endl;
      script << "  </div>" << endl;
      script << "  <div>Type: " << endl;
      script << "    " << AuditTable.AuditType() << endl;
      script << "  </div>" << endl;
      if(  AuditTable.auditType == 2 )
      {
        emitChangedColumns( AuditTable.key() , script );
        
      }
      else if(  AuditTable.auditType == 3 )
      {
        emitDeletedColumns( AuditTable.key() , script );
      }
      
      script << "</div>" << endl;
    }  while( AuditTable.next() );

    }
    catch(...)
    {
      script << "An Exception occurred!" << endl;
    }
    script << "</div>"  << endl;
  }
}

#include "read_write_base.cpp"