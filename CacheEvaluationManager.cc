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
#include "CacheEvaluationManager.h"
#include "UtilityFunctionGenerator.h"


CacheEvaluationManager::CacheEvaluationManager()
  : m_storageLimit(0)
  , m_protectInsert(false)
  , m_contentType(IcnDefault)
  , m_attribName("LfuEval")
  , m_fieldTemp(true)
{
}

CacheEvaluationManager::CacheEvaluationManager(ConfigWrapper & config)
  : m_storageLimit(0)
  , m_protectInsert(false)
  , m_contentType(IcnDefault)
  , m_attribName("LfuEval")
  , m_fieldTemp(true)
{
  Config(config);
}

CacheEvaluationManager::~CacheEvaluationManager() {


}

bool
CacheEvaluationManager::AddConfig(UtilityHandlerBase * uc, uint16_t position) {
  return false;
}

bool
CacheEvaluationManager::OnInit(UtilityExternalModule * module) {
  ModuleManager::OnInit(module);
  //get cache store association
  m_myNodeName = (module->GetDevice()->GetSelfNode())->Name();
  //should make this an easy subroutine
  m_statsMiss = createHeirarchicalStatName("TotalCacheMiss");
  m_statsMissNotFound = createHeirarchicalStatName("TotalCacheMissNotFound");
  m_statsHit = createHeirarchicalStatName("TotalCacheHit");
  m_statsHitExpired = createHeirarchicalStatName("TotalCacheHitExpired");
  m_stats->SetStats(m_statsMiss, (uint64_t) 0);
  m_stats->SetStats(m_statsMissNotFound, (uint64_t) 0);
  m_stats->SetStats(m_statsHit, (uint64_t) 0);
  m_stats->SetStats(m_statsHitExpired, (uint64_t) 0);
  return true;
}

void
CacheEvaluationManager::Config(ConfigWrapper & config) {
  ModuleManager::Config(config);
  if (!m_useAlias) {
    m_name = IdName();
  }
  m_protectInsert = config.GetAttributeBool("protNewPkt", m_protectInsert);
  m_fieldTemp = config.GetAttributeBool("fieldUseTemp", m_fieldTemp);
  std::string ctype;
  ctype = config.GetAttribute("ContentTypes", m_contentTypeNames[m_contentType]);
  if (ctype != m_contentTypeNames[m_contentType]) {
    if (ctype == "Icn")
      m_contentType = IcnDefault;
    else if (ctype == "File")
      m_contentType = IcnFile;
    else
      m_contentType = IcnDefault;
  }
  m_storageLimit = config.GetAttribute("cacheTrackingSize", m_storageLimit);
  m_attribName=config.GetAttribute("setAttribName",m_attribName);
}


void
CacheEvaluationManager::OnPktIngress(PktType & pkt) {
  switch (pkt.GetPacketPurpose()) {
  case PktType::DATAPKT:
  case PktType::INTERESTPKT:
    OnDataInterestPktIngress(pkt);
    break;
  case PktType::CONTROLPKT:
    OnControlPktIngress(pkt);
    break;
  case PktType::DEBUGPKT:
    OnDebugPktIngress(pkt);
    break;
  default:
    return;
    break;
  }
}

void
CacheEvaluationManager::OnDebugPktIngress(PktType & debug) {
}

void
CacheEvaluationManager::OnControlPktIngress(PktType & control) {
}

void
CacheEvaluationManager::OnDataInterestPktIngress(PktType & interest) {
  switch (m_contentType) {
  case IcnDefault:
    IcnDefaultAction(interest);
    break;
  case IcnFile:
    IcnFileAction(interest);
    break;
  default:
    assert("invalide packet handling format");
  }
}

void
CacheEvaluationManager::IcnFileAction(PktType & interest) {
	uint64_t tmp=0;
  bool header = interest.GetUnsignedNamedAttribute("Header", tmp, m_fieldTemp);
  AcclContentName name = interest.GetAcclName();
  //new data, may need to purge
  //ModuleManager::OnPktIngress(interest); //let utilities judge it
  if (header)
	  IcnDefaultAction(interest);

}

void
CacheEvaluationManager::IcnDefaultAction(PktType & interest) {
  ModuleManager::OnPktIngress(interest); //let utilities judge it


  //new data, may need to purge
  if (interest.GetPacketPurpose() & PktType::DATAPKT) {
    PurgeICNContent(interest);
  }            //interest matching
  else if (interest.GetPacketPurpose() & PktType::INTERESTPKT) {
    CacheIcnHit(interest);
  }
}

void CacheEvaluationManager::PurgeICNContent(PktType &pkt)
{
  //dont purge if already in cache
  if ( m_PktNames.find(pkt.GetAcclName()) != m_PktNames.end()) { return; }

  if(!m_protectInsert) {
    m_PktNames.insert(pkt.GetAcclName());
   }

  //FIXME TODO remove m_protectInsert, we have it as a utility.
  if ((m_PktNames.size()+(int) m_protectInsert)  > m_storageLimit) {
  //if ((m_PktNames.size())  > m_storageLimit) {
    std::list < std::pair < double, AcclContentName> > PktList, obsoletePktList;
    //Compute();
    CacheDataHandler(pkt, PktList);
    //Delete extra up to lower watermark
    if (m_PktNames.size() > m_storageLimit) {
      auto itb = PktList.begin();
      auto ite = itb;
      advance(ite, m_PktNames.size()-m_storageLimit);
      std::list < std::pair < double, AcclContentName> > PktListWm (itb, ite); //itb+m_PktNames.size()-m_lowerCacheWaterMark);
      LocalStoreDelete(PktListWm);
    }

  } 
  if (m_protectInsert) {
    m_PktNames.insert(pkt.GetAcclName());
  }

}

//Get all ranks, then purge FIXME TODO.
//insert new content, evaluate it, and purge low/least value content
void CacheEvaluationManager::CacheDataHandler(PktType &interest, std::list< std::pair<double, AcclContentName> > &PktList) {


  if (!m_protectInsert) {
    m_PktNames.insert(interest.GetAcclName());
  }

  Compute();
  GetLowestNPackets (m_PktNames.size(), PktList);  //Need to test this first

  //protected insertion
  if (m_protectInsert) {
    m_PktNames.insert(interest.GetAcclName());
  }
}

void CacheEvaluationManager::CacheIcnHit(PktType & interest) {
  PktType fakePkt;

  //only do this for interest or data packets
  bool found = (m_PktNames.find(interest.GetAcclName()) != m_PktNames.end());

  if (found) { //give interest packet LOW value
    //give low priority, modify packet
    //std::cout << "\nInterest Update:" << interest.GetAcclName ();
    Compute(interest.GetAcclName());
    double value = Value(interest.GetAcclName());

      LOG("Cache Eval Hit %s %f\n", interest.GetName().GetFullName().c_str(), value);
      interest.SetNamedAttribute(m_attribName, value, m_fieldTemp);
      //m_statsHitExpired++;
      if (m_stats)
        m_stats->IncStats(m_statsHitExpired);

  } else { //give interest packet HIGH value
    LOG("Cache Eval Miss %s 0\n", interest.GetName().GetFullName().c_str());
    interest.SetNamedAttribute(m_attribName, 0.0, m_fieldTemp);
    if (m_stats) {
      m_stats->IncStats(m_statsMiss);
      	  //stats by origin
          std::string pathMissName = createHeirarchicalStatName(interest.GetName().GetPath()+"/ExtendedMiss");
	  auto it=m_namesPerOrigin.find(pathMissName);
	  if (it == m_namesPerOrigin.end()) {
            m_stats->SetStats(pathMissName,(uint64_t) 0);
		  m_namesPerOrigin.insert(pathMissName);
	  } 
	  m_stats->IncStats(pathMissName);

    }
  }
}

//Check if cache hit (store1 for ICN packets or FILE HDR packets)
//TODO FIXME should be an asyncronous operation
void CacheEvaluationManager::CacheHdrHit(PktType & interest) {
  PktType fakePkt;

  //only do this for interest or data packets
  bool found = (m_PktNames.find(interest.GetAcclName()) != m_PktNames.end());

  if (found) { //give interest packet LOW value
    //give low priority, modify packet
    //std::cout << "\nInterest Update:" << interest.GetAcclName ();
    Compute(interest.GetAcclName());
    double value = Value(interest.GetAcclName());

    interest.SetNamedAttribute(m_attribName, value, m_fieldTemp);
//non multiple packets fail with a value of 0, for some reason JLM FIXME TODO
std::cout << interest.GetAcclName() << " has value of " << value << "\n";

      LOG("Hdr Extended Cache Eval Hit %s %f\n", interest.GetName().GetFullName().c_str(), value);

      //Get DATA from cached packet
      //FIXME TODO jlm
      //This should be replaced by white/black/red chained lists
      PktType newPacket;

      //m_statsHit++;
      if (m_stats) {
        m_stats->IncStats(m_statsHit);
		  //stats by origin
          std::string pathHitName = createHeirarchicalStatName(interest.GetName().GetPath()+"/ExtendedHits");
	  auto it=m_namesPerOrigin.find(pathHitName);
	  if (it == m_namesPerOrigin.end()) {
            m_stats->SetStats(pathHitName,(uint64_t) 0);
		  m_namesPerOrigin.insert(pathHitName);
	  } 
	  m_stats->IncStats(pathHitName);

      }
  } else { //give interest packet HIGH value
    LOG("Hdr Extended Cache Eval Miss %s 0\n", interest.GetName().GetFullName().c_str());
    interest.SetNamedAttribute(m_attribName, 0.0, m_fieldTemp);
    if (m_stats) {
      m_stats->IncStats(m_statsMiss);
    	  //stats by origin
          std::string pathMissName = createHeirarchicalStatName(interest.GetName().GetPath()+"/ExtendedMiss");
	  auto it=m_namesPerOrigin.find(pathMissName);
	  if (it == m_namesPerOrigin.end()) {
            m_stats->SetStats(pathMissName,(uint64_t) 0);
		  m_namesPerOrigin.insert(pathMissName);
	  } 
	  m_stats->IncStats(pathMissName);
      }
    }
}

void
CacheEvaluationManager::OnPktEgress(PktType & data, const PktTxStatus & status) {

  if (PktType::DATAPKT != data.GetPacketPurpose()) {
    return;
  }
  ModuleManager::OnPktEgress(data, status); //let utilities judge it
}

  void CacheEvaluationManager::DumpStore (std::ostream &os) {
     for(auto it =m_PktNames.begin(); it != m_PktNames.end(); it++) {
         double a = Value(*it);
	 os << "[" << *it << "," << a << "]\n"; 
     }
  }
  void CacheEvaluationManager::PrintStore () { DumpStore(std::cout); }

//StoreActionsDone JLM FIXME

void
CacheEvaluationManager::LocalStoreDelete(const std::list < std::pair < double, AcclContentName> > &list) {
  ModuleManager::LocalStoreDelete(list);

  for (auto itl = list.begin(); itl != list.end(); itl++) {
    {
      AcclContentName name = itl->second;
      //erase store value

      std::cout << "Erased " << name << " with a value of " << itl->first << "\n";
      m_PktNames.erase (name);
    }
  }
}


