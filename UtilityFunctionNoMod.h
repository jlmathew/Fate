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

//LRU specific data
class LruData
{                               //: public UtilitySpecificData {
public:
  timer_struct_t m_objectTimestamp;
};

class LruDataSpatial : public LruData
{                               //: public UtilitySpecificData {
public:
  uint64_t m_position;  //optional
};

class LfuData
{                               //: public UtilitySpecificData {
public:
  uint64_t m_weight;  //optional
};

class LfuDataSpatial : public LfuData
{                               //: public UtilitySpecificData {
public:
  uint64_t m_position;  //optional
};



class UtilityLruSp : public UtilityHandlerBase
{
public:
  UtilityLruSp ();
  UtilityLruSp (ConfigWrapper & xmlConfig);
  ~UtilityLruSp ();
 //
  //fixed increment returns an equal value, based upon how many are 
  //lower/higher than itself.
  //e.g. if 10 items are inserted, and 5 are below it, and 5 are
  //higher, it will have a value of 0.5, regardless of timestamp.

  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & data);   //Rx

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("LRU_Sp");
      return idName;
  }

  //call Compute before Value, in case need to adjust values in relation to itself
  virtual void Compute (const AcclContentName & name);
  virtual void Compute ();
  //virtual void Print(std::ostream & os, const AcclContentName & name, double &value)  const;
  virtual bool OnInit (UtilityExternalModule *);

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void) const;

  virtual void DoDelete (const AcclContentName & name);

  virtual bool SelfTest(void); 

protected:
  bool m_timeChange;
  class StorageClass < AcclContentName, LruDataSpatial > *m_scratchpad;
};


class UtilityLru : public UtilityHandlerBase
{
public:
  UtilityLru ();
  UtilityLru (ConfigWrapper & xmlConfig);
  ~UtilityLru ();
  //<Utility name="LRU"  instance="1" scaling="relative_age" | "fixed_increment" (default) />
  //both 'scaling' sets the oldest value to zero, and the newest to '1'.
  //but relative_age returns a value in ratio to that.  e.g. if the
  //oldest is 10 and the youngest is 20 timestamp, a value of 12
  //returns (12-10)/(20-10) = 0.2
  //

  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & data);   //Rx
  //virtual void Print(std::ostream & os, const AcclContentName & name, double &value)  const;

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("LRU");
      return idName;
  }

  //call Compute before Value, in case need to adjust values in relation to itself
  virtual void Compute (const AcclContentName & name);
  virtual void Compute ();

  virtual bool OnInit (UtilityExternalModule *);

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void) const;

  virtual void DoDelete (const AcclContentName & name);

  virtual bool SelfTest(void); 

protected:
  timer_struct_t m_oldestTime, m_newestTime;
  bool m_useNowAsTimeLimit;
  bool m_timeChange;
  class StorageClass < AcclContentName, LruData > *m_scratchpad;
};

class UtilityLfu:public UtilityHandlerBase
{
public:
  UtilityLfu ();
  UtilityLfu (ConfigWrapper & xmlConfig);
  ~UtilityLfu ();
  //<Utility name="LFU"  instance="1" />

  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & data);   //Rx
  //virtual void Print(std::ostream & os, const AcclContentName & name, double &value)  const;

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("LFU");
      return idName;
  }

  //call Compute before Value, in case need to adjust values in relation to itself
  virtual void Compute (const AcclContentName & name);
  virtual void Compute ();

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void) const;

  virtual void DoDelete (const AcclContentName & name);

  virtual bool SelfTest(void);

protected:
  class StorageClass < AcclContentName, LfuData > *m_scratchpad;
  bool m_spaceChange;
  LfuData m_lfu, m_mfu; //least and most recent
};

class UtilityLfuSp:public UtilityHandlerBase
{
public:
  UtilityLfuSp ();
  UtilityLfuSp (ConfigWrapper & xmlConfig);
  ~UtilityLfuSp ();
  //<Utility name="LFU"  instance="1" />

  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & data);   //Rx

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("LFU_Sp");
      return idName;
  }

  //call Compute before Value, in case need to adjust values in relation to itself
  virtual void Compute (const AcclContentName & name);
  virtual void Compute ();
  //virtual void Print(std::ostream & os, const AcclContentName & name, double &value)  const;

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void) const;

  virtual void DoDelete (const AcclContentName & name);

  virtual bool SelfTest(void);

protected:
  class StorageClass < AcclContentName, LfuDataSpatial > *m_scratchpad;
  bool m_spaceChange;
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

//add LFU

//add fifo


//Sensor based (new file)
//add positional (gps) value (source, current, destination)
//add power (battery) value
//Congestion (forwarding), network speed, reliability (errorness), phy security properties (fiber, copper, air)



#endif