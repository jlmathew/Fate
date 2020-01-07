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
#ifndef NODEMANAGERBASEPKTQUEUE_H_
#define NODEMANAGERBASEPKTQUEUE_H_

#include <vector>
#include "ContentName.h"

#include "ModuleManager.h"
#include "StoreManager.h"
#include "UtilityFunctionGenerator.h"
#include "NodeManager.h"
#include "UtilityExternalModule.h"
//class ModuleManager;

class NodeManagerPktQueue:public NodeManager
//class NodeManagerPktQueue:public ModuleManager
{
public:
  NodeManagerPktQueue ();

  NodeManagerPktQueue (ConfigWrapper & config);
  virtual ~ NodeManagerPktQueue ();


  virtual bool OnInit (UtilityExternalModule *);

  virtual void Config (ConfigWrapper & config);

  virtual const std::string & Name () const;

  virtual bool SelfTest(void);
  //virtual bool
  //AddConfig(ModuleManager *uc, uint16_t position=0);

  //delete Configs by name or number

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("NodeManagerPktQueue");
      return idName;
  }

  //store connectivity

  virtual void DumpAll (std::ostream &os) const;
  virtual void OnPktIngress (PktType & interest);       //Rx
  virtual void OnPktEgress (PktType & data, const PktTxStatus & status);        //Rx
  virtual void Print (std::ostream & os) const;
  virtual void Print (std::ostream & os, const AcclContentName & Name, double &value) const;
  virtual void Compute ();
  virtual void Compute (const AcclContentName & name);
  virtual void OnAsyncEvent (AsyncEvent & eventType);

  virtual ModuleManager *GetModule (const dataNameType_t & name);
  virtual ModuleManager *GetModule (uint32_t index);
  virtual bool RemoveModule (const dataNameType_t & name);
  virtual bool RemoveModule (uint32_t index);
  virtual void SetModule (ModuleManager *, uint32_t order);

  virtual uint32_t retNumModules ();


  virtual StoreManager *GetStore (const dataNameType_t & name);
  virtual StoreManager *GetStore (uint32_t index);
  virtual bool RemoveStore (const dataNameType_t & name);
  virtual bool RemoveStore (uint32_t index);
  virtual void SetStore (StoreManager *, uint32_t order);
  virtual uint32_t retNumStores ();
protected:
  //prioritized list of modules
  std::vector < ModuleManager * >m_moduleEval;
  std::vector < StoreManager * >m_StoreArr;
  //name to module index association
  std::map < dataNameType_t, uint32_t > m_modulePriority;
  std::map < dataNameType_t, uint32_t > m_StoreNames;

  std::string m_name;
};

#endif
