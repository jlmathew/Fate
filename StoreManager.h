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
#ifndef STOREMANAGERBASE_H_
#define STOREMANAGERBASE_H_

#include <vector>
#include "UtilityHandlerBase.h"
//#include "UtilityFunctionGenerator.h"

#include "UtilityExternalModule.h"
#include "PacketTypeBase.h"
#include "BaseStorage.h"
#include "ContentName.h"

class AsyncEvent;

//class UtilityHandlerBase;
class StoreManager:public EventHandlerBase
{                               //StorageClass < class key, class data > {
public:

  StoreManager ();
  StoreManager (ConfigWrapper & config);

  virtual ~ StoreManager ();
  virtual bool OnInit (UtilityExternalModule *);
  virtual void Config (ConfigWrapper & config);

  virtual const std::string & Name () const;
  virtual void OnAsyncEvent (AsyncEvent & eventType);

  void OnPktIngress (PktType & interest);       //Rx

  void OnPktEgress (PktType & data, const PktTxStatus & status);        //Rx
  void OnPrePktEgress (PktType & data); 
  virtual void DoDelete (const AcclContentName & name);

  virtual void ComputeAndValue (std::list < PktType > &pktList);
  virtual void Compute ();
  virtual void Compute (const AcclContentName & name);
  virtual double Value (const AcclContentName & name) const;

  virtual void LocalStoreDelete (const AcclContentName & name);
  virtual uint64_t EstMemoryUsed (void) const;

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("StoreManagerFunct");
      return idName;
  }
  virtual void Print (std::ostream & os, bool endParen = true) const;

  virtual void Print (std::ostream & os, const AcclContentName & tlvName, double &value) const;


  uint64_t GetStorageLimit () const;

  void SetStorageLimit (uint64_t);


protected:
  std::string m_name;
  UtilityExternalModule *m_externalModule;
  bool m_useAlias, m_useAliasName, m_useProxyName, m_usePrettyPrint;
  uint64_t m_storageLimit;
  StorageClass_t m_storageMethod;

  //StorageClass < key,  data> *m_storageType;

};


//class CacheStore : public StorageManager<dataNameType_t, PktType> {
//template<std::string, PktType>
class TypicalCacheStore:public StoreManager, public StorageClass < AcclContentName, PktType >
{
public:
  TypicalCacheStore ()
  {
  };

  TypicalCacheStore (ConfigWrapper & config)
  {
    Config (config);
  };
  virtual ~ TypicalCacheStore ()
  {
  }

/* void
OnPktIngress (PktType & pkt)
{
   if (pkt.GetPacketPurpose() & PktType::CONTROLPKT)
   {
        std::string key,  name;
	std::string value;
//match store name
       bool bname = pkt.GetNamedAttribute("Name", name);
std::cout << "Looking for store " << name << ", at store " << m_name << "\n";
       if (bname && name==m_name) {
         bool bkey = pkt.GetNamedAttribute("Key", key);
         bool bvalue = pkt.GetUnsignedNamedAttribute("value", value);
         uint64_t ddata=0;
//note hardcoded to double for now, should cover 'most' requirements, need to make more flexible
         std::list<std::string,double> tlist;
         if (bkey && bvalue) {
                //read map first, then update

                std::size_t pos = key.find(";");
                if (pos != std::string::npos)
		{
		  std::string key1=key.substr(0,pos);
                  key = key.substr(pos+1);

                  std::size_t dpos = value.find(";");
                  if (dpos != std::string::npos) {
			std::string ddata=value.substr(0,pos);
			value = value.substr(pos+1);
                        std::cout << "store key:" << key1 "," << value << "\n";
                        double dvalue = ::atof(value.c_str());
                        tlist.push_back(std::make_pair(key1,dvalue);
		  }
		  } else { bkey=false; } //mismatch string to data
		} else { bkey = false; }

                //assume ';' breaks each key/value
                //SetData(key,data)
                //ExistData()
                //EraseData()
                //SetDataAsList
		//StorageClass<AcclContentName, PktType>::SetDataAsList();
       }
     }
   } 
}    */                
  void Config (ConfigWrapper & config)
  {
    StoreManager::Config (config);
    setStorageType (m_storageMethod);
  }
  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("CacheBasicStore");

    return idName;
  }

};

#endif
