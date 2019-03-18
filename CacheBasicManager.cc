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


CacheBasicManager::CacheBasicManager()
  : m_cacheStore(nullptr)
  , m_storageLimit(0)
  , m_cacheStoreName("None")
  , m_dropValue(0.0)
  , m_protectInsert(true)
  , m_contentType(IcnDefault)
  , m_deleteByValue(false)
{
}

CacheBasicManager::CacheBasicManager(ConfigWrapper & config)
  : //ModuleManager::ModuleManager(config)
    m_cacheStore(nullptr)
  , m_storageLimit(0)
  , m_cacheStoreName("None")
  , m_dropValue(0.0)
  , m_protectInsert(true)
  , m_contentType(IcnDefault)
  , m_deleteByValue(false)
{
  Config(config);
}

CacheBasicManager::~CacheBasicManager() {


}

bool
CacheBasicManager::AddConfig(UtilityHandlerBase * uc, uint16_t position) {
  return false;
}

bool
CacheBasicManager::OnInit(UtilityExternalModule * module) {
  ModuleManager::OnInit(module);
  //get cache store association
  if ((m_useStore) && (NULL == m_cacheStore)) {
    m_cacheStore = dynamic_cast<TypicalCacheStore *> ((module->GetDevice()->GetSelfNode())->GetStore(m_cacheStoreName));
  }
  m_myNodeName = (module->GetDevice()->GetSelfNode())->Name();
  //should make this an easy subroutine
  m_statsMiss = createHeirarchicalStatName("CacheMiss");
  m_statsMissNotFound = createHeirarchicalStatName("CacheMissNotFound");
  m_statsHit = createHeirarchicalStatName("CacheHit");
  m_statsHitExpired = createHeirarchicalStatName("CacheHitExpired");
  m_stats->SetStats(m_statsMiss, (uint64_t) 0);
  m_stats->SetStats(m_statsMissNotFound, (uint64_t) 0);
  m_stats->SetStats(m_statsHit, (uint64_t) 0);
  m_stats->SetStats(m_statsHitExpired, (uint64_t) 0);
  return true;
}

void
CacheBasicManager::Config(ConfigWrapper & config) {
  ModuleManager::Config(config);
  if (!m_useAlias) {
    m_name = IdName();
  }
  m_cacheStoreName = config.GetAttribute("associatedStore", m_cacheStoreName.c_str());
  m_storageLimit = config.GetAttribute("cacheSize", m_storageLimit);
  m_dropValue = config.GetAttribute("minDropValue", m_dropValue);
  if (m_cacheStoreName.compare("None")) {
    m_useStore = true;
  } else {
    m_useStore = false;
  }
  m_protectInsert = config.GetAttributeBool("protNewPkt", m_protectInsert);
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
  m_lowerCacheWatermark = config.GetAttribute("cacheLowerWaterMark", m_storageLimit);
}

void
CacheBasicManager::SetStore(StoreManager * store) {
  if (m_cacheStore)
    delete m_cacheStore;

  m_useStore = true;
  m_cacheStore = dynamic_cast<TypicalCacheStore *> (store);
}

void
CacheBasicManager::OnPktIngress(PktType & pkt) {
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
CacheBasicManager::OnDebugPktIngress(PktType & debug) {
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
CacheBasicManager::OnControlPktIngress(PktType & control) {
}

void
CacheBasicManager::OnDataInterestPktIngress(PktType & interest) {
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
CacheBasicManager::IcnFileAction(PktType & interest) {
  //if header exists, file might
  //assume byte requests are file data, non byte requests are headers or single
  //FIXME TODO is this sufficient to replace 'IcnDefault' cache handling?
  uint64_t byteStart = 0, byteEnd = 0;
  bool nonheader = interest.GetUnsignedNamedAttribute("ByteStart", byteStart);
  nonheader = nonheader & interest.GetUnsignedNamedAttribute("ByteEnd", byteEnd);
  uint64_t tmp;
  bool header = interest.GetUnsignedNamedAttribute("Header", tmp);
  AcclContentName name = interest.GetAcclName();
  //new data, may need to purge
  ModuleManager::OnPktIngress(interest); //let utilities judge it
  if (interest.GetPacketPurpose() & PktType::DATAPKT) {
    if (header || (!header && !nonheader)) {
      CacheHdrHit(interest);
      //Create file entries, header has info
      uint64_t totalsize=0;
      bool sizeexist = interest.GetUnsignedNamedAttribute("TotalSize", totalsize);
      if (sizeexist)
        m_fileStore.SetFileSize(interest.GetAcclName(), totalsize);

      std::list < std::pair < double, AcclContentName> > PktList;
      //FIXME TODO need to loop until size in range
      if (header || nonheader) {
        PurgeBytesContent(interest);
      }
      else {
        PurgeICNContent(interest);
      }

    } else if (nonheader) {
      std::string strData;
      bool valid = interest.GetNamedAttribute("DATA", strData);
      if (!valid)
        return; //FIXME should we exit here?

      //only store if we have seen header before
      auto it = m_PktNames.find(interest.GetAcclName());
      if (m_PktNames.end() == it)
        return;

      std::vector<uint8_t> data;
      for(unsigned int i=0; i<strData.size(); i++) {
        data.push_back(strData[i]);
      }

      m_fileStore.SetDataRange(name, byteStart, byteEnd, data);
    }
  }        //interest matching
  else if (interest.GetPacketPurpose() & PktType::INTERESTPKT) {
    if (header || (!header && !nonheader)) {
      CacheHdrHit(interest);

    } else if (nonheader) {
      std::vector<uint8_t> data;
      data.resize(byteEnd-byteStart+1);
      bool exist = m_fileStore.GetDataRange(name,byteStart, byteEnd, data);
      if (exist) {
        LOG("Cache Segment Hit %s : byteStart %d \n", interest.GetName().GetFullName().c_str(), byteStart);
        interest.SetNamedAttribute("CacheHit", 1.0, true);
        interest.SetNamedAttribute("CacheNodeName", m_myNodeName);
        std::string strData(data.begin(),data.end());
        interest.SetNamedAttribute("DATA", strData);
      }
    }
  }
//check
  std::cout << "cache file done\n" << interest << "\n";
}

void
CacheBasicManager::IcnDefaultAction(PktType & interest) {
  ModuleManager::OnPktIngress(interest); //let utilities judge it


  //new data, may need to purge
  if (interest.GetPacketPurpose() & PktType::DATAPKT) {
    PurgeICNContent(interest);
  }            //interest matching
  else if (interest.GetPacketPurpose() & PktType::INTERESTPKT) {
    CacheIcnHit(interest);
  }
}

void CacheBasicManager::PurgeICNContent(PktType &pkt)
{
  if (!m_useStore) {
    return;
  }

  if ((m_PktNames.size()+(int) m_protectInsert)  > m_storageLimit) {
    std::list < std::pair < double, AcclContentName> > PktList, obsoletePktList;
    //Compute();
    CacheDataHandler(pkt, PktList);
    GetObsoleteList(PktList, obsoletePktList);
    if (obsoletePktList.size()) {
      LocalStoreDelete(obsoletePktList, false);
    }
    //Delete extra up to lower watermark
    if (m_PktNames.size() > m_lowerCacheWatermark) {
      auto itb = PktList.begin();
      auto ite = itb;
      advance(ite, m_PktNames.size()-m_lowerCacheWatermark);
      std::list < std::pair < double, AcclContentName> > PktListWm (itb, ite); //itb+m_PktNames.size()-m_lowerCacheWaterMark);
      LocalStoreDelete(PktListWm, false);
    }

  } else {
    m_cacheStore->SetData(pkt.GetAcclName(), pkt);
    m_PktNames.insert(pkt.GetAcclName());
  }

}
void CacheBasicManager::PurgeBytesContent(PktType &pkt)
{
  if (!m_useStore) {
    return;
  }
  uint64_t totalSize=0;
  pkt.GetUnsignedNamedAttribute("TotalSize", totalSize);
  if ( (m_fileStore.GetTotalBytesUsed()-(m_protectInsert)*totalSize) > (m_storageLimit)) {
    std::list < std::pair < double, AcclContentName> > PktList, obsoletePktList;
    //Compute();
    CacheDataHandler(pkt, PktList);
    GetObsoleteList(PktList, obsoletePktList);
    if (obsoletePktList.size()) {
      LocalStoreDelete(obsoletePktList, true);
    }
    //Delete extra up to lower watermark
    while (( m_fileStore.GetTotalBytesUsed()-m_protectInsert*totalSize) > (m_storageLimit) && PktList.size()) {
      auto first = PktList.begin();
      auto last = first;
      ++last;
      std::list < std::pair < double, AcclContentName> > PktListWm (first, last);
      PktList.erase(first);
      LocalStoreDelete(PktListWm, true);
    }
  } else {  //room in cache
    m_cacheStore->SetData(pkt.GetAcclName(), pkt);
    m_PktNames.insert(pkt.GetAcclName());


  }

}



//Get all ranks, then purge FIXME TODO.
//insert new content, evaluate it, and purge low/least value content
void CacheBasicManager::CacheDataHandler(PktType &interest, std::list< std::pair<double, AcclContentName> > &PktList) {

  //insert file if not protected
  if (!m_protectInsert) {
    if (m_useStore) {
      m_cacheStore->SetData(interest.GetAcclName(), interest);
    }
    m_PktNames.insert(interest.GetAcclName());
  }



  //RangeData < double >valueRange(0.0, m_dropValue);
  Compute();
  GetLowestNPackets (m_PktNames.size(), PktList);  //Need to test this first


  //protected insertion
  if (m_protectInsert) {
    if (m_useStore) {
      m_cacheStore->SetData(interest.GetAcclName(), interest);
    }
    m_PktNames.insert(interest.GetAcclName());
  }
}

void CacheBasicManager::CacheIcnHit(PktType & interest) {
  PktType fakePkt;

  //only do this for interest or data packets
  bool found = (m_PktNames.find(interest.GetAcclName()) != m_PktNames.end());

  if (found) { //give interest packet LOW value
    //give low priority, modify packet
    //std::cout << "\nInterest Update:" << interest.GetAcclName ();
    Compute(interest.GetAcclName());
    double value = Value(interest.GetAcclName());

    if (value <= m_dropValue) {
      LOG("Cache Expired Hit %s\n", interest.GetName().GetFullName().c_str());
      interest.SetNamedAttribute("CacheHit", 0.0, true);
      //m_statsHitExpired++;
      if (m_stats)
        m_stats->IncStats(m_statsHitExpired);
    } else {
      PktType newPacket;
      bool stat = m_cacheStore->ExistData(interest.GetAcclName(), newPacket);
      if (stat) {  //cachehit
        LOG("Cache Hit %s\n", interest.GetName().GetFullName().c_str());

        //Get DATA from cached packet
        //FIXME TODO jlm
        //This should be replaced by white/black/red chained lists
        interest.SetNamedAttribute("CacheHit", 1.0, true);
        interest.SetNamedAttribute("CacheNodeName", m_myNodeName);
        std::string datavctr;

        //FIXME TODO JLM create linked list of red/black/white lists and apply them
        //RED LIST Copy #segments, byte range and size, etc
        //For non headers, non segment packets (typical ICN)
        bool data = newPacket.GetNamedAttribute("DATA", datavctr);
        if (data)
        {
          interest.SetNamedAttribute("DATA", datavctr);
        }
        uint64_t value=0;
        bool exists = newPacket.GetUnsignedNamedAttribute("Segments", value);
        if (exists)
        {
          interest.SetUnsignedNamedAttribute("Segments", value);
        }

        //m_statsHit++;
        if (m_stats)
        {
          m_stats->IncStats(m_statsHit);
        }
      } else {  //not in store
        LOG("Cache Hit, but not found : Miss %s\n", interest.GetName().GetFullName().c_str());
        interest.SetNamedAttribute("CacheHit", 0.0, true);
        if (m_stats)
          m_stats->IncStats(m_statsMissNotFound);
      }
    }
  } else { //give interest packet HIGH value
    LOG("Cache Miss %s\n", interest.GetName().GetFullName().c_str());
    interest.SetNamedAttribute("CacheHit", 0.0, true);
    if (m_stats)
      m_stats->IncStats(m_statsMiss);
  }
}

//Check if cache hit (store1 for ICN packets or FILE HDR packets)
//TODO FIXME should be an asyncronous operation
void CacheBasicManager::CacheHdrHit(PktType & interest) {
  PktType fakePkt;

  //only do this for interest or data packets
  bool found = (m_PktNames.find(interest.GetAcclName()) != m_PktNames.end());

  if (found) { //give interest packet LOW value
    //give low priority, modify packet
    //std::cout << "\nInterest Update:" << interest.GetAcclName ();
    Compute(interest.GetAcclName());
    double value = Value(interest.GetAcclName());

    if (value <= m_dropValue) {
      LOG("Cache Expired Hit %s\n", interest.GetName().GetFullName().c_str());
      interest.SetNamedAttribute("CacheHit", 0.0, true);
      //m_statsHitExpired++;
      if (m_stats)
        m_stats->IncStats(m_statsHitExpired);
    } else {
      LOG("Cache Hit %s\n", interest.GetName().GetFullName().c_str());

      //Get DATA from cached packet
      //FIXME TODO jlm
      //This should be replaced by white/black/red chained lists
      PktType newPacket;
      bool stat = m_cacheStore->ExistData(interest.GetAcclName(), newPacket);
      if (stat) {  //cachehit
        interest.SetNamedAttribute("CacheHit", 1.0, true);
        interest.SetNamedAttribute("CacheNodeName", m_myNodeName);
        std::string datavctr;

        //FIXME TODO JLM create linked list of red/black/white lists and apply them
        //RED LIST Copy #segments, byte range and size, etc
        //For non headers, non segment packets (typical ICN)
        bool data = newPacket.GetNamedAttribute("DATA", datavctr);
        if (data)
        {
          interest.SetNamedAttribute("DATA", datavctr);
        }
        uint64_t value;
        bool exists;
        //Segments redlist (for headers with segments
        exists = newPacket.GetUnsignedNamedAttribute("Segments", value);
        if (exists)
        {
          interest.SetUnsignedNamedAttribute("Segments", value);
        }
        //TotalSize redList
        exists = newPacket.GetUnsignedNamedAttribute("TotalSize", value);
        if (exists)
        {
          interest.SetUnsignedNamedAttribute("TotalSize", value);
        }
        //SegSize redlist
        exists = newPacket.GetUnsignedNamedAttribute("SegSize", value);
        if (exists)
        {
          interest.SetUnsignedNamedAttribute("SegSize", value);
        }


      }

      //m_statsHit++;
      if (m_stats)
        m_stats->IncStats(m_statsHit);
    }
  } else { //give interest packet HIGH value
    LOG("Cache Miss %s\n", interest.GetName().GetFullName().c_str());
    interest.SetNamedAttribute("CacheHit", 0.0, true);
    if (m_stats)
      m_stats->IncStats(m_statsMiss);
  }
}

void
CacheBasicManager::OnPktEgress(PktType & data, const PktTxStatus & status) {

  if (PktType::DATAPKT != data.GetPacketPurpose()) {
    return;
  }
  ModuleManager::OnPktEgress(data, status); //let utilities judge it
}

void
CacheBasicManager::StoreActionsDone(const std::list < StoreEvents > &list) {
}

//StoreActionsDone JLM FIXME

void
CacheBasicManager::LocalStoreDelete(const std::list < std::pair < double, AcclContentName> > &list, bool isFile) {
  ModuleManager::LocalStoreDelete(list);

  for (auto itl = list.begin(); itl != list.end(); itl++) {
    {
      AcclContentName name = itl->second;
      //erase store value
      m_cacheStore->EraseData(name);
      if (isFile) {
        m_fileStore.DeleteFile(name);
      }

      std::cout << "Erased " << name << " with a value of " << name << "\n";

      m_PktNames.erase (name);
    }
  }
  //JLM FIX ME need to send out store delete event, or store insert event, as appropriate
  //we can also log deletions with valuations

}



//copy obsolete/expired content into new list
void
CacheBasicManager::GetObsoleteList( std::list < std::pair < double, AcclContentName> > &list, std::list < std::pair < double, AcclContentName> > &olist ) {


  RangeData < double >valueRange(0.0, m_dropValue);
  for (auto itl = list.begin(); itl != list.end(); itl++) {
    if (!valueRange.IsInRange(itl->first))
      return; //values are sorted numerically, we dont need to check each

    AcclContentName name = itl->second;
    //erase store value, pop onto new list
    olist.push_back(*itl);
    itl = list.erase(itl);

  }
}

//JLM FIX ME need to send out store delete event, or store insert event, as appropriate
//we can also log deletions with valuations


/*void
CacheBasicManager::FileStoreDelete(const std::list < std::pair < double, AcclContentName> > &list) {

  for (auto itl = list.begin(); itl != list.end(); itl++) {
    AcclContentName name = itl->second;

  }
  }*/
//JLM FIX ME need to send out store delete event, or store insert event, as appropriate
//we can also log deletions with valuations

