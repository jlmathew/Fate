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

#include "UtilityFunctionNoMod.h"
#include "RangeData.h"
#include "UtilityFunctionGenerator.h"


//LFU
UtilityLfu::UtilityLfu ()
  :  m_normalize(nullptr)
{
}

UtilityLfu::UtilityLfu (ConfigWrapper & xmlConfig)
  :  m_normalize(nullptr)
{
  Config (xmlConfig);
}

UtilityLfu::~UtilityLfu ()
{
  if (m_scratchpad)
    delete m_scratchpad;
  if (m_normalize)
    delete m_normalize;
}

void
UtilityLfu::Config (ConfigWrapper & xmlConfig)
{
  UtilityHandlerBase::Config(xmlConfig);
  if (!m_useAlias)
  {
    m_name = IdName ();
  }
  ConfigWrapper *normConfig = xmlConfig.GetFirstChildUtility("Normalize");
  if (normConfig->valid()) {
    m_normalize = NormalizeGenerator<LfuData>::CreateNewNormalizeEval(*normConfig);
    std::stringstream ss;
    m_normalize->Print(ss);
    m_name.append(ss.str());
  }
  m_scratchpad = new StorageClass < AcclContentName, LfuData >;
  m_scratchpad->setStorageType (m_storageMethod);
}


void
UtilityLfu::DoDelete (const AcclContentName & name)
{
  LfuData data;
  bool exist = m_scratchpad->ExistData (name, data);
  if (exist) {
    m_scratchpad->EraseData(name);
    if (m_normalize) {
      m_normalize->DeleteValue(data);
    }
  }
}

void
UtilityLfu::OnPktIngress (PktType & data)
{
  if (!((m_createEntryMask | m_updateEntryMask) & data.GetPacketPurpose ()))
  {
    return;
  }



  //uint64_t numEntries = m_scratchpad->size ();

  const AcclContentName name = data.GetAcclName ();

  //bool valid = data.GetName(name).GetFullName();
  LfuData data64;
  data64=0;
  bool exist = m_scratchpad->ExistData (name, data64);
  if (m_createEntryMask & data.GetPacketPurpose ())
  {
    if (exist) {  //update entry
      if (UINTMAX_MAX != data64) {
        m_normalize->DeleteValue(data64);
        data64++;
      m_scratchpad->SetData (name, data64);
      m_normalize->InsertValue(data64);
      }

    } else {  //first entry
      data64 = 1;
      m_scratchpad->SetData (name, data64);
      m_normalize->InsertValue(data64);
    }
    //update for interest pkts, only if the data exists
  }
  else if (exist) //m_updateEntryMask() is true, and the data exists, e.g. Interest packet
  {
    if (UINTMAX_MAX != data64) {
      m_normalize->DeleteValue(data64);
      data64++;
    m_scratchpad->SetData (name, data64);
    m_normalize->InsertValue(data64);
    }
  }

}




bool
UtilityLfu::SelfTest()
{
  bool results = true;
  //fix m_scratchpad->EraseAll();  //JLM FIXME TODO
  assert(m_scratchpad->empty());

  RangeData<double> rng;
  rng.SetRangeCompare(RangeData<double>::LeftRightInclusive);
  //send in 5 'pkts', 1,2,..5 times. Check values
  PktType test[5];
  IcnName < std::string > nameTest;
  nameTest.SetFullName("/sample/test");
  for(unsigned int i=0; i< 5; i++)
  {
    test[i].SetPacketPurpose (PktType::DATAPKT);
    nameTest.SetUniqAttribute("part", i);
    test[i].SetName(nameTest);
    for(unsigned int j=0; j<=i; j++) {
      OnPktIngress(test[i]); //1,2,3,4,5 times
    }
  }
  Compute();
  for(unsigned int i=0; i< 5; i++)
  {
    double val = Value(test[i].GetAcclName());
    rng.SetLowerBound((i+1)*.2-.0000000001);
    rng.SetUpperBound((i+1)*.2+.0000000001);
    results &= ( rng.IsInRange(val) ); //should use a range data item, FIXME TODO JLM
  }
  //Add in 3 interest pkts. check values
  for(unsigned int i=0; i< 3; i++)
  {
    test[i].SetPacketPurpose (PktType::INTERESTPKT);
    for(unsigned int j=0; j<i+2; j++)
    {
      OnPktIngress(test[i]); //3,5,7,4,5 times
    }
  }
  Compute();
  double ans[5]= {.2,.6,1,.4,.6};
  for(unsigned int i=0; i< 5; i++)
  {
    double val = Value(test[i].GetAcclName());
    rng.SetLowerBound(ans[i]-.0000000001);
    rng.SetUpperBound(ans[i]+.0000000001);
    results &= ( rng.IsInRange(val) ); //should use a range data item, FIXME TODO JLM

    //std::cout << m_name << " " << i << ":" << std::setprecision (15) << val << "\n";
  }
  //new interest packet, should have no effect.
  PktType test2;
  IcnName < std::string > nameTest2;
  nameTest2.SetFullName("/sample/test2");
  test2.SetName(nameTest2);
  test2.SetPacketPurpose (PktType::INTERESTPKT);
  OnPktIngress(test2); //should not count without data
  Compute();
  for(unsigned int i=0; i< 5; i++)
  {
    double val = Value(test[i].GetAcclName());
    rng.SetLowerBound(ans[i]-.0000000001);
    rng.SetUpperBound(ans[i]+.0000000001);
    results &= ( rng.IsInRange(val) );

    //std::cout << m_name << " " << i << ":" << std::setprecision (15) << val << "\n";
  }
  double val = Value(test2.GetAcclName());
  rng.SetLowerBound(-.0000000001);
  rng.SetUpperBound(.0000000001);
  results &= ( rng.IsInRange(val) );
  //delete the 7th weighted name.  Add in 1 new pkt. check values
  DoDelete(test[2].GetAcclName());
  Compute();
  ans[2] = 0.0;
  ans[3] = 2/3.0;
  ans[0] = 1/3.0;
  ans[1] = 1;
  ans[4] = 1;
  for(unsigned int i=0; i< 5; i++) //3,5,0,4,5 times
  {
    double val = Value(test[i].GetAcclName());
    rng.SetLowerBound(ans[i]-.0000000001);
    rng.SetUpperBound(ans[i]+.0000000001);
    results &= ( rng.IsInRange(val) ); //should use a range data item, FIXME TODO JLM

    // std::cout << m_name << " " << i << ":" << std::setprecision (15) << val << "\n";
  }

  test2.SetPacketPurpose (PktType::DATAPKT);
  OnPktIngress(test2);
  Compute();
  val = Value(test2.GetAcclName());
  double expVal = 0.2; //1/3.0;
  rng.SetLowerBound(expVal-.0000000001);
  rng.SetUpperBound(expVal+.0000000001);
  results &= ( rng.IsInRange(val) );

  ans[0]=0.6; //2/3.0;
  ans[3]=0.8; //333333333333333;
  //std::cout << val << "\n";
  for(unsigned int i=0; i< 5; i++) //3,5,0,4,5 times
  {
    double val = Value(test[i].GetAcclName());
    rng.SetLowerBound(ans[i]-.0000000001);
    rng.SetUpperBound(ans[i]+.0000000001);
    results &= ( rng.IsInRange(val) ); //should
    //std::cout << m_name << " " << i << ":" << std::setprecision (15) << val << "\n";
  }


  return results;
}

double
UtilityLfu::Value (const AcclContentName & name) const
{
  LfuData retValue;
  bool exist = m_scratchpad->ExistData (name, retValue);
  double retVal=-1;
  if (exist) {
    retVal=  m_normalize->EvaluateValue(retValue);
  } else { //data not found!
    retVal = m_defaultValue;
  }

std::cout << name << " lfu value of [" << retValue << "] is " << retVal << "\n";

  return retVal;
}


uint64_t
UtilityLfu::EstMemoryUsed (void) const
{
  return m_scratchpad->size ();    // * (sizeof(LruData) + 30 ); //EstMemoryUsed();
}


UtilityLru::UtilityLru ()
  : m_normalize(nullptr), m_useNowAsTimeLimit(false)
{
}

UtilityLru::UtilityLru (ConfigWrapper & xmlConfig)
  : m_normalize(nullptr),m_useNowAsTimeLimit(false)
{

  Config (xmlConfig);

}

UtilityLru::~UtilityLru ()
{ if (m_scratchpad)
    delete m_scratchpad;
  if (m_normalize)
    delete m_normalize;

}

void
UtilityLru::Config (ConfigWrapper & xmlConfig)
{
  UtilityHandlerBase::Config(xmlConfig);
  if (!m_useAlias)
  {
    m_name = IdName ();
  }
  ConfigWrapper *normConfig = xmlConfig.GetFirstChildUtility("Normalize");
  if (normConfig->valid()) {
    m_normalize = NormalizeGenerator<LruData>::CreateNewNormalizeEval(*normConfig);
    std::stringstream ss;
    m_normalize->Print(ss);
    m_name.append(ss.str());
  }

  m_useNowAsTimeLimit = xmlConfig.GetAttributeBool ("compareToNow", m_useNowAsTimeLimit);
  //JLM FIXME TODO, put in default
  m_scratchpad = new StorageClass < AcclContentName, LruData >;
  m_scratchpad->setStorageType (m_storageMethod);
}

void
UtilityLru::DoDelete (const AcclContentName & name)
{
  LruData retValue;
  bool exist = m_scratchpad->ExistData (name, retValue);
  if (exist) {
    m_scratchpad->EraseData(name);
    if (m_normalize) {
      m_normalize->DeleteValue(retValue);
    }
  }
}
void
UtilityLru::OnPktIngress (PktType & data)
{
  if (!((m_createEntryMask | m_updateEntryMask) & data.GetPacketPurpose ()))
  {
    return;
  }

  timer_struct_t timeStamp = m_externalModule->GetGlobalModule ()->GetGlobalTimer ()->GetTime ();


  LruData lruData, lruData2;
  const AcclContentName name = data.GetAcclName ();

  //bool valid = data.GetName(name).GetFullName();
  lruData.m_objectTimestamp = timeStamp;
  bool dataExists= (m_scratchpad->ExistData (name, lruData2));
  if (m_createEntryMask & data.GetPacketPurpose ())
  {
    if (dataExists) {
      m_normalize->DeleteValue(lruData2);
    }
    m_scratchpad->SetData (name, lruData);
    m_normalize->InsertValue(lruData);
    //update for interest pkts, only if the data exists
  }
  else if (dataExists)
  {
    m_normalize->DeleteValue(lruData2);
    m_scratchpad->SetData (name, lruData);
    m_normalize->InsertValue(lruData);
  }
}


bool
UtilityLru::OnInit (UtilityExternalModule * outsideData)
{
  m_externalModule = outsideData;
  bool status = true;
  if (m_externalModule == NULL) {
    status = false;
  } else  if (m_externalModule->GetGlobalModule () == NULL) {
    status = false;
  } else  if (m_externalModule->GetGlobalModule ()->GetGlobalTimer () == NULL) {
    status = false;
  }
  if (!status) {
    throw std::invalid_argument("Timer not specified in OnInit!");
  }
  return status;
}



double
UtilityLru::Value (const AcclContentName & name) const
{
  LruData lruData, existData;
  double retVal = -1.0;
  bool exist = m_scratchpad->ExistData (name, existData);
  if (exist) {
    const std::string fakeName="InvalidAndDontCareTempName";
    if (m_useNowAsTimeLimit) {
      timer_struct_t timeStamp = m_externalModule->GetGlobalModule ()->GetGlobalTimer ()->GetTime ();
      lruData.m_objectTimestamp = timeStamp;
      m_scratchpad->SetData (fakeName, lruData);
      m_normalize->InsertValue(lruData);

    }
    double val =  m_normalize->EvaluateValue(existData);
    if (m_useNowAsTimeLimit) {
      m_normalize->DeleteValue(lruData);
      m_scratchpad->EraseData(fakeName);

    }
    retVal = val;
  } else { //data not found!
    retVal = m_defaultValue;
  }

 std::cout << name << " lru value[" << existData << "] is " << retVal << "\n";

  return retVal;
}


uint64_t
UtilityLru::EstMemoryUsed (void) const
{
  return m_scratchpad->size ();    // * (sizeof(LruData) + 30 ); //EstMemoryUsed();
}

bool
UtilityLru::SelfTest()
{
  //send 10 packets to Lru, using temporal ordering
  //check if packets return correct value

  //access the 2nd packet.   check for correct values

  //delete 2nd packet.  check for correct values

  //add new packet, access '3rd' packet, check for correct values


  return true;   //jlm FIXME TODO until implemented
}

//match name
//Change match value to regex match value
UtilityNameAttrMatch::UtilityNameAttrMatch ()
  : m_modValue(1)
  , m_modMatchLow(1)
  , m_modMatchHigh(1)
  , m_matchType("option")
  , m_optionMatch("segment")
{
}
UtilityNameAttrMatch::UtilityNameAttrMatch(ConfigWrapper & config)
//: UtilityHandlerBase (config)
  : m_modValue(1)
  , m_modMatchLow(1)
  , m_modMatchHigh(1)
  , m_matchType("option")
  , m_optionMatch("segment")
{
  Config (config);
}

UtilityNameAttrMatch::~UtilityNameAttrMatch()
{
  delete m_scratchpad;
}

void
UtilityNameAttrMatch::Config (ConfigWrapper & xmlConfig)
{

  UtilityHandlerBase::Config(xmlConfig);
  if (!m_useAlias)
  {
    m_name = IdName ();
  }
  m_modValue = xmlConfig.GetAttribute ("modulus", m_modValue);
  m_modMatchLow = xmlConfig.GetAttribute ("matchLow", m_modMatchLow);
  m_modMatchHigh = xmlConfig.GetAttribute ("matchHigh", m_modMatchHigh);
  //hash on name, or hash on data
  m_matchType = xmlConfig.GetAttribute ("hashType", m_matchType);
  m_optionMatch = xmlConfig.GetAttribute ("nameOption", m_optionMatch);

  m_name.append ("_");
  if (m_matchType == "name")
  {
    m_name.append ("N:");
  }
  else if (m_matchType == "option")
  {
    m_name.append ("O(");
    m_name.append (m_optionMatch);
    m_name.append(")");
  }
  else {
    assert(0);
  }

  //m_nameOption = "%";
  std::ostringstream out;
  out << m_modValue;
  out << "%(";
  out << m_modMatchLow;
  out << ",";
  out << m_modMatchHigh;
  out << ")";
  m_name.append (out.str ());
  m_scratchpad = new StorageClass < AcclContentName, bool >;
  m_scratchpad->setStorageType (m_storageMethod);

}

void
UtilityNameAttrMatch::OnPktIngress (PktType & data)
{
  const AcclContentName name = data.GetAcclName ();
  uint64_t value=0;
  bool found = data.GetName().GetUniqAttribute(m_optionMatch, value);
  if (found) {
    value %= m_modValue;
    bool match = (bool) ((value >= m_modMatchLow) && (value <= m_modMatchHigh));
    m_scratchpad->SetData (name, match);
  } else {
    m_scratchpad->SetData (name, false);
  }

}

double
UtilityNameAttrMatch::Value (const AcclContentName & name) const
{
  if (m_scratchpad->empty ())
  {
    return m_defaultValue;
  }
  bool tmp = false;
  bool found = m_scratchpad->ExistData (name, tmp);  //could be a 'set' structure
  if (!found)
    return m_defaultValue;
  else
    return tmp ? 1.0 : 0.0;

}

uint64_t UtilityNameAttrMatch::EstMemoryUsed ()
{
  return 0.0;
}

bool
UtilityNameAttrMatch::SelfTest()
{
  bool results = true;
  //fix m_scratchpad->EraseAll();  //JLM FIXME TODO
  assert(m_scratchpad->empty());

  //FIXME TODO jlm SelfTest() should be configuration file agnostic
  //save current settings
  uint64_t modValue = m_modValue;
  uint64_t modMatchLow = m_modMatchLow;
  uint64_t modMatchHigh = m_modMatchHigh;
  std::string matchType = m_matchType;
  std::string optionMatch = m_optionMatch;

  //setup default values;
  m_modValue = 4;
  m_modMatchLow = 1;
  m_modMatchHigh = 2;
  m_matchType = "option";
  m_optionMatch = "seg";

  //create a name, with 3 attributes, match attribute
  IcnName < std::string > nameTest;
  nameTest.SetFullName("/sample/test/seg=2/b=2/c=3");
  PktType test(nameTest);
  test.SetPacketPurpose(PktType::DATAPKT);
  OnPktIngress(test);
  Compute();
  double val = Value(test.GetAcclName());

  results &= (val > .99);
  //check if incorrect attribute returns 0
  IcnName < std::string > nameTest2;
  nameTest2.SetFullName("/sample/test/ges=3/bb=2/c=3");
  PktType test2(nameTest2);
  test2.SetPacketPurpose(PktType::DATAPKT);
  OnPktIngress(test2);

  Compute();
  val = Value(test2.GetAcclName());

  results &= (val < .0001);

  //attribute matches, wrong range
  IcnName<std::string> nameTest3;
  nameTest3.SetFullName("/sample/test/seg=3/b=2/c=3");
  PktType test3(nameTest3);
  test3.SetPacketPurpose(PktType::DATAPKT);
  OnPktIngress(test3);
  Compute();
  val = Value(test3.GetAcclName());

  results &= (val < .0001);


  //check if correct name returns 1
  m_modValue = 4;
  m_modMatchLow = 1;
  m_modMatchHigh = 2;
  m_matchType = "file";
  m_optionMatch = "bb";

//add file name checking

  //check if correct attribute returns 1


  //reset defined defaults;
  m_modValue = modValue;
  m_modMatchLow =modMatchLow ;
  m_modMatchHigh =modMatchHigh ;
  m_matchType = matchType;
  m_optionMatch = optionMatch;

  return results;
}


//hash
UtilityHashModulus::UtilityHashModulus ()
  : m_modValue(1)
  , m_modMatchLow(1)
  , m_modMatchHigh(1)
  , m_hashType("fullname")
  , m_optionMatch("segment")
{
}
UtilityHashModulus::UtilityHashModulus (ConfigWrapper & config)
//: UtilityHandlerBase (config)
  : m_modValue(1)
  , m_modMatchLow(1)
  , m_modMatchHigh(1)
  , m_hashType("fullname")
  , m_optionMatch("segment")
{
  Config (config);
}

UtilityHashModulus::~UtilityHashModulus ()
{
}

void
UtilityHashModulus::Config (ConfigWrapper & xmlConfig)
{

  UtilityHandlerBase::Config(xmlConfig);
  if (!m_useAlias)
  {
    m_name = IdName ();
  }
  m_modValue = xmlConfig.GetAttribute ("modulus", m_modValue);
  m_modMatchLow = xmlConfig.GetAttribute ("matchLow", m_modMatchLow);
  m_modMatchHigh = xmlConfig.GetAttribute ("matchHigh", m_modMatchHigh);
  //hash on name, or hash on data
  m_hashType = xmlConfig.GetAttribute ("hashType", m_hashType);

  m_name.append ("_");
  if (m_hashType == "data")
  {
    m_name.append ("D:");
  }
  else if (m_hashType == "fullname")
  {
    m_name.append ("F:");
  }
  else {
    assert(0);
  }

  //m_nameOption = "%";
  std::ostringstream out;
  out << m_modValue;
  out << "%(";
  out << m_modMatchLow;
  out << ",";
  out << m_modMatchHigh;
  out << ")";
  m_name.append (out.str ());

}

double
UtilityHashModulus::Value (const AcclContentName & name) const
{

  uint64_t value;
  if (!m_hashType.compare("fullname")) {
    value = makeHash ((name.c_str()), name.size ());  //FIXME should be mapped, since aclname can be an integer
  }
  else
  {
    throw std::invalid_argument("Data hashing not implemented yet!");
  }


  //std::cout << value << "%" << m_modValue;
  value %= m_modValue;
  //std::cout << " = " << value << "\n";
  return (double) ((value >= m_modMatchLow) && (value <= m_modMatchHigh));

}

uint64_t UtilityHashModulus::EstMemoryUsed ()
{
  return 0.0;
}

//poor hash function, need improvement FIXME TODO
uint64_t
UtilityHashModulus::makeHash (const char *hash, uint32_t hashLength) const
{
  //simple hash
  uint64_t qHash = 0;
  uint64_t oHash;
  uint32_t start = 0;
  uint32_t length = hashLength;
  while (length > 8)
  {
    oHash = *((uint64_t *) & hash[start]);
    qHash ^= oHash;
    start += 8;
    length -= 8;
  }
  for (uint32_t i = start; i <= hashLength; i++)
  {
    oHash = hash[i];     // << i*8;
    qHash ^= oHash;
  }

  return qHash;

}


//Random
UtilityRnd::UtilityRnd (ConfigWrapper & config):
  //UtilityHandlerBase (config),
  m_rndType (rndOnce)
{
  Config (config);
}

UtilityRnd::~UtilityRnd ()
{
  delete m_scratchpad;
}

//jlm FIXME TODO should have an option to use other random generator
void
UtilityRnd::Config (ConfigWrapper & config)
{
  UtilityHandlerBase::Config(config);
  if (!m_useAlias)
  {
    m_name = IdName ();
  }
  std::string val = config.GetAttribute ("randomType", "");
  if (!val.empty ())
  {
    if (!val.compare ("onceRnd"))
    {
      m_rndType = rndOnce;
      m_name.append ("_O");
    }
    else if (!val.compare ("alwaysRnd"))
    {
      m_rndType = rndAlways;
      m_name.append ("_C");
    }
    else
    {
      throw std::invalid_argument ("Invalid Rnd parameter");
    }
  }
  m_scratchpad = new StorageClass < dataNameType_t, double >;

  m_scratchpad->setStorageType (m_storageMethod);
}

void
UtilityRnd::DoDelete (const AcclContentName & name)
{
  if (m_rndType == rndOnce)
    m_scratchpad->EraseData (name);
}

void
UtilityRnd::OnPktIngress (PktType & data)
{
  if (!((m_createEntryMask) & data.GetPacketPurpose ()))
  {
    return;
  }
  if (m_rndType == rndOnce)
  {
    double rndVal = (double) (std::rand ()) / (RAND_MAX);
    AcclContentName name = data.GetAcclName ();

    //m_scratchpad.insert(std::pair< AcclContentName, double > (name, rndVal));
    m_scratchpad->SetData (name, rndVal);
  }
}

double
UtilityRnd::Value (const AcclContentName & name) const
{

  if (rndOnce == m_rndType)
  {
    double retValue;
    bool exist = m_scratchpad->ExistData (name, retValue);

    if (exist)
      return retValue;
    else
      return m_defaultValue;
  }
  else
  {
    return (double) (std::rand ()) / (RAND_MAX);
  }
}

uint64_t
UtilityRnd::EstMemoryUsed (void)
{
  //return m_scratchpad.EstMemoryUsed();
  return 1;                     //JLM FIXME

}


//constant
UtilityConst::UtilityConst (ConfigWrapper & xmlConfig)//: UtilityHandlerBase (xmlConfig)
{

  Config (xmlConfig);
}

UtilityConst::~UtilityConst ()
{
}

void
UtilityConst::Config (ConfigWrapper & config)
{
  UtilityHandlerBase::Config(config);
  if (!m_useAlias)
  {
    m_name = IdName ();
  }
}

double
UtilityConst::Value (const AcclContentName & name) const
{
  return m_defaultValue;
}

uint64_t UtilityConst::EstMemoryUsed (void)
{
  return 0;
}

void
UtilityConst::Print (std::ostream & os, bool endParen) const
{
  if (m_usePrettyPrint && !m_useAlias)
  {
    os << m_defaultValue;
  }
  else
  {
    UtilityHandlerBase::Print (os, endParen);
  }
}

void
UtilityConst::Print (std::ostream & os, const AcclContentName & name, double &value)  const
{
  if (m_usePrettyPrint)
  {
    os << m_defaultValue;
  }
  else
  {
    UtilityHandlerBase::Print (os, name, value);
  }
  value = Value (name);
}

UtilityEgressCount::UtilityEgressCount()
{
}

UtilityEgressCount::UtilityEgressCount (ConfigWrapper & xmlConfig)
{
  Config (xmlConfig);
}

UtilityEgressCount::~UtilityEgressCount()
{
  delete m_scratchpad;
}

void
UtilityEgressCount::Config (ConfigWrapper & xmlConfig)
{
  UtilityHandlerBase::Config(xmlConfig);
  if (!m_useAlias)
  {
    m_name = IdName ();
  }
}

void
UtilityEgressCount::DoDelete (const AcclContentName & name)
{
  m_scratchpad->EraseData (name);
}
void
UtilityEgressCount::OnPktEgress (PktType & pkt, const PktTxStatus & status)
{

}

double
UtilityEgressCount::Value (const AcclContentName & name) const
{
  assert(0);
  return 0.0;

}


uint64_t
UtilityEgressCount::EstMemoryUsed (void) const
{
  return m_scratchpad->size ();
}

//evaluate based upon attribute value
UtilityU64ValuationEval::UtilityU64ValuationEval()
  : m_normalize(nullptr) {}
UtilityU64ValuationEval::UtilityU64ValuationEval(ConfigWrapper &config)
  : m_normalize(nullptr) {
  Config(config);
}
UtilityU64ValuationEval::~UtilityU64ValuationEval() {
  if (m_scratchpad)
    delete m_scratchpad;
  if (m_normalize)
    delete m_normalize;
}

void UtilityU64ValuationEval::OnPktIngress (PktType &data) {
  uint64_t value=0;
  AcclContentName name = data.GetAcclName ();
  bool attribExists = data.GetUnsignedNamedAttribute (m_defaultAttribute, value);

  if (attribExists) {
    uint64_t retValue;
    bool exist = m_scratchpad->ExistData (name, retValue);
    if (exist) {
      m_normalize->DeleteValue(retValue);
    }
    m_scratchpad->SetData(name, value);
    m_normalize->InsertValue(value);
  }
}

void UtilityU64ValuationEval::DoDelete (const AcclContentName & name) {
  uint64_t retValue;
  bool exist = m_scratchpad->ExistData (name, retValue);
  if (exist) {
    m_normalize->DeleteValue(retValue);
    m_scratchpad->EraseData(name);
  }
}

void UtilityU64ValuationEval::Config (ConfigWrapper & config) {
  UtilityHandlerBase::Config(config);
  if (!m_useAlias)
  {
    m_name = IdName ();
  }
  //set normalize
  ConfigWrapper *normConfig = config.GetFirstChildUtility("Normalize");
  if (normConfig->valid()) {
    m_normalize = NormalizeGenerator<uint64_t>::CreateNewNormalizeEval(*normConfig);
    std::stringstream ss;
    m_normalize->Print(ss);
    m_name.append(ss.str());
  }
  //set other values
  m_scratchpad = new StorageClass < dataNameType_t, uint64_t >;

  m_scratchpad->setStorageType (m_storageMethod);

}
double  UtilityU64ValuationEval::Value (const AcclContentName & name) const {
  uint64_t retValue;
  bool exist = m_scratchpad->ExistData (name, retValue);
  if (exist) {
    return m_normalize->EvaluateValue(retValue);
  } else { //data not found!
    return m_defaultValue;
  }

}

uint64_t UtilityU64ValuationEval::EstMemoryUsed (void) const {
  return 0;
}




UtilityProtLastElement::UtilityProtLastElement (ConfigWrapper & config):
  m_lastElemValid (false)
{
  Config (config);
}

UtilityProtLastElement::~UtilityProtLastElement ()
{
}

void
UtilityProtLastElement::Config (ConfigWrapper & config)
{
  UtilityHandlerBase::Config(config);
  if (!m_useAlias)
  {
    m_name = IdName ();
  }
}

void
UtilityProtLastElement::DoDelete (const AcclContentName & name)
{
   if (name == m_lastElementSeen)
	   m_lastElemValid=false;
}

void
UtilityProtLastElement::OnPktIngress (PktType & data)
{
	m_lastElementSeen= data.GetAcclName();
	m_lastElemValid = true;
}

double
UtilityProtLastElement::Value (const AcclContentName & name) const
{
   if (m_lastElemValid && name==m_lastElementSeen)
	   return 1.0;
   else
	   return 0.0;
}

uint64_t
UtilityProtLastElement::EstMemoryUsed (void)
{
  return m_lastElementSeen.size();                    
}

