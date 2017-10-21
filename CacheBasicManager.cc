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
#include "CacheBasicManager.h"
#include "UtilityFunctionGenerator.h"


CacheBasicManager::CacheBasicManager ()
  :m_cacheStore (NULL)
   , m_storageLimit(0)
   , m_cacheStoreName("CacheStore")
   , m_dropValue(0.0)
{
}

CacheBasicManager::CacheBasicManager (ConfigWrapper & config)
  : //ModuleManager::ModuleManager(config)
    m_cacheStore (NULL)
    ,m_storageLimit (0)
    ,m_cacheStoreName ("CacheStore")
   , m_dropValue(0.0)
{
  Config (config);
}

CacheBasicManager::~CacheBasicManager ()
{


}

bool
CacheBasicManager::AddConfig (UtilityHandlerBase * uc, uint16_t position)
{
  return false;
}

bool
CacheBasicManager::OnInit (UtilityExternalModule * module)
{
  ModuleManager::OnInit (module);
  //get cache store association
  if (NULL == m_cacheStore)
    {
      //FIXME JLM TODO need a 'self node' option for testing individual modules
      m_cacheStore = dynamic_cast < TypicalCacheStore * >((module->GetDevice ()->GetSelfNode ())->GetStore (m_cacheStoreName));

    }
  
  //should make this an easy subroutine
  m_statsMiss = createHeirarchicalStatName("CacheMiss");
  m_statsHit = createHeirarchicalStatName("CacheHit") ;
  m_statsHitExpired = createHeirarchicalStatName("CacheHitExpired") ;
  m_stats->SetStats(m_statsMiss,(uint64_t) 0);
  m_stats->SetStats(m_statsHit,(uint64_t) 0);
  m_stats->SetStats(m_statsHitExpired,(uint64_t) 0);
  return true;
}

void
CacheBasicManager::Config (ConfigWrapper & config)
{
  ModuleManager::Config(config);
  if (!m_useAlias)
    {
      m_name = IdName ();
    }
  m_cacheStoreName = config.GetAttribute ("associatedStore", m_cacheStoreName.c_str ());
  m_storageLimit = config.GetAttribute ("cacheSize", m_storageLimit);
  m_dropValue = config.GetAttribute ("minDropValue", m_dropValue);
}


void
CacheBasicManager::SetStore (StoreManager * store)
{
  m_cacheStore = dynamic_cast < TypicalCacheStore * >(store);
}

void
CacheBasicManager::OnPktIngress (PktType & pkt)
{
  switch (pkt.GetPacketPurpose ())
    {
      case PktType::DATAPKT:
      case PktType::INTERESTPKT:
        OnDataInterestPktIngress (pkt);
        break;
      case PktType::CONTROLPKT:
        OnControlPktIngress (pkt);
        break;
      case PktType::DEBUGPKT:
        OnDebugPktIngress (pkt);
        break;
      default:
        return;
        break;
    }
}

void
CacheBasicManager::OnDebugPktIngress (PktType & debug)
{
}

void
CacheBasicManager::OnControlPktIngress (PktType & control)
{
}



void
CacheBasicManager::OnDataInterestPktIngress (PktType & interest)
{
  ModuleManager::OnPktIngress (interest);       //let utilities judge it

  assert(m_cacheStore->size () <= m_storageLimit);

  if (interest.GetPacketPurpose () & PktType::DATAPKT)
    {
      m_cacheStore->SetData (interest.GetAcclName (), interest);
      m_PktNames.insert (interest.GetAcclName ());
//std::cout << "\nDATA PKT INSERT:" << interest.GetAcclName();
      if (m_cacheStore->size () > m_storageLimit)
        {
          //delete all expired values OR lowest value
          Compute ();
          RangeData < double >valueRange (0.0, m_dropValue);

          std::list < std::pair < double, AcclContentName> > PktList;
          //GetPacketsByValue (valueRange, PktList);  //Need to test this first
          //or
          if (PktList.empty ())
            {
              uint64_t n = 2;

              GetLowestNPackets (n, PktList);   //or lowest Packet
            }
assert(PktList.size() == 2);
          //We realy want to asyn queue up a delete store request, for now, do this
          //FIXME TODO, use DoStoreActions()
          //Also, we do NOT delete the current packet, UNLESS its value is below threshold.
  std::pair < double, AcclContentName > val = PktList.front();

  //drop lowest packet, unless its our recent packet AND its value is above dropValue 
  //drop same pkt if value is <= m_dropValue(typically 0)
  if ((val.second == interest.GetAcclName()) && (val.first > m_dropValue)) 
  {
          PktList.pop_front();     
  } else { 
//std::pair < double, AcclContentName > val = PktList.back();  //for debugging
PktList.pop_back();

 }

          LocalStoreDelete (PktList);
          while (!PktList.empty()) {
             
             std::pair < double, AcclContentName > val = PktList.front();
             m_PktNames.erase(val.second);
             //LOG("erase %d", PktList.front());
             PktList.pop_front();
          }
          //m_PktNames.erase (interest.GetAcclName ());
          //debug
        }
    }
  else if (interest.GetPacketPurpose () & PktType::INTERESTPKT)
    {

      PktType fakePkt;

      //only do this for interest or data packets
      bool found = m_cacheStore->ExistData (interest.GetAcclName (), fakePkt);

      if (found)
        {                       //give interest packet LOW value
          //give low priority, modify packet
//std::cout << "\nInterest Update:" << interest.GetAcclName ();
          Compute (interest.GetAcclName ());
          double value = Value (interest.GetAcclName ());

          if (value <= m_dropValue)
            {
              LOG ("Cache Expired Hit %s\n", interest.GetName ().GetFullName ().c_str ());
              interest.SetNamedAttribute ("CacheHit", 0.0, true);
              //m_statsHitExpired++;
              if (m_stats)
              m_stats->IncStats(m_statsHitExpired);
            }
          else
            {                   //drop, so its a miss
              LOG ("Cache Hit %s\n", interest.GetName ().GetFullName ().c_str ());
              interest.SetNamedAttribute ("CacheHit", 1.0, true);
              //m_statsHit++;
              if (m_stats)
              m_stats->IncStats(m_statsHit);
            }
        }
      else
        {                       //give interest packet HIGH value
          LOG ("Cache Miss %s\n", interest.GetName ().GetFullName ().c_str ());
          interest.SetNamedAttribute ("CacheHit", 0.0, true);
              if (m_stats)
          m_stats->IncStats(m_statsMiss);
          //m_statsMiss++;
        }
    }
}


void
CacheBasicManager::OnPktEgress (PktType & data, const PktTxStatus & status)
{

  if (PktType::DATAPKT != data.GetPacketPurpose ())
    {
      return;
    }
  ModuleManager::OnPktEgress (data, status);    //let utilities judge it
}



void
CacheBasicManager::StoreActionsDone (const std::list < StoreEvents > &list)
{
}

//StoreActionsDone JLM FIXME
void
CacheBasicManager::LocalStoreDelete (const std::list < std::pair < double, AcclContentName> > &list)
{

  std::list < std::pair< double, AcclContentName> >::const_iterator itl = list.begin ();
  for (; itl != list.end (); itl++)
    {
      AcclContentName name = itl->second;
      //std::cout << "\nCache Purge(" << name << "):";
      //double value=0.0;
      //Print(std::cout, name, value); 
      //std::cout << "or " << itl->first << "\n";
 
      std::vector < UtilityHandlerBase * >::iterator it = m_utilityEval.begin ();
      for (; it != m_utilityEval.end (); it++)
        {
          //(*it)->LocalStoreDelete(name);
          (*it)->DoDelete (name);
        }
      m_PktNames.erase (name);
      m_cacheStore->EraseData (name);

    }
  //JLM FIX ME need to send out store delete event, or store insert event, as appropriate
  //we can also log deletions with valuations

}
