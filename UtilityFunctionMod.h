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
#ifndef UTILITYFUNCTIONSMOD_H_
#define UTILITYFUNCTIONSMOD_H_

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
#include "RangeData.h"
#include "NormalizeValue.h"

//TODO JLM absolute and relative lifetime
class UtilityFresh : public UtilityHandlerBase
{
public:
  UtilityFresh (ConfigWrapper & xmlConfig);
  ~UtilityFresh ();
  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & data);    //Rx
virtual void
OnPrePktEgress (PktType & data);


  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void);
  virtual void DoDelete (const AcclContentName & name);

  enum decayRate_t
  {
    flat,
    linear
  };

  struct freshStruct_t
  {
    timer_struct_t pktRx;
    timer_struct_t pktExpire;

  };
  static const dataNameType_t IdName ()
  {
    static const dataNameType_t idName ("FRESH");
      return idName;
  };

private:
  decayRate_t m_decayRate;
  double m_defValue;

  //std::string m_matchingName;
  class StorageClass < AcclContentName, freshStruct_t > *m_scratchpad;
};

//add TTL (countdown), compare by range.   Use double, not uint64_t
//should work as TTL, or move (up/down) to other caches towards source.

class UtilityCnt : public UtilityHandlerBase
{
public:
  UtilityCnt (ConfigWrapper & xmlConfig);
  ~UtilityCnt ();
  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & ata);    //Rx


  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void);
  virtual void DoDelete (const AcclContentName & name);

  static const dataNameType_t IdName ()
  {
    static const dataNameType_t idName ("COUNT");
      return idName;
  };
  enum CountCondition {
     decrement,
     increment,
     none,
     numConditions 
  };
 
private:

  virtual void 
  Condition(uint64_t &count) const;

  uint64_t m_startCount;
  CountCondition m_cntCondition;
  //std::string m_matchingName;
  std::string m_op;
  RangeData<uint64_t> m_validRange;
  std::string m_rangeCompare;
  uint64_t m_lowerBound;
  uint64_t m_upperBound;
  class StorageClass < AcclContentName, bool > *m_scratchpad;
  NormalRanked<uint64_t> *m_normalize;
  
};

//used to trace a route
class UtilityNameChain : public UtilityHandlerBase
{
public:
  UtilityNameChain (ConfigWrapper & xmlConfig);
  ~UtilityNameChain ();
  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & data);    //Rx

  virtual double Value(const AcclContentName &name) const;
  static const dataNameType_t IdName ()
  {
    static const dataNameType_t idName ("NAMECHAIN");
      return idName;
  };
 
private:
   bool m_nameAppendIfnExist;
   bool m_appendFront; //append in front or rear
   bool m_allowNodeNameAppend; //yes/no for certain nodes to append name
   std::string m_partition;
   std::string m_nodeName;
   bool m_eachNodeUnique;
};


//return if chainname matches at front/anywhere/end
class UtilityMatchChainName : public UtilityHandlerBase
{
public:
  UtilityMatchChainName (ConfigWrapper & xmlConfig);
  ~UtilityMatchChainName ();
  virtual void Config (ConfigWrapper & xmlConfig);

  virtual void OnPktIngress (PktType & data);    //Rx

  virtual double Value (const AcclContentName & name) const;

  static const dataNameType_t IdName ()
  {
    static const dataNameType_t idName ("MATCHNAMECHAIN");
      return idName;
  };
 
private:
   bool m_checkOnlyFront; //check in front or rear
   bool m_checkAnywhere; //append anywhere 
   bool m_deleteMatch; //remove match
   std::string m_partition;
  class StorageClass < AcclContentName, bool > *m_scratchpad;
};

//save by incoming interface - pit (interface, mac, ip)

//add condition-config

//Add path direction; pop from list of destinations, return if match first or last destination.

//add based upon successful egress count -- done(EgressCount)?


//add attribute matching (e.g. social grouping) and values (exact and trie based)

//add 'total file' utilities (e.g. based upon LFU of 1st chunk of a file, predict when to purge, and which other chunks

//PREFETCH
//predictive prefetch based upon prior fetches (e.g. chunks or segments)

//manifest based prefetching

#endif
