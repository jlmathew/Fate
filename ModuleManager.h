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
#ifndef MODULEMANAGERBASE_H_
#define MODULEMANAGERBASE_H_

#include <vector>
#include <queue>
#include "UtilityHandlerBase.h"
#include "UtilityFunctionGenerator.h"
#include <set>
#include "UtilityExternalModule.h"
#include "RangeData.h"
#include "ContentName.h"
class NodeManager; 
class AsyncEvent;

//class UtilityHandlerBase;
#define LOG( A, ...) { if(m_log) {m_log->log(A, __VA_ARGS__);} }

class ModuleManager:public UtilityHandlerBase
{
public:
  ModuleManager ();

  ModuleManager (ConfigWrapper & config);
  virtual ~ ModuleManager ();


  virtual bool OnInit (UtilityExternalModule *);

  //set of names to whitelist (blacklist all others)
  virtual void Config (ConfigWrapper & config, std::set<AcclContentName> *names=nullptr, bool whitelist=true);

  virtual const std::string & Name () const;

  virtual double Value (const AcclContentName & name) const;

  virtual bool AddConfig (UtilityHandlerBase * uc, uint16_t position = 0);

  //delete Configs by name or number

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("ModuleManager");
      return idName;
  }

  //store connectivity

  virtual void OnPktIngress (PktType & interest);       //Rx
  virtual void OnPktEgress (PktType & data, const PktTxStatus & status);

  virtual void DoDelete (const AcclContentName & name);

  virtual void ComputeAndValue (std::list < PktType > &pktList);
  virtual void StoreActionsDone (const std::list < StoreEvents > &list);
  void DoStoreActions (const std::list < StoreEvents > &list);

  virtual void Print (std::ostream & os) const;
  virtual void Print (std::ostream & os, const AcclContentName & Name, double &value) const;
  virtual void Compute ();
  virtual void Compute (const AcclContentName & name);
  virtual void OnAsyncEvent (AsyncEvent & eventType);
  virtual UtilityHandlerBase *GetUtilityBlock (const dataNameType_t & name);
  virtual UtilityHandlerBase *GetUtilityBlock (uint32_t index);
  virtual UtilityHandlerBase *RemoveUtilityBlock (const dataNameType_t & name);
  virtual UtilityHandlerBase *RemoveUtilityBlock (uint32_t index);
  virtual void SetUtilityBlock (UtilityHandlerBase *, uint32_t order);

  virtual uint32_t retNumUtilityBlocks ();

  virtual void LogStats (std::ostream & os);
  virtual bool LocalStoreDelete (const AcclContentName & name);

  virtual void GetPacketsByValue (const RangeData < double >&rng, std::list < std::pair< double, AcclContentName> > &list);
  virtual void GetLowestNPackets (uint64_t lowestN, std::list < std::pair< double, AcclContentName> > &list);

  virtual void GetDebugInfo(PktType &packet) const;
  virtual bool SelfTest(void);
  virtual void LocalStoreDelete(const std::list < std::pair < double, AcclContentName> > &list);

protected:
  virtual dataNameType_t createHeirarchicalStatName(const dataNameType_t &stat);
  virtual uint32_t createHeirarchicalStatId(const dataNameType_t &stat);
  std::vector < UtilityHandlerBase * >m_utilityEval;
  //std::vector < dataNameType_t, uint32_t > m_utilityBlockPriority;
  std::map < dataNameType_t, uint32_t > m_utilityBlockPriority;

  GlobalModuleLog *m_log;

  //This is module recording which content is cached (store)
  std::set < AcclContentName > m_PktNames;
  //This is module tracking which content is tracked in utilities
  std::set < AcclContentName > m_PktNameTracked;
  StorageClass_t m_storageMethod;

  NodeManager * m_myNodeRootModule;
  NodeStats *m_stats;
  ConfigWrapper *m_defaultConfig;
  //std::string m_selfName; //Node name or id
};

#endif
