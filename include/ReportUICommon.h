/*

  ReportUICommon.h

  Holds the form and the UI for the report admin interfaces


*/

class Report_List:  public list_base
{
public: 
  // Constructor
  Report_List(cgiScript&sc,Session_Obj & session):list_base(sc,session){;}   
  ~Report_List(){;}
   bool list_display( void )
  {
    bool breturn = true;
    
    hotCol=-2;    
    editLink = listTemplate.getParagraph("hotcolumn");
    editLink = editLink.replace("$prog$","ReportUI.meta");
    emitFilter( "ReportUI.meta",
                "    <B>FILTER (by Name)</B>" );  
    string heading =
                     "<a class='sortcol' href='ReportUI.meta?sort=Id'>Id</a>|"
                     "<a class='sortcol' href='ReportUI.meta?sort=Name'>Name</a>|"
                     "<a class='sortcol' href='ReportUI.meta?sort=Category'>Category</a>|"
                     "<a class='sortcol' href='ReportUI.meta?sort=Show_In_List'>Show In List</a>"
                    ;
    emitHeadings(heading);
    getFilteredData( "Id, Name, Category, "
                     "case Show_In_List when 1 then 'Yes' else 'No' end", "Report", "Name like '$filter$%'", "Category, Name" );
    emitData();
    emitNavigation("ReportUI.meta");         
    emitEnd();
    return breturn;
  } 
  
}; 

class Report_Conditioning_List:  public list_base
{
public: 
  ocString Parent_Id;
  // Constructor
  Report_Conditioning_List(cgiScript&sc, Session_Obj & session): list_base(sc,session){;}  
  ~Report_Conditioning_List(){;}
  
  bool list_display( void )
  {
    bool breturn = true;
    string sql =  "select child.Id, "
                  "child.Col,  " 
                  "child.Sort_Pos,  " 
                  "child.Col_Handler,  " 
                  "child.Direction  " 
                  "from Report_Conditioning child inner join "
                  "Report parent on parent.Id = child.Report_Id " ;
    sql += " where child.Report_Id = ";
    sql += Parent_Id;
    sql += " order by child.Col ";
    hotCol=-2;    
    editLink = listTemplate.getParagraph("hotcolumn");
    editLink = editLink.replace("$prog$","Report_Conditioning_Popup.cgi");        
    string heading = "Id|"
                     "Col|" 
                     "Sort_Pos|" 
                     "Col_Handler|" 
                     "Direction";
    
    emitHeadings(heading);    
    getData( sql );   
    emitData();
    ocString list_new = listTemplate.getParagraph("list_new");
    ocString url = "Report_Conditioning_Popup.cgi?Report_Id=";
    url += Parent_Id;
    webIO << list_new.replace("$location",url.c_str()).replace("$item","Report Conditioning");
    emitEnd();
    return breturn;
  }      
}; 

class Report_form:  public Report_Obj, public forms_base
{
public: 
  Report_form(cgiScript & script):Report_Obj(),forms_base(script){setKey(*this);} 
  virtual ~Report_form(){;}
  
  void form_id_transfer( void )
  {
    llongFXfer( "Id", Id );
  }
  void form_data_transfer( void )
  {
    llongFXfer( "Template_Id", Template_Id);
    llongFXfer( "Composite_Query", Composite_Query);
    stringFXfer( "Name", Name);
    llongFXfer( "Media_Type", Media_Type);
    stringFXfer( "Category", Category);
    boolFXfer( "Show_In_List", Show_In_List);
    intFXfer( "Formatting_Modulus", Formatting_Modulus);
    intFXfer( "Items_Per_Page", Items_Per_Page);
  } 
  
  bool dbf_action( string mode, changeMap & changes )
  {
    changes["Template_Id"]="Template_Id"; // force the template update
    changes["Composite_Query"]="Composite_Query";
    return db_action( mode, changes );
  } 
  
  // implement pure virtual of form display
  bool form_display( void )
  {
    bool breturn = true;
    ocString sql; // for combo boxes
    ocString paramLink = "<a href=\"javascript:jopen('Report_Parameters_ui.cgi";
    if( Id )
    {
      paramLink += "?Report_Id=";
      paramLink.append(Id);
    }
    paramLink += "')\">Manage Filter Parameters</a>";
    
    script << makeTop("ReportUI.meta", "Reports" )  << paramLink ;
    script << "<br class='clearall'>" << endl;     
    script << formTemplate.getParagraph("advanced_begin");
    script << makeStaticBox("Id", "Id", Id ,"8");
    script << "<br class='clearall'>" << endl;
    script << formTemplate.getParagraph("advanced_end");
    sql = "select Id, Name from Report_Templates";
    script << makeComboBox("Template_Id", "Template_Id", Template_Id ,sql);

    script << "<a href=\"javascript:jopen('Report_Templates_Popup.cgi',"
            << "'scrollbars,resizable,width=500,height=300','_blank')\">Add Template</a>";

    if( Template_Id )    
    {
      script << "&nbsp; &nbsp; <a href=\"javascript:jopen('Report_Templates_Popup.cgi?Id=" 
             << Template_Id << "','scrollbars,resizable,width=500,height=300','_blank')\">Update Template</a>";   
    }
    script << "<br class='clearall'>" << endl; 
    sql = "select Id, Name from Composite_Query where Is_Workflow <= 0";    
    script << makeComboBox("Composite_Query", "Composite_Query", Composite_Query ,sql);

      script << "<a href=\"javascript:jopen('Composite_Query_Popup.cgi',"
             << "'scrollbars,resizable,width=500,height=300','_blank')\">Add Query</a>";   

     
    if( Composite_Query )    
    {
      script << "&nbsp; &nbsp; <a href=\"javascript:jopen('Composite_Query_Popup.cgi?Id=" 
             << Composite_Query << "','scrollbars,resizable,width=500,height=300','_blank')\">Update Query</a>";   
    }
    
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Name", "Name", Name ,"125","35");
    script << "<br class='clearall'>" << endl;
    sql = "select Id, Name from Report_Type";     
    script << makeComboBox("Media_Type", "Media_Type", Media_Type ,sql,"Pick/Add Media Type");

      script << "<a href=\"javascript:jopen('Report_Type_Popup.cgi',"
             << "'scrollbars,resizable,width=500,height=300','_blank')\">Add Media Type</a>";   

    if( Media_Type )    
    {
      script << "&nbsp; &nbsp; <a href=\"javascript:jopen('Report_Type_Popup.cgi?Id=" 
             << Media_Type << "','scrollbars,resizable,width=500,height=300','_blank')\">Update Media Type</a>";   
    }
    script << "<br class='clearall'>" << endl; 
    script << makeTextBox("Category", "Category", Category ,"85","35");
    script << "<br class='clearall'>" << endl; 
    script << makeBoolBox("Show_In_List", "Show_In_List", Show_In_List );
    script << "<br class='clearall'>" << endl;
    script << makeTextBox("Formatting_Modulus", "Formatting_Modulus", Formatting_Modulus ,"8","8");
    script << "<br class='clearall'>" << endl;
    script << makeTextBox("Items_Per_Page", "Items_Per_Page", Items_Per_Page ,"8","8");
    script << "<br class='clearall'>" << endl; 
    script << makeButtons( key() );        
    if( key() && Composite_Query )
    {
      Report_Conditioning_List cList(script,oLogin.Session());
      cList.Parent_Id.append(key()); 
      cList.loadListTemplates("Templates/spawninglist.htmp"); 
      script << "<h6>Report Conditioning</h6>" << endl;
      cList.list_display();
            
      Query_Result_List qList(script,oLogin.Session());
      qList.Query_Id.append(Composite_Query); 
      qList.loadListTemplates("Templates/spawninglist.htmp"); 
      script << "<h6>Query:</h6>" << endl;
      qList.list_display();        
    } 

    script << makeBottom( m_result ) << endl; 
    return breturn;
  }
};
