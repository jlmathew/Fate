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
#include "UtilityFunctionGenerator.h"
#include "UtilityFunctionsAggregate.h"
#include "UtilityFunctionMod.h"
#include "UtilityFunctionNoMod.h"

#include "ForwardBasicManager.h"
#include "SecurityBasicManager.h"
#include "CacheBasicManager.h"
#include "CacheBasicManager2.h"

//For Ns3 Ipv4 packets, conditional compile may be necessary
#ifndef NO_NS3
#include "ns3/ForwardNs3Ipv4Manager.h"
#include "ns3/ForwardNs3Ipv4ManagerF2.h"
#include "ns3/ForwardNs3Ipv4Manager3.h"   //does not need to be in ns3!! MOVE
#include "ns3/ForwardNs3Ipv4ManagerF3.h"   //does not need to be in ns3!! MOVE
#endif

UtilityGenerator::UtilityGenerator ()
{
}

UtilityGenerator::~UtilityGenerator ()
{
}

UtilityHandlerBase *
UtilityGenerator::CreateNewUtility (ConfigWrapper & config)
{
  dataNameType_t name = config.GetAttribute ("name", dataNameType_t (""));

  /*
  //delete me, only for compile
  #include <ostream>
  #include <iostream>
  Normalize<uint32_t> *p = NormalizeGenerator<uint32_t>::CreateNewNormalizeEval(config);
  std::cout << " p is " << p << "\n";
  */

  //start with Aggregate Utilities
  if (!name.compare (UtilitySum::IdName ()))
  {
    return new UtilitySum (config);
  }
  else if (!name.compare (UtilityMult::IdName ()))
  {
    return new UtilityMult (config);
  }
  else if (!name.compare (UtilityMin::IdName ()))
  {
    return new UtilityMin (config);
  }
  else if (!name.compare (UtilityMax::IdName ()))
  {
    return new UtilityMax (config);
  }
  else if (!name.compare (UtilityStepFn::IdName ()))
  {
    return new UtilityStepFn (config);
    // special blocks, eg. Utility block
  }
  else if (!name.compare (UtilityBlock::IdName ()))
  {
    return new UtilityBlock (config);

    //Functional utilities
  }
  else if (!name.compare (UtilityConst::IdName ()))
  {
    return new UtilityConst (config);
  }
  else if (!name.compare (UtilityRnd::IdName ()))
  {
    return new UtilityRnd (config);
  }
  else if (!name.compare (UtilityLru::IdName ()))
  {
    return new UtilityLru (config);
  }
  else if (!name.compare (UtilityHashModulus::IdName ()))
  {
    return new UtilityHashModulus (config);
  }
  else if (!name.compare (UtilityFresh::IdName ()))
  {
    return new UtilityFresh (config);
  }
  else if (!name.compare(UtilityEgressCount::IdName()))
  {
    return new UtilityEgressCount(config);
  }
  else if (!name.compare(UtilityCnt::IdName()))
  {
    return new UtilityCnt(config);
  }
  else if (!name.compare(UtilityNameChain::IdName()))
  {
    return new UtilityNameChain(config);
  }
  else if (!name.compare(UtilityMatchChainName::IdName()))
  {
    return new UtilityMatchChainName(config);
  }
  else if (!name.compare(UtilityLfu::IdName()))
  {
    return new UtilityLfu(config);
  }
  else if (!name.compare(UtilityNameAttrMatch::IdName()))
  {
    return new UtilityNameAttrMatch(config);
  }
  else if (!name.compare(UtilityLruSp::IdName()))
  {
    return new UtilityLruSp(config);
  }
  else if (!name.compare(UtilityLfuSp::IdName()))
  {
    return new UtilityLfuSp(config);
  }
  else if (!name.compare(UtilityU32ValuationEval::IdName()))
  {
    return new UtilityU32ValuationEval(config);
  }

  //no utilities found
  dataNameType_t errorStr = "Invalid Utility:";

  errorStr.append (name);
  errorStr.append (", can't create.");
  throw::std::invalid_argument (errorStr.c_str ());
}

ModuleManager *
ModuleGenerator::CreateNewModule (ConfigWrapper & config)
{
  dataNameType_t name = config.GetAttribute ("moduleName", dataNameType_t (""));

  //std::cout << "creating module:" << name << "\n";
  //start with Aggregate Utilities
  if (!name.compare (CacheBasicManager::IdName ()))
  {
    return new CacheBasicManager (config);
  }
  else if (!name.compare (CacheBasicManager2::IdName ()))
  {
    return new CacheBasicManager2 (config);
  }
  else if (!name.compare (SecurityBasicManager::IdName ()))
  {
    return new SecurityBasicManager (config);
  }
  else if (!name.compare (ForwardBasicManager::IdName ()))
  {
    return new ForwardBasicManager (config);
  }
  else if (!name.compare(NodeManager::IdName()))
  {
    return new NodeManager(config);
  }
//For Ns3 Ipv4 packets, conditional compile may be necessary
#ifndef NO_NS3
  else if (!name.compare (ForwardNs3Ipv4Manager::IdName ()))
  {
    return new ForwardNs3Ipv4Manager (config);
  }
  else if (!name.compare (ForwardNs3Ipv4ManagerF2::IdName ()))
  {
    return new ForwardNs3Ipv4ManagerF2 (config);
  }
  else if (!name.compare (ForwardNs3Ipv4Manager3::IdName ()))
  {
    return new ForwardNs3Ipv4Manager3 (config);
  }
  else if (!name.compare (ForwardNs3Ipv4ManagerF3::IdName ()))
  {
    return new ForwardNs3Ipv4ManagerF3 (config);
  }
#endif
  else
  {
    dataNameType_t errorStr = "Invalid Module:";

    errorStr.append (name);
    errorStr.append (", can't create.");
    throw::std::invalid_argument (errorStr.c_str ());

  }

}

//store generator

StoreManager *
StoreGenerator::CreateNewStore (ConfigWrapper & config)
{
  dataNameType_t name = config.GetAttribute ("storeName", dataNameType_t (""));

  //std::cout << "creating store:" << name << "\n";
  //start with Aggregate Utilities
  if (!name.compare (TypicalCacheStore::IdName ()))
  {
    StoreManager *store = new TypicalCacheStore (config);

    //UniqueStore::RegisterStore(storeName, store);
    return store;
    //return new StoreManager< dataName_t, PktType> (config);
  }
  else
  {
    dataNameType_t errorStr = "Invalid Store:";

    errorStr.append (name);
    errorStr.append (", can't create.");
    throw::std::invalid_argument (errorStr.c_str ());

  }

}


RegisterTemplate < std::string, StoreManager * >UniqueStore::m_allStores;

StoreManager *
UniqueStore::GetGlobalStore (std::string key)
{
  if (m_allStores.Exist (key))
  {
    return (m_allStores.Register (key));
  }
  else
  {
    throw "Not Registered";
  }
}

StoreManager *
UniqueStore::RegisterStore (std::string key, StoreManager * val)
{
  return (m_allStores.Register (key, val));
}
