/*
  tcInfoClassClass.h

  TrustCommerce Merchant Account Path Determination
  ---------------
  Get customer id and password for linking to payment gateway

  2007 D.K.McCombs

*/
#ifndef tc_Info_Class_h
#define tc_Info_Class_h
#include "connectstring.h"
#include "ocString.h"

class tcInfoClass
{
protected:
  // data members
  string m_tccid;
  string m_tcpwd;
public:
  // constructor
tcInfoClass():
  m_tccid(""),
  m_tcpwd("")
  { 
    const char * test = getConnectstring("gtway");
    if( test && strlen(test) )
    {
      ocString oneLiner = test;
      m_tccid = oneLiner.parse(":");
      m_tcpwd= oneLiner.remainder();
    }
  }
  // destructor
  virtual ~tcInfoClass(){;}
  // copy constructor 
  tcInfoClass( const tcInfoClass & in):
  m_tccid(in.m_tccid),
  m_tcpwd(in.m_tcpwd)
  {;}
  // assignment operator
  tcInfoClass & operator = ( const tcInfoClass & in )
  {
    m_tccid = in.m_tccid;
    m_tcpwd = in.m_tcpwd;
    return *this;
  }
  // member access operators  
  // get
  const char * tccid( void ) { return m_tccid.c_str(); }
  // set
  void tccid( const string & in ) { m_tccid = in; }
  
    // get
  const char * tcpwd( void ) { return m_tcpwd.c_str(); }
  // set
  void tcpwd( const string & in ) { m_tcpwd = in; }

};

#endif
