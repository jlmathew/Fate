/*
MIT (Modified) License

Copyright (c) 2017 jlmathew

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The only restriction to usage is to fully credit the original work, in some manner, 
such as by project name, website, academic reference, documentation, and/or other
non obscured means.
Exceptions to this are allowed by prior permission from the originator (James Mathewson)
is given.  All variants or derivations must give correct accreditation (except as noted
above).

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#ifndef UTILITYHANDLERBASE_H_
#define UTILITYHANDLERBASE_H_

#include <cstdlib>
#include <stdint.h>
#include <ostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <list>
#include <exception>
#include <vector>
#include <map>
#include <assert.h>
#include "EventHandlerBase.h"
#include "IcnName.h"
#include "ContentName.h"
#include "UtilityConfigBase.h"
#include "UtilityExternalModule.h"
#include "UtilityDeviceModule.h"
#include "GlobalModuleBase.h"
#include "BaseStorage.h"
#include "PacketTypeBase.h"

class UtilityExternalModule;
class PktType;

//should be in a storage header, might be ram/disk

class UtilityHandlerBase:public EventHandlerBase
{
protected:
  UtilityHandlerBase ();
public:

  /**
    *
    * \brief 
    * \param COnfigWrapper
    * \see COnfig()
    *
    */
  UtilityHandlerBase (ConfigWrapper & config);  //pugi::xml_node &xmlCOnfig);
  virtual ~ UtilityHandlerBase ();

  /**
    *
    *
    * \param
    * \return 
    */
  virtual bool OnInit (UtilityExternalModule *);
  virtual void DoDelete (const AcclContentName & name);

  virtual void Config (ConfigWrapper & config);

  virtual void Print (std::ostream & os, bool endParen = true) const;

  virtual void Print (std::ostream & os, const AcclContentName & tlvName, double &value) const;

  virtual const dataNameType_t & Name () const;

  static const dataNameType_t & IdName ()
  {
    throw::std::invalid_argument ("Base has no id name");
  }
  virtual void ComputeAndValue (std::list < PktType > &pktList);
  virtual void Compute (const AcclContentName & name);

  virtual void Compute ();

  virtual void OnPktIngress (PktType & data);   //Rx

  virtual void OnPktEgress (PktType & data, const PktTxStatus & status);        //Tx

  virtual void OnPrePktEgress(PktType &data); //Pre Tx, allows routines to modify timers counts on pkts (e.g. how long pkt is fresh)
  
  virtual uint64_t EstMemoryUsed (void) const;

  virtual bool SelfTest(void);

protected:
  bool m_usePrettyPrint;
  UtilityExternalModule *m_externalModule;
  bool m_useAlias;
  bool m_useAliasName;

  std::string m_name;           //name
  double m_defaultValue;        //if a function does not apply, what is the default value?
  bool m_useProxyName;
  StorageClass_t m_storageMethod;
  bool m_attributeInsert;
  double m_defaultMissingReturnValue;

  std::string m_defaultInsertValue;
  uint64_t m_createEntryMask;
  uint64_t m_updateEntryMask;

  std::string m_defaultAttribute;
};

#endif
