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
#include "PacketTypeBase.h"
#include "UtilityFunctionGenerator.h"
#include "UniqueRegister.h"
#include "pugixml.h"
#include <utility>

extern RegisterTemplate < dataNameType_t, uint16_t > GlobalAttributeNameRegistration;
extern RegisterTemplate < uint16_t,
  PktType::dataPktAttributeType_t >
  GlobalAttributeNameTypeRegistration;


PktType::PktType ():
m_nameValid (false), m_purpose (ILLEGALPKT)
{

}

PktType::PktType (const ContentName & name):
m_purpose (ILLEGALPKT)
{
  SetName (name);
  m_nameValid = true;
}

PktType::PktType (const PktType & other)
{
  copy (other);
}


PktType & PktType::operator= (const PktType & other)
{
  copy (other);

  return *this;
}

PktType::~PktType ()
{
//check if the maps clear the string (pointer) data
  ClearData ();
}

ContentName
PktType::GetName () const
{
  return m_contentName;
}

uint64_t 
PktType::size () const
{
  return
  0;
}                          //JLM FIXME

bool
PktType::DeleteNamedAttribute (const dataNameType_t & name, bool tempAttribute)
{
	size_t numDel=0;
	if (tempAttribute) {
         numDel = m_tempAttr.erase (name);
	} else {
         numDel = m_namedAttr.erase(name);
	}	

  numDel = m_tempAttr.erase (name);


  return numDel ? false : true;
  /*
     std::map< dataNameType_t, dataTypeValue_t >::iterator it;
     bool retValue=true;
     if (tempAttribute) {
     it = m_tempAttr.find(name);
     if (m_tempAttr.end() == it) {
     retValue = false;
     }
     } else {
     it = m_namedAttr.find(name);
     if (m_namedAttr.end() == it) {
     retValue = false;
     }
     }
     if(retValue) {
     if (it->second.type == stringType_t) {
     delete (it->second.data).pstrValue;
     } else if (it->second.type == rawType_t) {
     delete it->second.data.rawData;
     }
     m_namedAttr.erase(it);
     }
     return retValue;
   */

}

bool PktType::DeleteAllAttributes (bool tempAttribute)
{
  if (tempAttribute)
    {
      ClearTempData ();
    }
  else
    {
      ClearPersistantData ();
    }
  return true;
}

void
PktType::SetName (const ContentName & name)
{
  //FIXME TODO check if setname is set twice, it works JLM
  m_contentName = name;
  m_nameValid = true;
}

     bool PktType::GetNamedAttribute (const dataNameType_t & name, dataNameType_t & value, bool tempAttribute) const
     {
       std::map <
         dataNameType_t,
         dataTypeValue_t >::const_iterator
         cit;
       bool
         retValue = true;

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
           if (retValue && ((cit->second).type != stringType_t))
             {
               throw
               std::invalid_argument ("Not a string");
             }
           value = dataNameType_t (*(cit->second.data.pstrValue));
         }
       return retValue;



     }

     bool PktType::GetNamedAttributeType (const dataNameType_t & name, dataPktAttributeType_t & value, bool tempAttribute) const
     {
       std::map <
         dataNameType_t,
         dataTypeValue_t >::const_iterator
         cit;
       bool
         retValue = true;

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
           value = (cit->second).type;
           if (value >= MAXTYPES)
             {
               throw
               std::invalid_argument ("Not a string");
             }
         }
       return retValue;

     }

void
PktType::copy (const PktType & otherPkt)
{
  ClearData ();
  m_purpose = otherPkt.m_purpose;
  m_contentName = otherPkt.m_contentName;
  m_nameValid = otherPkt.m_nameValid;
  m_namedAttr = otherPkt.m_namedAttr;
  m_tempAttr = otherPkt.m_tempAttr;
}

void
PktType::SetNamedAttribute (const dataNameType_t & name, const dataNameType_t & value, bool tempAttribute)
{
  //DeleteNamedAttribute(name, tempAttribute);
  dataTypeValue_t dataValue;

  dataValue.type = stringType_t;
  dataValue.data.pstrValue = new dataNameType_t (value);

  if (tempAttribute)
    {
      m_tempAttr[name] = dataValue;
    }
  else
    {
      m_namedAttr[name] = dataValue;
    }

}

     bool PktType::GetNamedAttribute (const dataNameType_t & name, double &value, bool tempAttribute) const
     {
       std::map <
         dataNameType_t,
         dataTypeValue_t >::const_iterator
         cit;
       bool
         retValue = true;

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
           value = cit->second.data.dblValue;
           if ((cit->second).type != dblType_t)
             {
               throw
               std::invalid_argument ("Not a double");
             }

         }
       return retValue;

     }


void
PktType::SetNamedAttribute (const dataNameType_t & name, double value, bool tempAttribute)
{
  //DeleteNamedAttribute(name, tempAttribute);
  dataTypeValue_t dataValue;

  dataValue.type = dblType_t;
  dataValue.data.dblValue = value;
  if (tempAttribute)
    {
      m_tempAttr[name] = dataValue;
      //m_tempAttr.insert(std::make_pair(name, dataValue));
    }
  else
    {
      m_namedAttr[name] = dataValue;
    }

}

     bool PktType::GetUnsignedNamedAttribute (const dataNameType_t & name, uint64_t & value, bool tempAttribute) const
     {
       std::map <
         dataNameType_t,
         dataTypeValue_t >::const_iterator
         cit;
       bool
         retValue = true;

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
           value = cit->second.data.u64Value;
           if ((cit->second).type != u64endianType_t)
             {
               throw
               std::invalid_argument ("Not a uint_64");
             }

         }
       return retValue;

     }

void
PktType::SetUnsignedNamedAttribute (const dataNameType_t & name, uint64_t value, bool tempAttribute)
{
  //DeleteNamedAttribute(name, tempAttribute);
  dataTypeValue_t dataValue;

  dataValue.type = u64endianType_t;
  dataValue.data.u64Value = value;
  if (tempAttribute)
    {
      m_tempAttr[name] = dataValue;
    }
  else
    {
      m_namedAttr[name] = dataValue;
    }

}

     bool PktType::GetSignedNamedAttribute (const dataNameType_t & name, int64_t & value, bool tempAttribute) const
     {
       std::map <
         dataNameType_t,
         dataTypeValue_t >::const_iterator
         cit;
       bool
         retValue = true;

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
           value = cit->second.data.i64Value;
           if ((cit->second).type != i64endianType_t)
             {
               throw
               std::invalid_argument ("Not an int64");
             }
         }
       return retValue;

     }

void
PktType::SetSignedNamedAttribute (const dataNameType_t & name, int64_t value, bool tempAttribute)
{
  //DeleteNamedAttribute(name, tempAttribute);
  dataTypeValue_t dataValue;

  dataValue.type = i64endianType_t;
  dataValue.data.i64Value = value;
  if (tempAttribute)
    {
      m_tempAttr[name] = dataValue;
    }
  else
    {
      m_namedAttr[name] = dataValue;
    }

}
void
PktType::SetRawCpyNamedAttribute(dataNameType_t name, uint8_t *buffer, uint16_t size, bool tempAttribute)
{
    DeleteNamedAttribute (name, tempAttribute);
    dataTypeValue_t dataValue;
    dataValue.type = rawType_t;
    dataValue.rawDataSize = size;
    dataValue.data.rawData = new uint8_t[dataValue.rawDataSize];
    memcpy (dataValue.data.rawData, buffer, dataValue.rawDataSize);
    if (tempAttribute)
      {
        m_tempAttr[name] = dataValue;
      }
    else
      {
        m_namedAttr[name] = dataValue;
      }

}

bool
PktType::GetRawCpyNamedAttribute(const dataNameType_t &name, uint8_t **buffer, uint16_t &size, bool tempAttribute)
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
        
        if ((cit->second).type != rawType_t)
          {
            throw std::invalid_argument ("Not a Memory Copied Object");
          }
        size = cit->second.rawDataSize;
        *buffer = new uint8_t[size];
        memcpy(*buffer, cit->second.data.rawData, size);

      }
    return retValue;


}

PktType::PacketPurposeTypes_t PktType::GetPacketPurpose () const
{
  return m_purpose;
}

void
PktType::SetPacketPurpose (PacketPurposeTypes_t purpose)
{
  if (purpose > NUM_PCK_TYPES)
    {
      throw std::invalid_argument ("out of bounds");
    }
  else
    {
      m_purpose = purpose;
    }
}

uint16_t PktType::deserial16Helper (const std::vector < uint8_t > &serialData, int &i)
{
  uint16_t
    retVal = ntohs (*(reinterpret_cast < const uint16_t * >(&serialData[i])));

  i += 2;
  return retVal;
}

dataNameType_t PktType::deserialStringHelper (const std::vector < uint8_t > &serialData, int &i, int size)
{
  dataNameType_t
    retVal;                     // = new dataNameType_t;

  for (int j = 0; j < size; j++)
    {
      char
        charByte;

      charByte = serialData[j + i];
      retVal.push_back (charByte);
    }

  i += size;
  return retVal;
}

void
PktType::deserialAttributeHelper (const std::vector < uint8_t > &serialData, int &i, dataTypeValue_t & data)
{

  uint16_t length;
  dataNameType_t stringName;

  switch (data.type)
    {
      case stringType_t:
        length = deserial16Helper (serialData, i);
        data.rawDataSize = length; //not used, as we treat data as string \0 terminated
        stringName = deserialStringHelper (serialData, i, length);
        data.data.pstrValue = new dataNameType_t (stringName);
        break;
      case rawType_t:
        //length = data.rawDataSize;
        length = deserial16Helper (serialData, i);
        data.rawDataSize = length;
        data.data.rawData = new uint8_t[length];
        for (int j = 0; j < length; j++)
          {
            data.data.rawData[j] = serialData[j + i];
          }
        i += length;
        break;
      case u64endianType_t:
        data.data.u64Value = deserial64Helper (serialData, i);
        //i += sizeof(uint64_t);
        break;
      case i64endianType_t:
        data.data.i64Value = static_cast < int64_t > (deserial64Helper (serialData, i));
        //i += sizeof(int64_t);
        break;
      case dblType_t:
        data.data.dblValue = *(reinterpret_cast < const double *>(&serialData[i]));
        i += sizeof (double);
        break;

      default:
        assert (0);             //no type defined
        break;

    }


}

void
PktType::Serialize (std::vector < uint8_t > &serialData)
{
  //packet purpose
  serial16Helper (serialData, m_purpose);

  //write 'name type'
  dataNameType_t serialName = m_contentName.GetFullName ();

  uint16_t size = serialName.size ();

  serial16Helper (serialData, size);
  for (int i = 0; i < size; i++)
    {
      serialData.push_back (serialName[i]);
    }

  //write how many regular/temp
  serial16Helper (serialData, m_namedAttr.size ());
  serial16Helper (serialData, m_tempAttr.size ());

  for (std::map < dataNameType_t, dataTypeValue_t >::const_iterator cit = m_namedAttr.begin (); cit != m_namedAttr.end (); cit++)
    {
      uint16_t type = GlobalAttributeNameRegistration.Register (cit->first);
      dataTypeValue_t dataType = (cit->second);

      //packet attributes are tuples: attribute name-Id/data type/length of data/data
      serial16Helper (serialData, type);
      serial16Helper (serialData, static_cast < uint16_t > (dataType.type));
      serialAttributeHelper (serialData, cit->second);
    }
  for (std::map < dataNameType_t, dataTypeValue_t >::const_iterator cit = m_tempAttr.begin (); cit != m_tempAttr.end (); cit++)
    {
      uint16_t type = GlobalAttributeNameRegistration.Register (cit->first);
      dataTypeValue_t dataType = (cit->second);

      //packet attributes are tuples: attribute name-Id/data type/data
      serial16Helper (serialData, type);
      serial16Helper (serialData, static_cast < uint16_t > (dataType.type));
      serialAttributeHelper (serialData, cit->second);
    }
}

void
PktType::serial16Helper (std::vector < uint8_t > &serialData, uint16_t data)
{
  uint16_t netFormat = htons (data);
  uint8_t *bPtr = reinterpret_cast < uint8_t * >(&netFormat);

  serialData.push_back (bPtr[0]);
  serialData.push_back (bPtr[1]);
}

void
PktType::serial64Helper (std::vector < uint8_t > &serialData, uint64_t data)
{
  uint32_t netFormatBig = htonl (data >> 32);
  uint32_t netFormatSmall = htonl (data & 0xffffffff);
  uint8_t *bPtr = reinterpret_cast < uint8_t * >(&netFormatBig);

  for (int i = 0; i < 4; i++)
    {
      serialData.push_back (bPtr[i]);
    }
  bPtr = reinterpret_cast < uint8_t * >(&netFormatSmall);
  for (int i = 0; i < 4; i++)
    {
      serialData.push_back (bPtr[i]);
    }
}

uint64_t PktType::deserial64Helper (const std::vector < uint8_t > &serialData, int &i)
{


  //uint64_t data = *(reinterpret_cast<const uint64_t *>(&serialData[i]));
  uint32_t
    dataBig = ntohl (*(reinterpret_cast < const uint32_t * >(&serialData[i])));
  uint32_t
    dataSmall = ntohl (*(reinterpret_cast < const uint64_t * >(&serialData[i + 4])));

  i += sizeof (uint64_t);


  uint64_t
    data = dataBig;

  data = data << 32;
  data = data | dataSmall;
  return data;
}

void
PktType::serialAttributeHelper (std::vector < uint8_t > &serialData, const dataTypeValue_t & data)
{
  uint16_t length;
  uint8_t *bPtr;
  double value;

  switch (data.type)
    {
      case stringType_t:
        length = strlen ((data.data.pstrValue)->c_str ());
        serial16Helper (serialData, length);
        for (int i = 0; i < length; i++)
          {
            serialData.push_back (((data.data.pstrValue->c_str ()))[i]);
          }
        break;
      case rawType_t:
        length = data.rawDataSize;
        serial16Helper (serialData, length);
        for (int i = 0; i < length; i++)
          {
            serialData.push_back (data.data.rawData[i]);
          }
        break;
      case u64endianType_t:
        length = sizeof (uint64_t);
        //serial16Helper(serialData, length);
        serial64Helper (serialData, data.data.u64Value);
        break;
      case i64endianType_t:
        length = sizeof (int64_t);
        //serial16Helper(serialData, length);
        serial64Helper (serialData, static_cast < uint64_t > (data.data.i64Value));
        break;
      case dblType_t:
        length = sizeof (double);
        //serial16Helper(serialData, length);
        value = data.data.dblValue;
        bPtr = reinterpret_cast < uint8_t * >(&value);
        for (int i = 0; i < length; i++)
          {
            serialData.push_back (bPtr[i]);
          }
        break;

      default:
        assert (0);             //no type defined
        break;

    }

}

void
PktType::Deserialize (const std::vector < uint8_t > &serialData)
{
  ClearData ();

  int i = 0;

  m_purpose = static_cast < PacketPurposeTypes_t > (deserial16Helper (serialData, i));

  uint16_t size = deserial16Helper (serialData, i);

  dataNameType_t serialName = deserialStringHelper (serialData, i, size);

  m_contentName.SetFullName (serialName);

  uint16_t numAttributes = deserial16Helper (serialData, i);
  uint16_t tempAttributes = deserial16Helper (serialData, i);

  for (int j = 0; j < numAttributes; j++)
    {
      uint16_t type = deserial16Helper (serialData, i);
      dataNameType_t attrName = GlobalAttributeNameRegistration.DeRegister (type);
      uint16_t datatype = deserial16Helper (serialData, i);
      dataTypeValue_t dataType;

      dataType.type = static_cast < PktType::dataPktAttributeType_t > (datatype);
      deserialAttributeHelper (serialData, i, dataType);
      m_namedAttr[attrName] = dataType;
    }
  for (int j = 0; j < tempAttributes; j++)
    {
      uint16_t type = deserial16Helper (serialData, i);
      dataNameType_t attrName = GlobalAttributeNameRegistration.DeRegister (type);
      uint16_t datatype = deserial16Helper (serialData, i);
      dataTypeValue_t dataType;

      dataType.type = static_cast < PktType::dataPktAttributeType_t > (datatype);
      deserialAttributeHelper (serialData, i, dataType);
      m_tempAttr[attrName] = dataType;
    }
}

void
PktType::ClearTempData ()
{
  std::map < dataNameType_t, dataTypeValue_t >::iterator it;
  for (it = m_tempAttr.begin (); it != m_tempAttr.end (); it++)
    {
      if ((it->second.type == stringType_t))
        {
          //delete it->second.data.pstrValue;
        }
      else if (it->second.type == rawType_t)
        {
          //delete it->second.data.rawData;

        }
    }
  //if (!m_tempAttr.empty()) 
  m_tempAttr.clear ();
}

void
PktType::ClearPersistantData ()
{
  std::map < dataNameType_t, dataTypeValue_t >::iterator it;
  for (it = m_namedAttr.begin (); it != m_namedAttr.end (); it++)
    {
      if ((it->second.type == stringType_t))
        {
          //delete it->second.data.pstrValue;
        }
      else if (it->second.type == rawType_t)
        {
          //delete it->second.data.rawData;

        }
    }
  //if (!m_namedAttr.empty()) 
  m_namedAttr.clear ();
}

void
PktType::ClearData ()
{
  ClearTempData ();
  ClearPersistantData ();
}

AcclContentName
PktType::GetAcclName () const
{
  return GetName ().GetFullName ();
}

std::ostream & operator << (std::ostream & os, const PktType & i)
{

  dataNameType_t name = (i.GetName ()).GetFullName ();

  os << "<FATEPKT purpose=\"" << i.GetPacketPurpose () << "\" name=\"" << name << "\">\n";


  for (std::map < dataNameType_t,
       PktType::dataTypeValue_t >::const_iterator cit = i.m_namedAttr.begin (); cit != i.m_namedAttr.end (); cit++)
    {

      uint16_t type = GlobalAttributeNameRegistration.Register (cit->first);

      os << "  <Attribute name=\"" << cit->first << "\"" << " nameType=\"" <<
        type << "\" dataType=\"" << cit->second.type << "\" data=\"";
      //os << "  <Attribute> name=\""  << cit->first << "\"" << " dataType=\"" << cit->second.type <<"\" data=\"" ;
      dataNameType_t temp;

      switch (cit->second.type)
        {
          case PktType::stringType_t:
            temp = *(cit->second.data.pstrValue);
            os << temp;
            break;
          case PktType::rawType_t:
            for (unsigned int i = 0; i < cit->second.rawDataSize; i++)
              {
                std::ios::fmtflags saveFlags = os.flags ();
                os << std::setw (2) << std::setfill ('0') << std::hex << static_cast < int >(cit->second.data.rawData[i]);

                os.flags (saveFlags);
              }
            break;
          case PktType::u64endianType_t:
            os << cit->second.data.u64Value;
            break;
          case PktType::i64endianType_t:
            os << cit->second.data.i64Value;
            break;
          case PktType::dblType_t:
            {
              std::ios::fmtflags saveFlags = os.flags ();
              os.unsetf (std::ios::floatfield); // floatfield not set
              os.precision (15);
              os.setf (std::ios::floatfield);

              os << cit->second.data.dblValue;
              os.flags (saveFlags);
            }
            break;
          default:
            assert (0);         //no type defined
            break;

        }
      os << "\"";
      os << " />\n";
      //os << "  </Attribute>\n";
    }

  for (std::map < dataNameType_t,
       PktType::dataTypeValue_t >::const_iterator cit = i.m_tempAttr.begin (); cit != i.m_tempAttr.end (); cit++)
    {
      uint16_t type = GlobalAttributeNameRegistration.Register (cit->first);

      os << "  <TempAttribute name=\"" << cit->first << "\"" << " nameType=\""
        << type << "\" dataType=\"" << cit->second.type << "\" data=\"";
      //os << "  <TempAttribute> name=\""  << cit->first << "\"" << " dataType=\"" << cit->second.type <<"\" data=\"" ;
      dataNameType_t temp;

      switch (cit->second.type)
        {
          case PktType::stringType_t:
            temp = *(cit->second.data.pstrValue);
            os << temp;
            break;
          case PktType::rawType_t:
            {
            std::ios::fmtflags saveFlags = os.flags ();
            for (unsigned int i = 0; i < cit->second.rawDataSize; i++)
              {
                os << std::setw (2) << std::setfill ('0') << std::hex << static_cast < int >(cit->second.data.rawData[i]);
              }
              os.flags (saveFlags);
            }
            break;
          case PktType::u64endianType_t:
            os << cit->second.data.u64Value;
            break;
          case PktType::i64endianType_t:
            os << cit->second.data.i64Value;
            break;
          case PktType::dblType_t:
            os << cit->second.data.dblValue;
            break;
          default:
            assert (0);         //no type defined
            break;

        }

      os << "\"";
      //os << " </TempAttribute>\n";
      os << " />\n";
    }

  os << "</FATEPKT>\n";

  return os;
}

//{ i.Print(os); return os; }

std::istream & operator >> (std::istream & iss, PktType & i)
{
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load (iss);
  if (!result)
    {
      //bad parse, error
      std::cerr << "Bad FATE Pkt parse:" << result.description () << std::endl;
      return iss;
    }

  i.ClearData ();

  pugi::xml_node pktXml = doc.child ("FATEPKT");


  i.m_purpose = static_cast < PktType::PacketPurposeTypes_t > (pktXml.attribute ("purpose").as_uint (0));

  std::string name = pktXml.attribute ("name").as_string ("");
  ContentName cName;

  cName.SetFullName (name);
  i.SetName (cName);

  for (pugi::xml_node attrXml = pktXml.child ("Attribute"); attrXml; attrXml = attrXml.next_sibling ("Attribute"))
    {

      PktType::dataTypeValue_t attribData;
      std::string attribName = attrXml.attribute ("name").as_string ("");
      PktType::dataPktAttributeType_t attribType =
        static_cast < PktType::dataPktAttributeType_t > (attrXml.attribute ("dataType").as_uint (0));

      attribData.type = attribType;
      std::string tempBuffer;
      uint8_t *byteBuffer;

      switch (attribType)
        {
          case PktType::stringType_t:
            attribData.data.pstrValue = new dataNameType_t (attrXml.attribute ("data").as_string (""));
            break;
          case PktType::rawType_t:
            tempBuffer = (attrXml.attribute ("data").as_string (""));
            byteBuffer = new uint8_t[tempBuffer.length () / 2];
            attribData.rawDataSize = tempBuffer.length () / 2;
            attribData.data.rawData = byteBuffer;

            for (unsigned int i = 0; i < tempBuffer.length (); i += 2)
              {
                byteBuffer[i / 2] = (strtoul (tempBuffer.substr (i, 2).c_str (), NULL, 16));
              }
            break;
          case PktType::u64endianType_t:
            attribData.data.u64Value = attrXml.attribute ("data").as_ullong (0);
            break;
          case PktType::i64endianType_t:
            attribData.data.i64Value = attrXml.attribute ("data").as_llong (0);
            break;
          case PktType::dblType_t:
            attribData.data.dblValue = attrXml.attribute ("data").as_double (0.0);
            break;
          default:
            assert (0);         //no type defined
            break;

        }
      i.m_namedAttr[attribName] = attribData;
    }

  for (pugi::xml_node attrXml = pktXml.child ("TempAttribute"); attrXml; attrXml = attrXml.next_sibling ("TempAttribute"))
    {
      PktType::dataTypeValue_t attribData;
      std::string attribName = attrXml.attribute ("name").as_string ("");
      PktType::dataPktAttributeType_t attribType =
        static_cast < PktType::dataPktAttributeType_t > (attrXml.attribute ("dataType").as_uint (0));
      attribData.type = attribType;
      std::string tempBuffer;
      uint8_t *byteBuffer;

      switch (attribType)
        {
          case PktType::stringType_t:
            attribData.data.pstrValue = new dataNameType_t (attrXml.attribute ("data").as_string (""));
            break;
          case PktType::rawType_t:
            tempBuffer = (attrXml.attribute ("data").as_string (""));
            byteBuffer = new uint8_t[tempBuffer.length () / 2];
            attribData.rawDataSize = tempBuffer.length () / 2;
            attribData.data.rawData = byteBuffer;

            for (unsigned int i = 0; i < tempBuffer.length (); i += 2)
              {
                byteBuffer[i / 2] = (strtoul (tempBuffer.substr (i, 2).c_str (), NULL, 16));
              }
            break;
          case PktType::u64endianType_t:
            attribData.data.u64Value = attrXml.attribute ("data").as_ullong (0);
            break;
          case PktType::i64endianType_t:
            attribData.data.i64Value = attrXml.attribute ("data").as_llong (0);
            break;
          case PktType::dblType_t:
            attribData.data.dblValue = attrXml.attribute ("data").as_double (0.0);
            break;
          default:
            assert (0);         //no type defined
            break;

        }

      i.m_tempAttr[attribName] = attribData;
    }


  return iss;
}
