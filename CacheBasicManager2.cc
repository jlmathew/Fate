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
#include "CacheBasicManager2.h"
#include "UtilityFunctionGenerator.h"


CacheBasicManager2::CacheBasicManager2 ()
  :m_cacheStore (NULL)
   , m_storageLimit(0)
   , m_cacheStoreName("None")
   , m_dropValue(0.0)
   , m_protectInsert(true)
{
}

CacheBasicManager2::CacheBasicManager2 (ConfigWrapper & config)
  : //ModuleManager::ModuleManager(config)
    m_cacheStore (NULL)
    ,m_storageLimit (0)
    ,m_cacheStoreName ("None")
   , m_dropValue(0.0)
   , m_protectInsert(true)
{
  Config (config);
}

CacheBasicManager2::~CacheBasicManager2 ()
{


}

bool
CacheBasicManager2::AddConfig (UtilityHandlerBase * uc, uint16_t position)
{
  return false;
}

bool
CacheBasicManager2::OnInit (UtilityExternalModule * module)
{
  ModuleManager::OnInit (module);
  //get cache store association
  if ((m_useStore) && (NULL == m_cacheStore))
    {
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
CacheBasicManager2::Config (ConfigWrapper & config)
{
  ModuleManager::Config(config);
  if (!m_useAlias)
    {
      m_name = IdName ();
    }
  m_cacheStoreName = config.GetAttribute ("associatedStore", m_cacheStoreName.c_str ());
  m_storageLimit = config.GetAttribute ("cacheSize", m_storageLimit);
  m_dropValue = config.GetAttribute ("minDropValue", m_dropValue);
  if (m_cacheStoreName.compare("None")) {
    m_useStore=true;
  } else {
    m_useStore=false;
  }
  m_protectInsert=config.GetAttributeBool("protNewPkt", m_protectInsert);

}


void
CacheBasicManager2::SetStore (StoreManager * store)
{
  if (m_cacheStore)
    delete m_cacheStore;

  m_useStore = true;
  m_cacheStore = dynamic_cast < TypicalCacheStore * >(store);
}

void
CacheBasicManager2::OnPktIngress (PktType & pkt)
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
CacheBasicManager2::OnDebugPktIngress (PktType & debug)
{
  /*switch (cmd) {
     case hitRatio:
     break;
     case namevalue:
     break;
     case allnames:
     break;
     case allnamevalues:
     break;
     case purgednamevalues:
     break;
     case allnameformula;
     break;
     case purgednameformula;
     break;

  }*/
}

void
CacheBasicManager2::OnControlPktIngress (PktType & control)
{
}



void
CacheBasicManager2::OnDataInterestPktIngress (PktType & interest)
{
  ModuleManager::OnPktIngress (interest);       //let utilities judge it

//std::cout << "cb2 pkt is " << interest;

  if (interest.GetPacketPurpose () & PktType::DATAPKT)
    {
      if(m_useStore)
         m_cacheStore->SetData (interest.GetAcclName (), interest);

      m_PktNames.insert (interest.GetAcclName ());
//std::cout << "\nDATA PKT INSERT:" << interest.GetAcclName();
      if (m_PktNames.size() > m_storageLimit)
        {
          //delete all expired values OR lowest value
          Compute ();
          RangeData < double >valueRange (0.0, m_dropValue);

          std::list < std::pair < double, AcclContentName> > PktList;
          //GetPacketsByValue (valueRange, PktList);  //Need to test this first
          //or
          
          if (PktList.empty ())
            {
  
              uint64_t n;
              if (m_protectInsert)
              {
                   n = 2;
              }
              else 
              {
                   n = 1;
              }
              GetLowestNPackets (n, PktList);   //or lowest Packet
            }
          //We realy want to asyn queue up a delete store request, for now, do this
          //FIXME TODO, use DoStoreActions()
          //Also, we do NOT delete the current packet, UNLESS its value is below threshold.
  std::pair < double, AcclContentName > val = PktList.front();

  //drop lowest packet, unless its our recent packet AND its value is above dropValue 
  //drop same pkt if value is <= m_dropValue(typically 0)
  if (m_protectInsert && (val.second == interest.GetAcclName()) && (val.first > m_dropValue)) 
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
      bool found  = (m_PktNames.find(interest.GetAcclName()) != m_PktNames.end());

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
CacheBasicManager2::OnPktEgress (PktType & data, const PktTxStatus & status)
{

  if (PktType::DATAPKT != data.GetPacketPurpose ())
    {
      return;
    }
  ModuleManager::OnPktEgress (data, status);    //let utilities judge it
}



void
CacheBasicManager2::StoreActionsDone (const std::list < StoreEvents > &list)
{
}

//StoreActionsDone JLM FIXME
void
CacheBasicManager2::LocalStoreDelete (const std::list < std::pair < double, AcclContentName> > &list)
{
  ModuleManager::LocalStoreDelete(list);

  for (auto itl=list.begin(); itl != list.end (); itl++)
    {
      AcclContentName name = itl->second;
      if (m_useStore)
         m_cacheStore->EraseData (name);

    }
  //JLM FIX ME need to send out store delete event, or store insert event, as appropriate
  //we can also log deletions with valuations

}
