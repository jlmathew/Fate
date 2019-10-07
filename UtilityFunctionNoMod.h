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
#ifndef UTILITYFUNCTIONSNOMOD_H_
#define UTILITYFUNCTIONSNOMOD_H_
#include <algorithm>
#include <cstdlib>
#include <stdint.h>
#include <ostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <list>
#include <exception>
#include <vector>
#include <map>
#include "UtilityHandlerBase.h"
#include "ContentName.h"
#include "IcnName.h"
#include "UtilityConfigBase.h"
#include "BaseStorage.h"
#include "PacketTypeBase.h"
#include "NormalizeValue.h"
#include <regex>
//LRU specific data
class LruData
{ //: public UtilitySpecificData {
public:
  timer_struct_t m_objectTimestamp;
  LruData() {
    m_objectTimestamp.tv_sec = 0;
    m_objectTimestamp.tv_nsec = 0;
  }
  LruData(const LruData &other) {
    m_objectTimestamp.tv_sec = other.m_objectTimestamp.tv_sec;
    m_objectTimestamp.tv_nsec = other.m_objectTimestamp.tv_nsec;
  }
  LruData(const timer_struct_t &other) {
    m_objectTimestamp.tv_sec = other.tv_sec;
    m_objectTimestamp.tv_nsec = other.tv_nsec;
  }
  LruData & operator=(const LruData &other) {
    if (this != &other) {
      m_objectTimestamp.tv_sec = other.m_objectTimestamp.tv_sec;
      m_objectTimestamp.tv_nsec = other.m_objectTimestamp.tv_nsec;
    }
    return *this;
  }
  LruData(long double val) {
    uint64_t sec = (uint64_t) val;
    uint64_t nsec = (val-sec) * 1000000000;
    m_objectTimestamp.tv_sec = sec;
    m_objectTimestamp.tv_nsec = nsec;
  }
  operator long double() const
  {
    long double ret = m_objectTimestamp.tv_sec + m_objectTimestamp.tv_nsec/1000000000.0; // magic happens here
    return ret;
  }
  virtual ~LruData() {}
  static signed int TimeCompare (const LruData & a, const LruData &b) {
    if (a==b) {
      return 0; // -1, a < b, 0 a== b, 1 a > b
    }
    if (a<b) {
      return -1;
    }
    return 1;
  }
  signed int TimeCompare(const LruData &rhs) {
    return TimeCompare(*this, rhs);
  }

  LruData  operator- (const LruData &rhs) const
  {
    LruData retVal;
    retVal.m_objectTimestamp.tv_sec =m_objectTimestamp.tv_sec - rhs.m_objectTimestamp.tv_sec  ;
    if (m_objectTimestamp.tv_nsec < rhs.m_objectTimestamp.tv_nsec)  {
      retVal.m_objectTimestamp.tv_sec--;
      retVal.m_objectTimestamp.tv_nsec =m_objectTimestamp.tv_nsec + 1000000000- rhs.m_objectTimestamp.tv_nsec  ;
    } else {
      retVal.m_objectTimestamp.tv_nsec =m_objectTimestamp.tv_nsec - rhs.m_objectTimestamp.tv_nsec  ;
    }
    return retVal;
  }
  bool
  operator< (const LruData & rhs) const
  {
    if (m_objectTimestamp.tv_sec == rhs.m_objectTimestamp.tv_sec)
    {
      return m_objectTimestamp.tv_nsec < rhs.m_objectTimestamp.tv_nsec;
    }
    return (m_objectTimestamp.tv_sec < rhs.m_objectTimestamp.tv_sec);
  }
  bool
  operator> (const LruData & rhs) const
  {
    return !(*this<rhs);
  }

  bool
  operator== (const LruData & rhs) const
  {
    if (m_objectTimestamp.tv_sec == rhs.m_objectTimestamp.tv_sec)
    {
      return m_objectTimestamp.tv_nsec == rhs.m_objectTimestamp.tv_nsec;
    }
    return false;
  }
};

typedef uint64_t LfuData;





class UtilityLru : public UtilityHandlerBase
{
public:
  UtilityLru ();
  UtilityLru (ConfigWrapper & xmlConfig);
  ~UtilityLru ();

  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & data);   //Rx
  //virtual void Print(std::ostream & os, const AcclContentName & name, double &value)  const;

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("LRU");
    return idName;
  }

  //call Compute before Value, in case need to adjust values in relation to itself
  //virtual void Compute (const AcclContentName & name);
  //virtual void Compute ();

  virtual bool OnInit (UtilityExternalModule *);

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void) const;

  virtual void DoDelete (const AcclContentName & name);

  virtual bool SelfTest(void);

protected:
  class StorageClass < AcclContentName, LruData > *m_scratchpad;
  class Normalize<LruData> *m_normalize;
  bool m_useNowAsTimeLimit;

};

class UtilityLfu:public UtilityHandlerBase
{
public:
  UtilityLfu ();
  UtilityLfu (ConfigWrapper & xmlConfig);
  ~UtilityLfu ();

  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & data);   //Rx
  //virtual void Print(std::ostream & os, const AcclContentName & name, double &value)  const;

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("LFU");
    return idName;
  }

  //call Compute before Value, in case need to adjust values in relation to itself
  //virtual void Compute (const AcclContentName & name);
  //virtual void Compute ();

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void) const;

  virtual void DoDelete (const AcclContentName & name);

  virtual bool SelfTest(void);

protected:
  class StorageClass < AcclContentName, LfuData > *m_scratchpad;
  class Normalize<LfuData> *m_normalize;
};


class UtilityNameAttrMatch:public UtilityHandlerBase
{
public:
  UtilityNameAttrMatch ();
  UtilityNameAttrMatch (ConfigWrapper & xmlConfig);
  ~UtilityNameAttrMatch ();
  virtual void Config (ConfigWrapper & xmlConfig);

  static const dataNameType_t &
  IdName ()
  {
    static const dataNameType_t idName ("NAME_ATTR_MATCH");
    return idName;
  }
  virtual void OnPktIngress (PktType & data);   //Rx

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void);

  virtual bool SelfTest(void);
private:
  //FIXME TODO should have a matching range
  uint64_t m_modValue;
  uint64_t m_modMatchLow;
  uint64_t m_modMatchHigh;
  std::string m_matchType;
  class StorageClass < AcclContentName, bool > *m_scratchpad;
  std::string m_optionMatch;
};

//hash of name
class UtilityHashModulus:public UtilityHandlerBase
{
public:
  UtilityHashModulus ();
  UtilityHashModulus (ConfigWrapper & xmlConfig);
  ~UtilityHashModulus ();
  virtual void Config (ConfigWrapper & xmlConfig);

  static const dataNameType_t &
  IdName ()
  {
    static const dataNameType_t idName ("HASH_MODULUS");
    return idName;
  }

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void);

protected:
  uint64_t makeHash (const char *hash, uint32_t hashLength) const;

private:
  //FIXME TODO should have a matching range
  uint64_t m_modValue;
  uint64_t m_modMatchLow;
  uint64_t m_modMatchHigh;

  std::string m_hashType;
  std::string m_optionMatch;
};


//Random
class UtilityRnd:public UtilityHandlerBase
{
public:
  UtilityRnd (ConfigWrapper & xmlConfig);
  ~UtilityRnd ();

  enum randType_t
  {
    invalid = 0,
    rndOnce,                    //calculate randomness at start
    rndAlways                   //every time Value() is called, send rnd value
  };

  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & data);   //Rx


  static const dataNameType_t IdName ()
  {
    static const dataNameType_t idName ("RND");
    return idName;
  };

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void);

  virtual void DoDelete (const AcclContentName & name);

private:
  randType_t m_rndType;
  //std::map<  AcclContentName, double > m_scratchpad;
  class StorageClass < dataNameType_t, double >*m_scratchpad;
  std::map < AcclContentName, double >::iterator it;
  std::map < AcclContentName, double >::const_iterator cit;
};



//constant
class UtilityConst : public UtilityHandlerBase
{
public:
  UtilityConst (ConfigWrapper & config);
  ~UtilityConst ();
  virtual void Config (ConfigWrapper & config);

  virtual void Print (std::ostream & os, bool endParen = true) const;
  virtual void Print (std::ostream & os, const AcclContentName & name, double &value) const;
  static const dataNameType_t & IdName ()
  {
    static const dataNameType_t idName ("CONST");
    return idName;
  }

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void);
};

class UtilityEgressCount : public UtilityHandlerBase
{
public:
  UtilityEgressCount();
  UtilityEgressCount(ConfigWrapper &config);
  ~UtilityEgressCount();
  virtual void OnPktEgress (PktType & pkt, const PktTxStatus & status);

  virtual void DoDelete (const AcclContentName & name);

  virtual void Config (ConfigWrapper & config);

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("EgressCnt");
    return idName;
  }

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void) const;



private:
  class StorageClass < dataNameType_t, uint32_t >*m_scratchpad;

};


//Used for already evaluated parameters
class UtilityNormValuationEval : public UtilityHandlerBase
{
public:
  UtilityNormValuationEval();
  UtilityNormValuationEval(ConfigWrapper &config);
  ~UtilityNormValuationEval();
  virtual void OnPktIngress (PktType & pkt);

  virtual void DoDelete (const AcclContentName & name);

  virtual void Config (ConfigWrapper & config);

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("NormEval");
    return idName;
  }

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void) const;

private:
  class StorageClass < dataNameType_t, double >*m_scratchpad;
  bool m_temp;

};

//Sensor based (new file)
//add positional (gps) value (source, current, destination)
//add power (battery) value
//Congestion (forwarding), network speed, reliability (errorness), phy security properties (fiber, copper, air)
//this is for fields containing values, e.g. hops=5
class UtilityU64ValuationEval : public UtilityHandlerBase
{
public:
  UtilityU64ValuationEval();
  UtilityU64ValuationEval(ConfigWrapper &config);
  ~UtilityU64ValuationEval();
  virtual void OnPktIngress (PktType & pkt);

  virtual void DoDelete (const AcclContentName & name);

  virtual void Config (ConfigWrapper & config);

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("RawEval");
    return idName;
  }

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void) const;

private:
  class StorageClass < dataNameType_t, uint64_t >*m_scratchpad;
  class Normalize<uint64_t> *m_normalize;
  bool m_temp;

};


//protect for last inserted elements
class UtilityProtLastElement:public UtilityHandlerBase
{
public:
  UtilityProtLastElement (ConfigWrapper & xmlConfig);
  ~UtilityProtLastElement ();


  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & data);   //Rx


  static const dataNameType_t IdName ()
  {
    static const dataNameType_t idName ("PLE");
    return idName;
  };

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void);

  virtual void DoDelete (const AcclContentName & name);

private:
   dataNameType_t m_lastElementSeen;
   bool m_lastElemValid;
};

//////////////
class UtilityLastSeen : public UtilityHandlerBase
{
public:
  UtilityLastSeen ();
  UtilityLastSeen (ConfigWrapper & xmlConfig);
  ~UtilityLastSeen ();

  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & data);   //Rx
  //virtual void Print(std::ostream & os, const AcclContentName & name, double &value)  const;

  virtual void Compute();
  virtual void Compute(const AcclContentName &name);
  
  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("LASTSEEN");
    return idName;
  }

  //call Compute before Value, in case need to adjust values in relation to itself
  //virtual void Compute (const AcclContentName & name);
  //virtual void Compute ();

  virtual bool OnInit (UtilityExternalModule *);

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void) const;

  virtual void DoDelete (const AcclContentName & name);


protected:
  class StorageClass < AcclContentName, LruData > *m_scratchpad;
  class Normalize<LruData> *m_normalize;

};

//TODO FIXME not tested or implemented JLM
class UtilityRegexMatch:public UtilityHandlerBase
{
public:
  UtilityRegexMatch ();
  UtilityRegexMatch (ConfigWrapper & xmlConfig);
  ~UtilityRegexMatch ();
  virtual void Config (ConfigWrapper & xmlConfig);

  static const dataNameType_t &
  IdName ()
  {
    static const dataNameType_t idName ("REGEX_MATCH");
    return idName;
  }
  virtual void OnPktIngress (PktType & data);   //Rx

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void);

  virtual bool SelfTest(void);
private:
  //FIXME TODO should have a matching range
  std::string m_matchingField;
  enum matchingType_t {
     illegal=0,
     baseName,
     specifierName,
     fullName,
     fieldNameTemp,
     filedNamePerm,
     allFieldsTemp,
     allFieldsPerm,
     allFields,

  };

  std::string m_regPattern;
  matchingType_t m_matchTypeEnum;
  std::string m_regMatch;
  std::string m_matchType;
  std::regex m_regexPat;
  class StorageClass < AcclContentName, bool > *m_scratchpad;
  std::string m_optionMatch;
};

/*
#include <list>
class LruDiffData
{
public:

  LruDiffData(uint32_t num, const LruData &firstVal) {
	  m_maxCount=num;
	  m_objectTimestamp.push_back(firstVal);
          
  }
  virtual ~LruDiffData() {}

  double getAverage() const
  {
    LruData fakeNow((uint64_t)(-1));	  
    return getAverage(fakeNow);
  }
  double getAverage(const LruData &now) const
  {
	  double retVal = 0.0;
     auto it1=m_objectTimestamp.begin();
     auto it2=it1++;
     for(; it2 != m_objectTimestamp.end(); ++it2) {
         retVal += *it2-*it1;
	 it1=it2;
     }
     //now for current time
     retVal += now-*it1;
     retVal /= m_objectTimestamp.size();

     return retVal;
  }
  void addTimestamp(const LruData &now)
  {
    m_objectTimestamp.push_back(now);
    if (m_objectTimestamp.size() > m_maxCount) {
      m_objectTimestamp.pop_front();
    }
  }
 
 void Init(uint32_t mxCnt) { m_maxCount = mxCnt; } 
   LruDiffData() {
	  m_maxCount=0;
  } 
 LruDiffData & operator=(const LruDiffData &other) {
    if (this != &other) {
	    //copy other
       m_maxCount=other.m_maxCount;
       m_objectTimestamp.clear();
       m_objectTimestamp.insert (m_objectTimestamp.begin(),other.m_objectTimestamp.begin(),other.m_objectTimestamp.end());
       
    }
    return *this;
  }

  LruDiffData(const LruDiffData &other) {
//copy other
       m_maxCount=other.m_maxCount;
       m_objectTimestamp.insert (m_objectTimestamp.begin(),other.m_objectTimestamp.begin(),other.m_objectTimestamp.end());
  }

  bool
  operator< (const LruDiffData & rhs) const
  {
	return getAverage() < rhs.getAverage();

  }
  bool
  operator> (const LruDiffData & rhs) const
  {
    return !(*this<rhs);
  }

  bool
  operator== (const LruDiffData & rhs) const
  {
	  return getAverage() == rhs.getAverage();
  }

private:
  uint32_t m_maxCount;
	std::list<LruData> m_objectTimestamp;


};

class UtilityLruDiff:public UtilityHandlerBase
{
public:
  UtilityLruDiff ();
  UtilityLruDiff (ConfigWrapper & xmlConfig);
  virtual ~UtilityLruDiff ();

  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & data);   //Rx
  //virtual void Print(std::ostream & os, const AcclContentName & name, double &value)  const;

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("LFUDIFF");
    return idName;
  }
  bool
OnInit (UtilityExternalModule * outsideData);

  //call Compute before Value, in case need to adjust values in relation to itself
  //virtual void Compute (const AcclContentName & name);
  //virtual void Compute ();

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void) const;

  virtual void DoDelete (const AcclContentName & name);

  virtual bool SelfTest(void);

protected:
  class StorageClass < AcclContentName, LruDiffData > *m_scratchpad;
  class Normalize<LruDiffData> *m_normalize;
    uint64_t m_totalHistory;
};
*/

#endif
