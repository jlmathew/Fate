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
#ifndef ICNName_H_
#define ICNName_H_

#include <list>
#include <utility>
#include <iostream>
#include <algorithm>

#include <iostream>
#include <ostream>
#include <sstream>

#include <cstdlib>
#include <stdint.h>
//#include "ContentName.h"

//This template is based upon some type of character/string/wide character class

template < class T > class IcnName;
#include "ContentName.h"

template < class T > std::ostream & operator << (std::ostream &, const IcnName < T > &);

template < class T > std::istream & operator >> (std::istream &, IcnName < T > &);


//JLM FIXME TODO helpful to add an 'update if not exist' parameter
template < class T > class IcnName      // : public ContentName
{
public:
  IcnName ()
  {
    SetPathDelim ("/");
    SetAttributeDelim ("=");
  }
  friend std::ostream & operator << <> (std::ostream & os, const IcnName < T > &i);
//{ i.Print(os); return os; }

  friend
    std::istream &
  operator >> <> (std::istream & iss, IcnName < T > &i);

  virtual ~ IcnName ()
  {
    m_nameList.erase (m_nameList.begin (), m_nameList.end ());
    m_optionList.erase (m_optionList.begin (), m_optionList.end ());
    //m_optionList.
  }

  IcnName (const IcnName & otherName)
  {
    SetPathDelim (otherName.GetPathDelim ());
    SetAttributeDelim (otherName.GetAttributeDelim ());
    //SetFullPathName(otherName.GetFullPathName());
    m_nameList.assign (otherName.m_nameList.begin (), otherName.m_nameList.end ());
    m_optionList.assign (otherName.m_optionList.begin (), otherName.m_optionList.end ());
    if (m_optionList.size() > 2) {
        std::cout << "bad\n";
    }
  }

  void
  SetUniqAttribute (const T & key, uint64_t value)
  {
    T dataValue;

    std::ostringstream os;
    os << value;
    dataValue = os.str ();
    SetUniqAttribute (key, dataValue);
  }

  bool
  GetUniqAttribute (const T & key, uint64_t & value) const
  {
    T strValue;

    bool result = GetUniqAttribute (key, strValue);
    std::istringstream
    sin (strValue);
    sin >>
      value;

    return
      result;
  }

  bool
  GetUniqAttribute (const T & key, T & value) const
  {
    return GetNthAttribute (key, value, 1);

  }


  void
  SetUniqAttribute (const T & key, const T & value)
  {
    //if it exists, write over it
    //if (AttributeExist(key)) { RemoveUniqAttribute(key); }
    RemoveUniqAttribute (key);
    //append attributes at end
    m_optionList.push_back (std::make_pair (key, value));
    if (m_optionList.size() > 2) {
        std::cout << "bad2\n";
    }

  }

bool
  PathMatch(const IcnName<T> &name)
  {

    return name.GetPath() == GetPath();
  }


  bool
  PartialAttributeMatch(const T &name, const std::list< std::pair< T, T > > &match)
  {
    if (name != GetPathName())
      return false;

    if (match.size() == 0) 
      return true;

    const_it itMatch = match.begin();
    bool ret = false;

    for(; itMatch != match.end(); itMatch++) {
      const_it it = m_optionList.begin();
      for(; it != m_optionList.end(); it++) {
        if (it->first == itMatch->first) {
          if (it->second == itMatch->second) {
            ret = true;
          }
        }
      }
      //xth match failed, exit early
      if (!ret) {
        return ret;
      }
    }
    return ret;
  }

  bool
  PartialAttributeMatch(const IcnName<T> &minMatchName)
  {
     T name = minMatchName.GetPathName();
     std::list< std::pair< T, T > > options = minMatchName.m_optionList;
     return PartialAttributeMatch(name, options);
  }

    bool
  PartialAttributeMatch(const T &minMatchName)
  {
     IcnName<T> name;
     name.SetFullName(minMatchName);
     return PartialAttributeMatch(name);
  }

  void
  RemoveUniqAttribute (const T & key, const T & value)
  {
    m_optionList.remove (make_pair (key, value));
  }

  void
  RemoveUniqAttribute (const T & key)
  {
    iter
      it = findAttribute (key, 1);

    if (it != m_optionList.end ())
      {
        m_optionList.erase (it);
      }
  }

  //void RemoveAllAttribute(const dataNameType_t &key, const dataNameType_t &value);
  bool
  AttributeExist (const T & key) const
  {
    T value;
    return GetNthAttribute (key, value, 1);

  }




  void
  SetPathName (const T & name)
  {
    m_nameList.erase (m_nameList.begin (), m_nameList.end ());
    unsigned int index = 0;
    unsigned int index2 = 0;

    while (index < name.size ())
      {
        index = name.find (m_pathDelimiter, index2);
        //avoid empty delimiters
        if (index != index2)
          {
            m_nameList.push_back (name.substr (index2, index - index2));
          }
        index2 = index + m_pathDelimiter.size ();
      }

  }

  void
  SetFullName (const T & name)
  {
    //copy options to a temp variable
    T
      options;
    unsigned int firstOpt = name.find (m_optionDelimiter);
    unsigned int lastPath = 0;
m_optionList.clear(); //erase, as we insert new ones

    if (firstOpt <= name.size ())
      {                         //string::npos, but ont very generic
        lastPath = name.rfind (m_pathDelimiter, firstOpt);
        SetPathName (name.substr (0, lastPath));
//std::cout << ":: " << GetPathName() << std::endl;
        //options
        lastPath += 1;
        while (lastPath < name.size ())
          {

            firstOpt = name.find (m_pathDelimiter, lastPath);
            if (firstOpt > name.size ())
              {
                firstOpt = name.size ();
              }
            //option in string between [lastPath, firstOpt]
            //option in string between [lastPath, delimPos, firstOpt]
            int
              delimPos = name.find (m_optionDelimiter, lastPath);
            T
              option = name.substr (lastPath, delimPos - lastPath);
            T
              value = name.substr (delimPos + m_optionDelimiter.size (),
                                   firstOpt - delimPos - 1);


            m_optionList.push_back (make_pair (option, value));

    if (m_optionList.size() > 2) {
        std::cout << "bad3\n";
    }


            lastPath = firstOpt + m_optionDelimiter.size ();

          }
      }
    else
      {                         // no options
        SetPathName (name);
      }

  }


  uint16_t
  GetPathCount () const
  {
    return m_nameList.size ();
  }

  uint16_t
  GetAttributeCount (const T & key = "") const
  {
    return m_optionList.size ();
  }


  //bool GetPathHierarchyElement(uint16_t hierarchyElementNum);

  T
  GetPathDelim () const
  {
    return m_pathDelimiter;
  }


  T
  GetAttributeDelim () const
  {
    return m_optionDelimiter;
  }


  void
  SetPathDelim (const T & pathDelim)
  {
    m_pathDelimiter = pathDelim;
  }


  void
  SetAttributeDelim (const T & optionDelim)
  {
    m_optionDelimiter = optionDelim;
  }



  //void SetNthAttribute(const dataNameType_t &key, const dataNameType_t &value, uint16_t count)

  bool
  GetNthAttribute (const T & key, T & value, uint16_t count) const
  {
    const_it it;

    it = findAttribute (key, count);
    if (it == m_optionList.end ())
      {
        return false;
      }
    value = it->second;

    return true;

  }

  //bool RemoveNthAttribute(const dataNameType_t &key, dataNameType_t &value, uint16_t count) const

  /*
     void SetChunk(uint64_t chunk);
     void SetSerialNum(uint64_t serialNum);
     void SetDigest(const uint8_t *digest, uint16_t digestLen);
     void SetOriginDate(uint64_t dateTimeOrigin);
     void SetOriginDate(const struct tm dateTimeOrigin);
   */

  T
  GetPath () const
  {
    T path;
    typename std::list < T >::const_iterator it;
    typename std::list < T >::const_iterator itEnd;
    it = m_nameList.begin ();
    itEnd = m_nameList.end ();
    if (m_nameList.size() == 1)
      {
        path.append (m_pathDelimiter);
        path.append(*it);
        return path;
      }
    //const_it it = m_nameList.begin();
    uint32_t count=m_nameList.size()-1;
    //while (it != itEnd)
    while (count)
      {
        path.append (m_pathDelimiter);
        path.append (*it);
        it++;
        --count;
      }


    return path;

  }


T
GetAttributes () const
{
    T options;
    std::list < std::pair < T , T > > sortOptionList = m_optionList; 
    sortOptionList.sort();
    const_it it = sortOptionList.begin ();
    while (it != sortOptionList.end ())
      {
        options.append (m_pathDelimiter);
        options.append (it->first);
        options.append (m_optionDelimiter);
        options.append (it->second);
        it++;
      }

    return options;
}

T
GetNthPathSegment (uint32_t segment);

T
GetName() const
{
   T name;
   auto it=m_nameList.rbegin();
   name = *it;
   return name;
}

T
GetPathName () const
{
    T path;
    typename std::list < T >::const_iterator it;
    it = m_nameList.begin ();
    //const_it it = m_nameList.begin();
    while (it != m_nameList.end ())
      {
        path.append (m_pathDelimiter);
        path.append (*it);
        it++;
      }

      return path;

}

T
GetFullName () const
{
    T fullName = GetPathName ();
    fullName.  append (GetAttributes ());
    return fullName;
}


/*
    bool GetChunk(uint64_t &chunk) const;
    bool GetSerialNum(uint64_t &serialNum) const;
    bool GetDigest(uint8_t *digest, uint16_t &digestLen) const ;
    bool GetOriginDate(uint64_t &dateTimeOrigin) const;
   bool GetOriginDate(struct tm &dateTimeOrigin) const ;
   */
  void
  Print (std::ostream & os) const
  {
    os << GetFullName ();
  }

  IcnName < T > *
  Implement (std::istream & is)
  {
    T fullName;

    fullName << is;
    SetFullName (fullName);
  }




  void
  operator = (const IcnName < T > &other)
  {
    if (this == &other)
      return;                   // Gracefully handle self assignment
    m_nameList.assign (other.m_nameList.begin (), other.m_nameList.end ());
    m_optionList.assign (other.m_optionList.begin (), other.m_optionList.end ());
    m_pathDelimiter = other.GetPathDelim ();
    m_optionDelimiter = other.GetAttributeDelim ();
    //copy other to here;
    if (m_optionList.size() > 2) {
        std::cout << "bad4\n";
    }
  }

  bool
  operator < (const IcnName < T > &other) const
  {
    return (GetFullName () < other.GetFullName ());
  }

  bool
  operator == (const IcnName < T > &other) const
  {
    return (GetFullName () == other.GetFullName ());
  }

private:
  typedef typename std::list < std::pair < T, T > >::const_iterator const_it;
  typedef typename std::list < std::pair < T, T > >::iterator iter;

  //does this work for iteration?

  const_it
  findAttribute (const T & name, uint16_t matchNum) const
  {
    const_it
      it = m_optionList.begin ();
    for (; it != m_optionList.end (); it++)
      {
        if (it->first == name)
          {
            matchNum--;
            if (!matchNum)
              {
                return it;
              }
          }

      }
    return
      it;
  }

  iter
  findAttribute (const T & name, uint16_t matchNum)
  {
    iter it = m_optionList.begin ();

    for (; it != m_optionList.end (); it++)
      {
        if (it->first == name)
          {
            matchNum--;
            if (!matchNum)
              {
                return it;
              }
          }

      }
    return it;
  }

  T m_pathDelimiter, m_optionDelimiter;

  std::list < T > m_nameList;
  std::list < std::pair < T, T > >m_optionList;


};

template < class T > std::ostream & operator << (std::ostream & os, const IcnName < T > &i)
{
  return os;
}

//{ i.Print(os); return os; }

template < class T > std::istream & operator >> (std::istream & iss, IcnName < T > &i)
{
  return iss;
}

//{ i.Implement(iss); return iss; }



/*
template <class dataNameType_t>
    void IcnName<dataNameType_t>::SetChunk(uint64_t chunk){}
template <class dataNameType_t>
    void IcnName<dataNameType_t>::SetSerialNum(uint64_t serialNum){}
template <class dataNameType_t>
    void IcnName<dataNameType_t>::SetDelimiter(const dataNameType_t &delim){}
template <class dataNameType_t>
    void IcnName<dataNameType_t>::SetDigest(const uint8_t *digest, uint16_t digestLen){}
template <class dataNameType_t>
    void IcnName<dataNameType_t>::SetOriginDate(uint64_t dateTimeOrigin){}
template <class dataNameType_t>
    void IcnName<dataNameType_t>::SetOriginDate(const struct tm dateTimeOrigin) {}
*/



/*     
template <class dataNameType_t>
    bool IcnName<dataNameType_t>::GetChunk(uint64_t &chunk) const{
    }

template <class dataNameType_t>
    bool IcnName<dataNameType_t>::GetSerialNum(uint64_t &serialNum) const{}
template <class dataNameType_t>
    bool IcnName<dataNameType_t>::GetDigest(uint8_t *digest, uint16_t &digestLen) const {}
template <class dataNameType_t>
    bool IcnName<dataNameType_t>::GetOriginDate(uint64_t &dateTimeOrigin) const{}
template <class dataNameType_t>
    bool IcnName<dataNameType_t>::GetOriginDate(struct tm &dateTimeOrigin) const {} 
 */



#endif
