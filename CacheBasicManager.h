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
#ifndef CACHEBASICMANAGER_H_
#define CACHEBASICMANAGER_H_

#include "ModuleManager.h"
#include "BaseStorage.h"
#include "StoreManager.h"
//#include "UtilityFunctionGenerator.h"
#include "RangeData.h"
#include "PacketTypeBase.h"
#include "NodeManager.h"
#include "BaseFileStorage.h"

class CacheBasicManager:public ModuleManager
{
public:
  enum CachePacketType {
    Invalid,
    IcnDefault,
    IcnFile,

    maxTypes
  };
  CacheBasicManager ();

  CacheBasicManager (ConfigWrapper & config);
  virtual ~ CacheBasicManager ();


  virtual bool OnInit (UtilityExternalModule *);

  virtual void Config (ConfigWrapper & config);

  virtual bool AddConfig (UtilityHandlerBase * uc, uint16_t position = 0);

  virtual void CacheHdrHit(PktType &interest);
  virtual void CacheDataHandler(PktType &interest, std::list< std::pair<double,AcclContentName>  > &PktList, bool useIcnFileLimit=false);

  //delete Configs by name or number

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("CacheBasicManager");
      return idName;
  }

  //store connectivity

  void SetStore (StoreManager *);

  void OnPktIngress (PktType & interest);       //Rx
  void OnPktEgress (PktType & data, const PktTxStatus & status);        //Rx

  void StoreActionsDone (const std::list < StoreEvents > &list);
  void LocalStoreDelete (const std::list < std::pair< double, AcclContentName> > &list);
  void FileStoreDelete (const std::list < std::pair< double, AcclContentName> > &list);
  void DoStoreActions (const std::list < StoreEvents > &list);

private:

  //default actions
  void OnDataInterestPktIngress (PktType & interest);
  void OnControlPktIngress (PktType & control);
  void OnDebugPktIngress (PktType & debug);
  void IcnDefaultAction(PktType &pkt);
  void IcnFileAction(PktType &pkt);
  
  TypicalCacheStore *m_cacheStore;
  TypicalCacheStore *m_cacheFileStore;
  uint64_t m_storageLimit;

  std::string m_cacheStoreName;

  std::string m_statsMiss;
  std::string m_statsHit;
  std::string m_statsHitExpired;
  std::string m_statsFileHit;
  std::string m_statsFileMiss;
  double m_dropValue;
  std::string m_moduleName; //hierarchical name: nodeName/moduleName
  bool m_useStore;
  bool m_protectInsert;
  CachePacketType m_contentType;
  const std::vector< std::string > m_contentTypeNames = {"Invalid","IcnDefault","IcnFile"};
  AcclContentName m_matchHeaderName;
  BaseFileStorage  m_fileStore;  //FIXME TODO set max size of fileStore
  std::string m_myNodeName;
  bool m_deleteByValue;
};

#endif
