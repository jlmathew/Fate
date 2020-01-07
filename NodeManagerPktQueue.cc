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
#include "NodeManagerPktQueue.h"
//#include "UtilityFunctionGenerator.h"


NodeManagerPktQueue::NodeManagerPktQueue ()
  : m_name("NoName")
{
}

NodeManagerPktQueue::NodeManagerPktQueue (ConfigWrapper & config)
  //:ModuleManagerPktQueue::ModuleManager(config)
   : m_name("NoName")
{
  Config (config);
}

NodeManagerPktQueue::~NodeManagerPktQueue ()
{
  if (!m_moduleEval.empty())
  {
    for (unsigned int i=0; i< m_moduleEval.size(); i++) 
    {
       delete m_moduleEval[i];
    }
  }
  m_moduleEval.clear();

  if (!m_StoreArr.empty())
  {
    for (unsigned int i=0; i< m_StoreArr.size(); i++) 
    {
       delete m_StoreArr[i];
    }
  }
  m_StoreArr.clear();

  m_modulePriority.clear();
  m_StoreNames.clear();
}

  //bool
  //NodeManagerPktQueue::AddConfig(ModuleManager *uc, uint16_t position) {}
bool
NodeManagerPktQueue::OnInit (UtilityExternalModule * data)
{
  ModuleManager::OnInit(data);
  std::vector < ModuleManager * >::const_iterator it = m_moduleEval.begin ();
  data->GetDevice()->SetSelfNode (this);  //give submodules access to stores, and other modules (if necessary) 
  std::string selfName = data->GetDevice()->GetNodeName();
  if (selfName.size() > 0) {
     m_name = selfName;
  }
  bool success = true;

  while (it != m_moduleEval.end ())
    {
      success &= (*it)->OnInit (data);
      it++;
    }
  return success;

}

  //default is no action
  //need to do it in order - fixme JLM
void
NodeManagerPktQueue::OnAsyncEvent (AsyncEvent & eventType)
{
  std::vector < ModuleManager * >::const_iterator it = m_moduleEval.begin ();
  while (it != m_moduleEval.end ())
    {
      (*it)->OnAsyncEvent (eventType);
      it++;
    }

}

void
NodeManagerPktQueue::Config (ConfigWrapper & config)
{
    ModuleManager::Config(config);
  if (!m_useAlias)
    {
      m_name = IdName ();
    }

  ConfigWrapper *storeConfig = config.GetFirstChildStore ("Store");

  while (storeConfig->valid ())
    {
      StoreManager *store = (StoreGenerator::CreateNewStore (*storeConfig));

      //m_StoreArr.push_back(store);
      //storeConfig->GetNextChildStore();
      //std::string name = store->Name();
      //m_StoreNames[name] = m_StoreArr.size()-1;
      SetStore (store, 0);
      storeConfig->GetNextChildStore ("Store");
      
    }
  delete storeConfig;

  ConfigWrapper *childConfig = config.GetFirstChildModule ("UtilityModule");

  while (childConfig->valid ())
    {
      ModuleManager *module = (ModuleGenerator::CreateNewModule (*childConfig));

      //m_moduleEval.push_back(module);
      //std::string name = module->Name();
      // m_modulePriority[name] = m_moduleEval.size()-1;
      SetModule (module, 0);
      childConfig->GetNextChildModule ("UtilityModule");

    }
  //need to sort, and associate name to index

  //free unused configuration memory
  delete childConfig;

}

const std::string &
NodeManagerPktQueue::Name () const
{
  return m_name;
}


void NodeManagerPktQueue::OnPktIngress (PktType & interest)
{
  std::vector < ModuleManager * >::const_iterator it = m_moduleEval.begin ();
  while (it != m_moduleEval.end ())
    {
      (*it)->OnPktIngress (interest);
      it++;
    }
    //loop for all stores
    std::vector < StoreManager *>::const_iterator store_it = m_StoreArr.begin();
    while (store_it != m_StoreArr.end())
    {
	(*store_it)->OnPktIngress(interest);	
	store_it++;
    }
}                               //Rx

void
NodeManagerPktQueue::OnPktEgress (PktType & data, const PktTxStatus & status)
{
  std::vector < ModuleManager * >::const_iterator it = m_moduleEval.begin ();
  while (it != m_moduleEval.end ())
    {
      (*it)->OnPktEgress (data, status);
      it++;
    }
}                               //Rx

void
     NodeManagerPktQueue::Print (std::ostream & os) const
     {
       os << "Node:" << m_name << "\n";
       std::vector < ModuleManager * >::const_iterator it = m_moduleEval.begin ();
       while (it != m_moduleEval.end ())
         {
           //os << "\tUtility" << (*it)->Name():";
           (*it)->Print (os);
           it++;
           os << "\n";

         }
       os << "\n";

     }

void
NodeManagerPktQueue::DumpAll(std::ostream &os) const
{
  
  std::vector < ModuleManager * >::const_iterator it = m_moduleEval.begin ();
  while (it != m_moduleEval.end ())
    {
      (*it)->DumpStore (os);
      it++;
    }
}
void
     NodeManagerPktQueue::Print (std::ostream & os, const AcclContentName & Name, double &value) const
     {
       std::vector < ModuleManager * >::const_iterator it = m_moduleEval.end ();
       --it;
       if (!m_moduleEval.empty ())
         {
           (*it)->Print (os, Name, value);
         }

     }

void
NodeManagerPktQueue::Compute ()
{
  std::vector < ModuleManager * >::const_iterator it = m_moduleEval.begin ();
  while (it != m_moduleEval.end ())
    {
      (*it)->Compute ();
      it++;
    }
}

void
NodeManagerPktQueue::Compute (const AcclContentName & name)
{
  std::vector < ModuleManager * >::const_iterator it = m_moduleEval.begin ();
  while (it != m_moduleEval.end ())
    {
      (*it)->Compute (name);
      it++;
    }
}

bool
NodeManagerPktQueue::SelfTest() 
{
  bool ret=true;
  std::vector < ModuleManager * >::const_iterator it = m_moduleEval.begin ();
  while (it != m_moduleEval.end ())
    {
      ret &= (*it)->SelfTest ();
      it++;
    }
  return ret;
}

ModuleManager *
NodeManagerPktQueue::GetModule (uint32_t index)
{
  return m_moduleEval[index];
}

ModuleManager *
NodeManagerPktQueue::GetModule (const dataNameType_t & name)
{
  uint32_t index = m_modulePriority[name];

  return m_moduleEval[index];
}

bool
NodeManagerPktQueue::RemoveModule (uint32_t index)
{
  return false;                 //m_moduleEval[index];
}

bool
NodeManagerPktQueue::RemoveModule (const dataNameType_t & name)
{
  //uint32_t index = m_modulePriority[name];

  return false;                 //m_moduleEval.remove[index];
}

//FIXME TODO need to associate name ...
void
NodeManagerPktQueue::SetModule (ModuleManager * module, uint32_t order)
{
  //m_moduleEval.push_back(module); //we need to order it
  m_moduleEval.push_back (module);
  std::string name = module->Name ();
  m_modulePriority[name] = m_moduleEval.size () - 1;


}

//FIXME TODO check if names exist, dont want to create false entries
uint32_t
NodeManagerPktQueue::retNumModules ()
{
  return m_moduleEval.size ();
}

StoreManager *
NodeManagerPktQueue::GetStore (const dataNameType_t & name)
{
  uint32_t index = m_StoreNames[name];

  return m_StoreArr[index];
}

StoreManager *
NodeManagerPktQueue::GetStore (uint32_t index)
{
  return m_StoreArr[index];
}

bool
NodeManagerPktQueue::RemoveStore (const dataNameType_t & name)
{
  assert (0);
  return false;
}

bool
NodeManagerPktQueue::RemoveStore (uint32_t index)
{
  assert (0);
  return false;
}

void
NodeManagerPktQueue::SetStore (StoreManager * store, uint32_t order)
{
  m_StoreArr.push_back (store);
  std::string name = store->Name ();
  m_StoreNames[name] = m_StoreArr.size () - 1;

}

uint32_t NodeManagerPktQueue::retNumStores ()
{
  return m_StoreArr.size ();
}
