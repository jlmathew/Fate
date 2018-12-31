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
#include "ModuleManager.h"
#include "UtilityFunctionGenerator.h"
#include <algorithm>
#include "NodeManager.h"

ModuleManager::ModuleManager ():
  m_log(nullptr),
  m_myNodeRootModule(nullptr),
  m_stats(nullptr),
  m_defaultConfig(nullptr)
{
}

ModuleManager::ModuleManager (ConfigWrapper & config) :
  m_log(nullptr),
  m_myNodeRootModule(nullptr),
  m_stats(nullptr),
  m_defaultConfig(nullptr)
{

  Config (config);
}

ModuleManager::~ModuleManager ()
{
  m_PktNames.clear ();
  if (!m_utilityEval.empty()) {
     for (unsigned int i=0; i< m_utilityEval.size(); i++)
     {
        delete m_utilityEval[i];
     }
  }
}

void
ModuleManager::DoDelete (const AcclContentName & name)
{
  LocalStoreDelete (name);
}

bool
ModuleManager::AddConfig (UtilityHandlerBase * uc, uint16_t position)
{
  return false;
}

bool
ModuleManager::OnInit (UtilityExternalModule * data)
{
  std::vector < UtilityHandlerBase * >::const_iterator it = m_utilityEval.begin ();

  bool success = true;

  while (it != m_utilityEval.end ())
    {
      success &= (*it)->OnInit (data);
      it++;
    }
  //Should be read from config file, with a name of 'default'
  //FIXME TODO JLM or a list of logs for each.
  m_log = data->GetGlobalModule ()->GetGlobalLog ("default");
  data->GetDevice()->SetSelfModule(this);
  m_myNodeRootModule = data->GetDevice()->GetSelfNode();
  m_stats = data->GetDevice()->GetNodeStats();
 // m_moduleName = m_myNodeRootModule->GetNodeName();
  //m_moduleName.append("/");
  //m_moduleName.append(m_name);
  
  return success;

}
dataNameType_t
ModuleManager::createHeirarchicalStatName(const dataNameType_t &stat)
{
      std::string name;
   if (m_myNodeRootModule)
      name = m_myNodeRootModule->Name();
   name.append("/");
   name.append(Name());
   name.append("/");
   name.append(stat);

   return name;
}

uint32_t
ModuleManager::createHeirarchicalStatId(const dataNameType_t &stat)
{
   dataNameType_t fullName = createHeirarchicalStatName(stat);
   //unique register Name
   uint32_t Id=0;
   return Id;
}
  
void
ModuleManager::StoreActionsDone (const std::list < StoreEvents > &list)
{
  assert(0);
}

void
ModuleManager::DoStoreActions (const std::list < StoreEvents > &list)
{
  assert(0);
}

  //default is no action  //JLM
void
ModuleManager::OnAsyncEvent (AsyncEvent & event)
{
  std::vector < UtilityHandlerBase * >::const_iterator it = m_utilityEval.begin ();
  while (it != m_utilityEval.end ())
    {
      assert (0);
      //(*it)->OnAsyncHWEvent(eventType, eventInfo);
      it++;
    }

}

void
ModuleManager::ComputeAndValue (std::list < PktType > &pktList)
{
  std::vector < UtilityHandlerBase * >::const_iterator it = m_utilityEval.begin ();
  while (it != m_utilityEval.end ())
    {
      (*it)->ComputeAndValue (pktList);
      it++;
    }
  assert(0);
}

bool
ModuleManager::SelfTest()
{
  std::vector < UtilityHandlerBase * >::const_iterator it = m_utilityEval.begin ();
  bool results = true;
  while (it != m_utilityEval.end ())
    {
      results &= (*it)->SelfTest();
      it++;
    }

  return results;
}

//single config do
//multiple match black-white list
//exception for multiple, find default
void
ModuleManager::Config (ConfigWrapper & config,  std::set<std::string> *names, bool whitelist)
{

  ConfigWrapper *childConfig = config.GetFirstChildUtility ("Utility");
  if (!childConfig)
	  return;

  uint32_t counter=0;

  //first entry is default, unless, explicityly labelled 'default' later
  if ((0 == counter) && (childConfig->valid())) {
     if (m_defaultConfig)
         delete m_defaultConfig;

     m_defaultConfig = childConfig->Clone();
  }

  while (childConfig->valid ())
    {
      auto block = (UtilityGenerator::CreateNewUtility (*childConfig));
      assert(block);
      if (nullptr != names) {
       if (names->end() == names->find(block->Name())) {
           if (whitelist) {  //blacklist, we wont implement those
             m_utilityEval.push_back (block);
             m_utilityBlockPriority[block->Name()]=counter++;
           }
        } else if ("default" == block->Name()) { //FIXME, name as 'default'?
           m_utilityEval.push_back (block);
           m_utilityBlockPriority[block->Name()]=counter++;
           if (m_defaultConfig)
                delete m_defaultConfig;
           m_defaultConfig=childConfig->Clone();
        }
      } else { //no black-white list, implement all
           m_utilityEval.push_back (block);
           m_utilityBlockPriority[block->Name()]=counter++;
      }
      childConfig->GetNextChildUtility ("Utility");
    }

  delete childConfig;
}


const std::string &
ModuleManager::Name () const
{
   return m_name;
}


void
ModuleManager::OnPktIngress (PktType & interest)
{
  std::vector < UtilityHandlerBase * >::const_iterator it = m_utilityEval.begin ();
  while (it != m_utilityEval.end ())
    {
      (*it)->OnPktIngress (interest);
      it++;
    }

}                               //Rx

void
ModuleManager::OnPktEgress (PktType & data, const PktTxStatus & status)
{
  std::vector < UtilityHandlerBase * >::const_iterator it = m_utilityEval.begin ();
  while (it != m_utilityEval.end ())
    {
      (*it)->OnPktEgress (data, status);
      it++;
    }
}                               //Rx

void
ModuleManager::Print (std::ostream & os) const
{
  os << "Module:" << m_name << "\n";
  std::vector < UtilityHandlerBase * >::const_iterator it = m_utilityEval.begin ();
  while (it != m_utilityEval.end ())
    {
      os << "Utility:";
      (*it)->Print (os);
      it++;
      os << "\n";

    } 
  os << "\n";

}

void
ModuleManager::Print (std::ostream & os, const AcclContentName & tlvName, double &value) const
{
  std::vector < UtilityHandlerBase * >::const_iterator it = m_utilityEval.end ();
  --it;
  if (!m_utilityEval.empty ())
    {
      (*it)->Print (os, tlvName, value);
    }

}

void
ModuleManager::Compute ()
{
  std::vector < UtilityHandlerBase * >::const_iterator it = m_utilityEval.begin ();
  while (it != m_utilityEval.end ())
    {
      (*it)->Compute ();
      it++;
    }
}

void
ModuleManager::Compute (const AcclContentName & name)
{
  std::vector < UtilityHandlerBase * >::const_iterator it = m_utilityEval.begin ();
  while (it != m_utilityEval.end ())
    {
      (*it)->Compute (name);
      it++;
    }
}

double
ModuleManager::Value (const AcclContentName & name) const
{
  std::vector < UtilityHandlerBase * >::const_iterator it = m_utilityEval.begin ();
  double returnVal = -1.0;

  while (it != m_utilityEval.end ())
    {
      returnVal = (*it)->Value (name);
      it++;
    }
  return returnVal;        //We only return the LAST evaluation

}

UtilityHandlerBase *
ModuleManager::GetUtilityBlock (uint32_t index)
{
  return m_utilityEval[index];
}

UtilityHandlerBase *
ModuleManager::GetUtilityBlock (const dataNameType_t & name)
{
  uint32_t index = m_utilityBlockPriority[name];

  return m_utilityEval[index];
}

UtilityHandlerBase *
ModuleManager::RemoveUtilityBlock (uint32_t index)
{
  return m_utilityEval[index];
}

UtilityHandlerBase *
ModuleManager::RemoveUtilityBlock (const dataNameType_t & name)
{
  uint32_t index = m_utilityBlockPriority[name];

  return m_utilityEval[index];
}

void
ModuleManager::SetUtilityBlock (UtilityHandlerBase * ucBlock, uint32_t order)
{
  m_utilityEval.push_back (ucBlock);    //we need to order it
}

uint32_t
ModuleManager::retNumUtilityBlocks ()
{
  return m_utilityEval.size ();
}


void
ModuleManager::GetPacketsByValue (const RangeData < double >&rng, std::list < AcclContentName > &list)
{
  UtilityHandlerBase *lastBlock = m_utilityEval.back ();

  std::set < AcclContentName >::const_iterator cit = m_PktNames.begin ();
  for (; cit != m_PktNames.end (); cit++)
    {
      double rngVal = lastBlock->Value (*cit);

      if (rng.IsInRange (rngVal))
        {
          list.push_back (*cit);
        }
    }

}

void
ModuleManager::GetDebugInfo(PktType &packet) const {
    //NodeModule checks if debugpkt and if correct node name
    //if (packet.GetPacketType() != PktType::DEBUGPKT)
    //  return;
    //identify node (with respect to tracert or ping) (only for Node)

    //options
    //identify modules.  If no module identified, all modules
    

    //get packet valuation

    //get setup (config) information

    //get self-address information (e.g. ip/mac)

    //get tracert or ping information (name-timestamp)

    //get sensor info (location, battery level, etc)

    //Store info?

    //Output information (packet or log)
    //default is packet, if log not specified.
   
}
bool
ModuleManager::LocalStoreDelete (const AcclContentName & name)
{
  return false;
}

void
ModuleManager::GetLowestNPackets (uint64_t lowestN, std::list < std::pair< double, AcclContentName> > &mylist)
{
  UtilityHandlerBase *lastBlock = m_utilityEval.back ();

  std::set < AcclContentName >::const_iterator cit = m_PktNames.begin ();
  typedef std::pair<double, AcclContentName > P;
  //std::priority_queue< P, std::vector<P>, std::greater_equal<P> > orderQueue;
  std::priority_queue< P, std::vector<P>, std::greater<P> > orderQueue;

  for (; cit != m_PktNames.end (); cit++)
    {
      double rngVal = lastBlock->Value (*cit);
      orderQueue.push (make_pair (rngVal, *cit));
    //std::cout << "\n(" << *cit << ")";
    //Print(std::cout, *cit, rngVal); 

    }


  uint64_t lowerBound = lowestN;
  if (lowestN > orderQueue.size ())
    {
      lowerBound = orderQueue.size ();
    }
  //pop the LOWEST values first!
  for (uint64_t i = 0; i < lowerBound; i++)
    {
      std::pair < double, AcclContentName > val = orderQueue.top ();

      orderQueue.pop ();
      //mylist.push_back (val.second);
      mylist.push_back (val);
    }

}

void
ModuleManager::LogStats ()
{
   //send to correct log an action (time, event[insert,update,delete,compute], name, value)
}                               //do nothing, default

void
ModuleManager::LocalStoreDelete(const std::list < std::pair < double, AcclContentName> > &list)
{

  std::list < std::pair< double, AcclContentName> >::const_iterator itl = list.begin ();
  for (; itl != list.end (); itl++)
    {
      AcclContentName name = itl->second;
 
      std::vector < UtilityHandlerBase * >::iterator it = m_utilityEval.begin ();
      for (; it != m_utilityEval.end (); it++)
        {
          (*it)->DoDelete (name);
        }
      m_PktNames.erase (name);

    }

}


