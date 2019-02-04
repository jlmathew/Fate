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
#ifndef _BASESTORAGE_H__
#define _BASESTORAGE_H__

#include <string>
#include <map>

enum StorageClass_t
{
  Storage_NotInitialized,
  Storage_defaultStore,
  Storage_MemMap,
  Storage_DBStore,
  Storage_MaxTypes
};

const StorageClass_t DEFAULT_STORAGE_CLASS = Storage_MemMap;



template < class dataName_t = AcclContentName, class UtilityData_t = double >
class memMap
{
private:

  std::map < dataName_t, UtilityData_t > m_utilityData;

public:
  memMap ()
  {
  }
   ~memMap ()
  {
  }

  std::map < dataName_t, UtilityData_t > *
  GetDataAsMap ()const
  {
    std::map < dataName_t, UtilityData_t > *tempMap =
      new std::map < dataName_t, UtilityData_t > (m_utilityData.begin (), m_utilityData.end ());
    return tempMap;

  }

  std::list < std::pair < dataName_t, UtilityData_t > >*
    GetDataAsList () const
  {
    std::list < std::pair < dataName_t, UtilityData_t > >*tempList =
      new std::list < std::pair < dataName_t, UtilityData_t > >(m_utilityData.begin (), m_utilityData.end ());
    return tempList;

  }
  void SetDataAsMap (const std::map < dataName_t, UtilityData_t > &mapp)
  {
    m_utilityData.clear ();
    m_utilityData = mapp;
  }
  void SetDataAsList (const std::list < std::pair < dataName_t, UtilityData_t > >&listt)
  {
    m_utilityData.clear ();
    // std::transform(listt.begin(), listt.end(), std::inserter(m_utilityData, m_utilityData.end()));
    for (typename std::list < std::pair < dataName_t,
         UtilityData_t > >::const_iterator it = listt.begin (); it != listt.end (); it++)
      {
        m_utilityData.insert (*it);
      }

  }
  std::string type ()
  {
    return "memstorage:";
  }
  bool empty () const
  {
    return m_utilityData.empty ();
  }

  UtilityData_t
    GetData (const dataName_t & key_tName) const
  {
    typename std::map < dataName_t, UtilityData_t >::const_iterator it;
      it = m_utilityData.find (key_tName);
    if (it == m_utilityData.end ())
      {                         //name doesnt exist
        std::string msg = key_tName;    //really need to make this more generic
        msg.append (": data not found");
        throw std::out_of_range (msg);
      }
    return (it->second);        //avoid creating entry if not found, add NULL check

  }

  bool 
  ExistData (const dataName_t & key_tName, UtilityData_t & retData)
  {
    typename std::map < dataName_t, UtilityData_t >::const_iterator it;

    it = m_utilityData.find (key_tName);
    if (it == m_utilityData.end ())
      {                         //name doesnt exist
        return false;
      }
    retData = it->second;
    return true;
  }

  void SetData (const dataName_t & key_tName, const UtilityData_t & updatedData)
  {
    m_utilityData[key_tName] = updatedData;
  }
  int64_t UpdateData (const dataName_t & key_tName, const UtilityData_t & updatedData)
  {
    uint64_t size;
    bool exist = (ExistData (key_tName, updatedData));
	    //ASUMPTION named data wont change
    if (!exist)
      {
        m_utilityData[key_tName] = updatedData;
	return updatedData.size();
      } else {
        return 0;
      }
  }
  void EraseData (const dataName_t & key_tName)
  {
    m_utilityData.erase (key_tName);
  }

  void EraseAll() 
  {
    m_utilityData.clear ();
  }

  uint64_t size () const
  {
    return m_utilityData.size ();
  }

  uint64_t EstMemoryUsed () const
  {
    uint64_t memUsed = 0;
    typename std::map < dataName_t, UtilityData_t >::const_iterator it;
    for (it = m_utilityData.begin (); it != m_utilityData.end (); it++)
      {
        memUsed += (it->first).size ();
        memUsed += sizeof (it->second); //fix for time JLM FIXME
        //memUsed+=(it->second).size();
      }
    return memUsed;
  }


    void Print(std::ostream &os, const std::string &delim=" = ",const std::string &eol="\n" ) const {
    typename std::map < dataName_t, UtilityData_t >::const_iterator it;

    for (it = m_utilityData.begin (); it != m_utilityData.end (); it++)
    {
        os << it->first << delim << it->second << eol;
    }
  }

};

template < class dataName_t = std::string, class UtilityData_t = double >
  class StorageClass
{
public:

  std::map <
    dataName_t,
  UtilityData_t > *
  GetDataAsMap () const
  {
    switch (m_storageType)
      {
        case Storage_MemMap:
          return m_storageElement.m_memMap->GetDataAsMap ();
          break;
          default:
            throw
            "Undefined storage type";
      }

  }

  void EraseAll(void) { throw "No type to erase";}

  std::list < std::pair < dataName_t, UtilityData_t > >*
  GetDataAsList () const
  {
    switch (m_storageType)
      {
        case Storage_MemMap:
          return m_storageElement.m_memMap->GetDataAsList ();
          break;
          default:
            throw
            "Undefined storage type";
      }

  }

  void
  SetDataAsMap (const std::map < dataName_t, UtilityData_t > &mapp)
  {
    switch (m_storageType)
      {
        case Storage_MemMap:
          m_storageElement.m_memMap->SetDataAsMap (mapp);
          break;
        default:
          throw "Undefined storage type";
      }

  }
  void
  SetDataAsList (const std::list < std::pair < dataName_t, UtilityData_t > >&listt)
  {
    switch (m_storageType)
      {
        case Storage_MemMap:
          m_storageElement.m_memMap->SetDataAsList (listt);
          break;
        default:
          throw "Undefined storage type";
      }

  }
  UtilityData_t
  GetData (const dataName_t & key_tName) const
  {
    switch (m_storageType)
      {
        case Storage_MemMap:
          return m_storageElement.m_memMap->GetData (key_tName);
          break;
          default:
            throw
            "Undefined storage type";
      }

  }
  void
  SetData (const dataName_t & key_tName, const UtilityData_t & updatedData)
  {
    switch (m_storageType)
      {
        case Storage_MemMap:
          m_storageElement.m_memMap->SetData (key_tName, updatedData);
          break;
        default:
          throw "Undefined storage type";
      }
  }

  bool ExistData (const dataName_t & key_tName, UtilityData_t & retData)
  {
    switch (m_storageType)
      {
        case Storage_MemMap:
          return m_storageElement.m_memMap->ExistData (key_tName, retData);
          break;
        default:
          throw "Undefined storage type";
      }

  }

  void
  EraseData (const dataName_t & key_tName)
  {
    switch (m_storageType)
      {
        case Storage_MemMap:
          m_storageElement.m_memMap->EraseData (key_tName);
          break;
        default:
          throw "Undefined storage type";
      }

  }

  uint64_t size ()const
  {
    switch (m_storageType)
      {
        case Storage_MemMap:
          return m_storageElement.m_memMap->size ();
          break;
          default:
            throw
            "Undefined storage type";
      }
  }

  uint64_t
  EstMemoryUsed () const
  {
    switch (m_storageType)
      {
        case Storage_MemMap:
          return m_storageElement.m_memMap->EstMemoryUsed ();
          break;
          default:
            throw
            "Undefined storage type";
      }
  }
  bool
  empty () const
  {
    switch (m_storageType)
      {
        case Storage_MemMap:
          return m_storageElement.m_memMap->empty ();
          break;
          default:
            throw
            "Undefined storage type";
      }
  }
  void
  setStorageType (StorageClass_t storageType)
  {

    if (Storage_defaultStore == storageType)
      {
        m_storageType = DEFAULT_STORAGE_CLASS;
      }
    else
      {
        m_storageType = storageType;
      }
    switch (m_storageType)
      {
        case Storage_MemMap:
          m_storageElement.m_memMap = new memMap < dataName_t, UtilityData_t >;
          break;
        default:
          throw "Undefined storage type";
      }

  }



StorageClass ()
  :m_storageType (Storage_NotInitialized)
  {
  }

  ~StorageClass ()
  {
       if (m_storageType != Storage_NotInitialized) {
         delete m_storageElement.m_memMap;
        }
  }

  //void begin() const{}
  std::string type ()
  {
    switch (m_storageType)
      {
        case Storage_MemMap:
          return m_storageElement.m_memMap->type ();
          break;
        default:
          throw "Undefined storage type";
      }
  }


private:
  StorageClass_t m_storageType;
  union storageElements_t
  {
    class memMap < dataName_t, UtilityData_t > * m_memMap;
  } m_storageElement;

};


#endif
