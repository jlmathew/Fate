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
#ifndef PACKETTYPEBASE__H
#define PACKETTYPEBASE__H

#include <stdint.h>
#include <string>
#include "ContentName.h"
#include <assert.h>
#include <map>
#include <list>
#include <vector>
#include <stdexcept>            // std::invalid_argument
#include "UniqueRegister.h"
#include <arpa/inet.h>
#include <string.h>
#include <type_traits>
//only here for testing
struct testSt_t
{
  int a;
  double b;
  char c;
};
class StoreEvents;
enum PktTxStatus
{
  Unknown = 0,
  SuccessfulTx,                 //Packet sent and verified
  Sent,                         //Packet sent, unknown if successful
  PartialSent, 			//Broadcast-Multicast packet, only sent to a few neighbors
  AllSent,			//Broadcast-multicast packet, all sent to correct neighbors
  TxFail,                       //Packet not sent successfully
  NotSentLowValue,              //Packet wasnt sent due to low valuation
  NotSentDestUnknown,
  InterestDropCacheHit,
  Corrupt,
};


class PktType
{
public:
  PktType ();
  PktType (const ContentName & name);
    PktType (const PktType & other);
    virtual ~ PktType ();


  enum PacketPurposeTypes_t
  {
    ILLEGALPKT = 0,
    //our private packet types
    CONTROLPKT = 1,
    DEBUGPKT = 2,

    //CCN types/Basic packet functions
    INTERESTPKT = 4,
    DATAPKT = 8,
    INTERESTRESPONSEPKT = 16,
    DISCOVERYPKT = 32,
    DISCOVERYRESPONSEPKT = 64, 

    //Last
    NUM_PCK_TYPES
  };
  enum dataPktAttributeType_t
  {
    illegalType_t = 0,
    stringType_t,
    rawType_t,
    stlType_t,
    u64endianType_t,
    i64endianType_t,
    dblType_t,
    MAXTYPES
  };

  class dataTypeValue_t
  {
  public:
    dataPktAttributeType_t type;
    uint32_t rawDataSize;
    union
    {
      dataNameType_t *pstrValue;
      uint8_t *rawData;
      double dblValue;
      uint64_t u64Value;
      int64_t i64Value;
    } data;

    void copy (const dataTypeValue_t & other)
    {
      //delete any existing pointer data
      if (type == stringType_t)
        {
          delete data.pstrValue;
        }
      else if (type == rawType_t)
        {
          delete[] data.rawData;
        }

      type = other.type;

      rawDataSize = other.rawDataSize;
      if (type == stringType_t)
        {
          dataNameType_t *copyStr = new dataNameType_t (*(other.data.pstrValue));

          data.pstrValue = copyStr;
        }
      else if (type == rawType_t)
        {
          uint8_t *copyRaw = new uint8_t[rawDataSize];

          memcpy (copyRaw, other.data.rawData, rawDataSize);
          data.rawData = copyRaw;
        }
      else
        {
          data = other.data;
        }
    }
    dataTypeValue_t & operator= (const dataTypeValue_t & other)
    {
      copy (other);

      return *this;
    }



    dataTypeValue_t (const dataTypeValue_t & other)
    {
      type = illegalType_t;
      copy (other);
    }

    dataTypeValue_t ()
    {
      type = illegalType_t;
      rawDataSize = 0;
      data.u64Value = 0;
    }

    virtual ~ dataTypeValue_t ()
    {
      if (type == stringType_t && data.pstrValue)
        {
          delete data.pstrValue;
        }
      else if (type == rawType_t && data.rawData)
        {
          delete [] data.rawData;
        }
    }


  };

  virtual uint64_t size () const;
  virtual ContentName GetName () const;
  virtual void SetName (const ContentName & newName);

  virtual AcclContentName GetAcclName () const;

  virtual void ClearData ();
  virtual void ClearTempData ();
  virtual void ClearPersistantData ();

  virtual PktType & operator= (const PktType & other);


  //JLM TODO this will leak 
  //when we clear dataTypeValue_t, we MUST
  //free pstrValue and rawData;
  //rawData should be an array of uint8_t

  //templates do not inheirit base class.
  //want to make these methods available, but at the base class, just assert
  //memcpy(&data, sizeof(data))
  template < class T > 
  bool
  GetObjectCpyNamedAttribute (const dataNameType_t & name, T & value, bool tempAttribute = false) const
  {
    std::map < dataNameType_t, dataTypeValue_t >::const_iterator cit;
    bool retValue = true;
    if (tempAttribute)
      {
        cit = m_tempAttr.find (name);
        if (m_tempAttr.end () == cit)
          {
            retValue = false;
          }
      }
    else
      {
        cit = m_namedAttr.find (name);
        if (m_namedAttr.end () == cit)
          {
            retValue = false;
          }
      }

    if (retValue)
      {
        value = *(reinterpret_cast < const T * >((cit->second.data.rawData)));
        if ((cit->second).type != rawType_t)
          {
            throw std::invalid_argument ("Not a Memory Copied Object");
          }

      }
    return retValue;

  }

void
SetRawCpyNamedAttribute(dataNameType_t name, uint8_t *buffer, uint16_t size, bool tempAttribute);

//notworking
template <typename Iter>
void SetObjectCpyIteratorNamedAttribute ( dataNameType_t name, Iter it, Iter end, bool tempAttribute = false)  {
using T = typename std::iterator_traits<Iter>::value_type;

    std::vector< T > tarray;
uint32_t size=0;
    for (; it!=end; ++it)
     {
         tarray.push_back(*it);
         if (std::is_pod<T>::value) {  //use is_pod, is_scalar?
           size += sizeof(T);
         } else {
           size += *it.size();
         }

     }

     SetRawCpyNamedAttribute(name, tarray, size, tempAttribute);

  }


//notworking
template <typename Iter>
bool GetObjectCpyIteratorNamedAttribute (const  dataNameType_t &name, Iter &it, bool tempAttribute = false)  {

using T = typename std::iterator_traits<Iter>::value_type;
//typedef typename std::iterator_traits<InputIt>::value_type Result
uint16_t totalsize;
uint8_t *buffer;
bool found = GetRawCpyNamedAttribute(name, &buffer, totalsize);
if(!found) return false;

//JLM need to test both Get's and Sets, use vector and map.
uint32_t size=0;
T element;
    while (size < totalsize)
     {
         
         it->insert(element);
         if (std::is_pod<Iter>::value) {  //use is_arthmetic, is_scalar?
           size += sizeof(T);
         } else {
           size += element.size();
         }

     }
    delete[] buffer;
    return true;
  }

//notworking
template <typename Iter>
void SetPrintedIteratorNamedAttribute ( dataNameType_t name, Iter it, Iter end, bool tempAttribute = false) {
    std::stringstream ss;
    for (; it!=end; ++it)
     {
         ss << *it << " ";
//std::cout << *it << " ";

     }
     SetNamedAttribute(name, ss.str(), tempAttribute);

  }

//notworking
template <typename Iter>
void GetPrintedIteratorNamedAttribute ( dataNameType_t name, Iter &it, bool tempAttribute = false) {
   //using T = typename std::iterator_traits<Iter>::value_type; //if want type, passing iterator
   typedef typename Iter::value_type T;  //if want type, passing container
    std::istringstream is;
    dataNameType_t buffer;
    bool retVal = GetNamedAttribute (name, buffer, tempAttribute);

    is.str (buffer);
    T element;


    while (is.peek() != decltype(is)::traits_type::eof())
     {
        is >> element;
        it.insert(it.end(),element);

     }

  }

  template < class T > 
    void 
    SetObjectCpyNamedAttribute (const dataNameType_t & name, const T & value, bool tempAttribute, uint32_t size)
  {
    DeleteNamedAttribute (name, tempAttribute);
    dataTypeValue_t dataValue;
    
    dataValue.type = rawType_t;
    dataValue.rawDataSize = size;
    //std::cout << "size is : " << dataValue.rawDataSize << "\n";
    dataValue.data.rawData = new uint8_t[dataValue.rawDataSize];
    memcpy (dataValue.data.rawData, &value, dataValue.rawDataSize);
    if (tempAttribute)
      {
        m_tempAttr[name] = dataValue;
      }
    else
      {
        m_namedAttr[name] = dataValue;
      }

  }

  template < class T > 
    void 
    SetObjectCpyNamedAttribute (const dataNameType_t & name, const T & value, bool tempAttribute = false)
  {
     SetObjectCpyNamedAttribute(name, value, tempAttribute, sizeof(T));
  }

  //cout << variable
  template < class T > 
    bool 
    GetPrintedNamedAttribute (const dataNameType_t & name, T & value, bool tempAttribute = false)
  {
    std::istringstream is;
    dataNameType_t buffer;
    bool retVal = GetNamedAttribute (name, buffer, tempAttribute);

    is.str (buffer);
    is >> value;
    return retVal;

  }
bool
GetRawCpyNamedAttribute(const dataNameType_t &name, uint8_t **buffer, uint16_t &size, bool tempAttribute = false);


  template < class T > 
    void 
    SetPrintedNamedAttribute (const dataNameType_t & name, const T & value, bool tempAttribute = false)
  {
    DeleteNamedAttribute (name, tempAttribute);
    std::ostringstream os;
    os << value << std::flush;
    SetNamedAttribute (name, os.str (), tempAttribute);
  }

  virtual bool DeleteNamedAttribute (const dataNameType_t & name, bool tempAttribute = false);
  virtual bool DeleteAllAttributes (bool tempAttribute = true);

  virtual bool GetNamedAttributeType (const dataNameType_t & name,
                                      dataPktAttributeType_t & value, bool tempAttribute = false) const;

  virtual bool GetNamedAttribute (const dataNameType_t & name, dataNameType_t & value, bool tempAttribute = false) const;
  virtual void SetNamedAttribute (const dataNameType_t & name, const dataNameType_t & value, bool tempAttribute = false);

  virtual bool GetNamedAttribute (const dataNameType_t & name, double &value, bool tempAttribute = false) const;
  virtual void SetNamedAttribute (const dataNameType_t & name, double value, bool tempAttribute = false);

  virtual bool GetUnsignedNamedAttribute (const dataNameType_t & name, uint64_t & value, bool tempAttribute = false) const;
  virtual void SetUnsignedNamedAttribute (const dataNameType_t & name, uint64_t value, bool tempAttribute = false);

  virtual bool GetSignedNamedAttribute (const dataNameType_t & name, int64_t & value, bool tempAttribute = false) const;
  virtual void SetSignedNamedAttribute (const dataNameType_t & name, int64_t value, bool tempAttribute = false);


  virtual PacketPurposeTypes_t GetPacketPurpose () const;

  void SetPacketPurpose (PacketPurposeTypes_t purpose);


  void Serialize (std::vector < uint8_t > &serialData);

  void Deserialize (const std::vector < uint8_t > &serialData);

  //redirect

  friend std::ostream & operator << (std::ostream & os, const PktType & i);

//{ i.Print(os); return os; }

  friend std::istream & operator >> (std::istream & iss, PktType & i);

//Need copy operator FIXME TODO

private:
  ContentName m_contentName;
  bool m_nameValid;
  void serial64Helper (std::vector < uint8_t > &serialData, uint64_t data);

  void serial16Helper (std::vector < uint8_t > &serialData, uint16_t data);
  void serialAttributeHelper (std::vector < uint8_t > &serialData, const dataTypeValue_t & data);
  uint16_t deserial16Helper (const std::vector < uint8_t > &serialData, int &i);
  uint64_t deserial64Helper (const std::vector < uint8_t > &serialData, int &i);
  dataNameType_t deserialStringHelper (const std::vector < uint8_t > &serialData, int &i, int size);
  void deserialAttributeHelper (const std::vector < uint8_t > &serialData, int &i, dataTypeValue_t & data);

  void copy (const PktType & otherPkt);

  std::map < dataNameType_t, dataTypeValue_t > m_namedAttr;
  std::map < dataNameType_t, dataTypeValue_t > m_tempAttr;
  PacketPurposeTypes_t m_purpose;
};



#endif
