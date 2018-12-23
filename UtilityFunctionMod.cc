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
#include "UtilityFunctionMod.h"


//Freshness, use T_EXPIRY
UtilityFresh::UtilityFresh (ConfigWrapper & xmlConfig)
  //:UtilityHandlerBase (xmlConfig)
   : m_decayRate (flat)
   , m_defValue (0.0)
{
  Config (xmlConfig);
}

UtilityFresh::~UtilityFresh ()
{
  delete m_scratchpad;
}

void
UtilityFresh::Config (ConfigWrapper & xmlConfig)
{
  UtilityHandlerBase::Config(xmlConfig);
  if (!m_useAlias)
    {
      m_name = IdName ();
    }
  std::string decayType = xmlConfig.GetAttribute ("decay_rate", "linear");
  if (!decayType.compare ("linear"))
    {
      m_decayRate = linear;
      m_name.append ("_L");
    }
  else
    {
      m_decayRate = flat;
      m_name.append ("_F");
    }
  //m_matchingName = xmlConfig.GetAttribute ("fresh_name", m_matchingName);
  //read default value again, this time as a double
  m_defValue = xmlConfig.GetAttribute ("missingAttributeDefaultValue", m_defValue);
  m_scratchpad = new StorageClass < AcclContentName, freshStruct_t >;
  m_scratchpad->setStorageType (m_storageMethod);
  if (m_defaultAttribute.empty()) {
     m_defaultAttribute = IdName();
  }
}

void
UtilityFresh::OnPktIngress (PktType & data)
{
  if (!((m_createEntryMask | m_updateEntryMask) & data.GetPacketPurpose ()))
    {
      return;
    }
  double value;
  bool freshExists = data.GetNamedAttribute (m_defaultAttribute, value);;   //does T_EXPIRY exist?
  if (freshExists)
    {
      //JLM FIXME do math on timer struct, not this garbage
      timer_struct_t timeNow = m_externalModule->GetGlobalModule ()->GetGlobalTimer ()->GetTime ();
      timer_struct_t timeExpire;

      timeExpire.tv_sec = timeNow.tv_sec + (int) value;
      timeExpire.tv_nsec = timeNow.tv_nsec + (value - ((int) value)) * 1000000000.0;
      //this is the expiration time
      freshStruct_t timestamp;

      timestamp.pktExpire = timeExpire;
      timestamp.pktRx = timeNow;
      m_scratchpad->SetData (data.GetAcclName (), timestamp);
    }
  else if (m_attributeInsert)
    {                           //Not there, add default
      data.SetNamedAttribute (m_defaultAttribute, m_defaultInsertValue);
      timer_struct_t timeNow = m_externalModule->GetGlobalModule ()->GetGlobalTimer ()->GetTime ();
      timer_struct_t timeExpire;

      timeExpire.tv_sec = timeNow.tv_sec + (uint64_t) m_defValue;
      timeExpire.tv_nsec = timeNow.tv_sec + (m_defValue - ((uint64_t) m_defValue)) * 1000000000ULL;
      freshStruct_t timestamp;

      timestamp.pktExpire = timeExpire;
      timestamp.pktRx = timeNow;

      m_scratchpad->SetData (data.GetAcclName (), timestamp);
    }
  else
    {
      //uint64_t max = (uint64_t) -1;
      //timer_struct_t expireTime;
      //expireTime.tv_sec = max;
      //m_scratchpad->SetData(data.GetAcclName(), expireTime);

    }
}

void
UtilityFresh::OnPrePktEgress (PktType & data)
{
   //modify timer, does NOT change the packet in store/cache
} 

double
UtilityFresh::Value (const AcclContentName & name) const
{
  //timer_struct_t time; //expiration time
  freshStruct_t timeData;
  bool freshExists = m_scratchpad->ExistData (name, timeData);
  timer_struct_t now = m_externalModule->GetGlobalModule ()->GetGlobalTimer ()->GetTime ();
  if (!freshExists)
    {
      return m_defaultMissingReturnValue;
    }                      //send default value

  //check if content is stale (not fresh)
  if (1 == m_externalModule->GetGlobalModule ()->GetGlobalTimer ()->TimeCompare (now, timeData.pktExpire))
    {
      return 0.0;
    }

  if (flat == m_decayRate)
    {
      return 1.0;
    }
  else
    {
      double ret = m_externalModule->GetGlobalModule ()->GetGlobalTimer ()->Ratio (now, timeData.pktExpire, timeData.pktRx);

      return 1.0 - ret;
    }
}

uint64_t
UtilityFresh::EstMemoryUsed (void)
{
  return m_scratchpad->EstMemoryUsed ();
}

void
UtilityFresh::DoDelete (const AcclContentName & name)
{
  m_scratchpad->EraseData (name);
}






//Count (either up or down)
UtilityCnt::UtilityCnt (ConfigWrapper & xmlConfig)
   : m_startCount (0) //default is not to do a count
   , m_cntCondition (decrement)
   , m_op("decrement")
   , m_rangeCompare("LeftRightInclusive")
   , m_lowerBound(0)
   , m_upperBound(0)
{
  Config (xmlConfig);
}

UtilityCnt::~UtilityCnt ()
{
   delete m_scratchpad;
}

void
UtilityCnt::Config (ConfigWrapper & xmlConfig)
{
  UtilityHandlerBase::Config(xmlConfig);
  if (!m_useAlias)
    {
      m_name = IdName ();
    }
  if (m_defaultAttribute.empty()) {
     m_defaultAttribute = IdName();
  }
  m_startCount = xmlConfig.GetAttribute ("missing_count_value", m_startCount);
  m_lowerBound = xmlConfig.GetAttribute ("matching_lower_bound", m_lowerBound);
  m_upperBound = xmlConfig.GetAttribute ("matching_upper_bound", m_upperBound);
  std::string decayType = xmlConfig.GetAttribute ("count_condition", m_op);
  if (!decayType.compare ("decrement"))
    {
      m_cntCondition = decrement;
      m_name.append ("_d:");
    }
  else if (!decayType.compare("increment"))
    {
      m_cntCondition = increment;
      m_name.append ("_i:");
    }
  else if (!decayType.compare("none")) {} 
  else { //unknown
      assert(0);
    }

  m_rangeCompare = xmlConfig.GetAttribute("match_criteria", m_rangeCompare);
  //Set lower range, higher range, inclusion
  m_validRange.SetLowerBound(m_lowerBound);
  m_validRange.SetUpperBound(m_upperBound);

  m_validRange.SetRangeCompare(m_rangeCompare);
//.IsInRange();
  std::ostringstream out;
  out << m_validRange;
  m_name.append(out.str());

  //read default value again, this time as a double
  m_scratchpad = new StorageClass < AcclContentName, bool >;
  m_scratchpad->setStorageType (m_storageMethod);

}

void
UtilityCnt::OnPktIngress (PktType & data)
{
  if (!((m_createEntryMask | m_updateEntryMask) & data.GetPacketPurpose ()))
    {
      return;
    }
  uint64_t value=0;
  bool countExists = data.GetUnsignedNamedAttribute (m_defaultAttribute, value);
  if (countExists) {
     Condition(value);
     if (m_validRange.IsInRange(value)) { //in not in range
      //data.SetUnsignedNamedAttribute(m_defaultAttribute, value); //dont write if criteria is met, otherwise it may go out of bounds!
      m_scratchpad->SetData (data.GetAcclName (), true);
     } else {
      data.SetUnsignedNamedAttribute(m_defaultAttribute, value);
      m_scratchpad->SetData (data.GetAcclName (), false);
     }
  } else if (m_startCount) {
     data.SetUnsignedNamedAttribute(m_defaultAttribute, m_startCount);
  }

}



double
UtilityCnt::Value (const AcclContentName & name) const
{
  //timer_struct_t time; //expiration time
  bool cntMet;
  bool exists = m_scratchpad->ExistData (name, cntMet);
  if (!exists)
    {
      return m_defaultMissingReturnValue;
    }                      //send default value

   //check if content is stale (not fresh)
  if (cntMet) {
      return 1.0;
  } else {
      return 0.0 ;
    }
}

uint64_t
UtilityCnt::EstMemoryUsed (void)
{
  return m_scratchpad->EstMemoryUsed ();
}

void
UtilityCnt::DoDelete (const AcclContentName & name)
{
  m_scratchpad->EraseData (name);
}

//match range - need to be int?
void
UtilityCnt::Condition(uint64_t &count) const 
{
  //JLM FIXME add condition to count by another external field
  if(m_cntCondition == decrement) {
    count--;
  } else if (m_cntCondition == increment) {
    count++;
  } else if (m_cntCondition == none) {}
  else //unknown
  {
     assert(0);
  }
}

//NameChain
UtilityNameChain::UtilityNameChain (ConfigWrapper & xmlConfig)
   : m_nameAppendIfnExist(false),
   m_appendFront(true),
   m_allowNodeNameAppend(true),
   m_partition("/"),
   m_eachNodeUnique(true)
{
  Config (xmlConfig);
}

UtilityNameChain::~UtilityNameChain ()
{
}

void
UtilityNameChain::Config (ConfigWrapper & xmlConfig)
{
  UtilityHandlerBase::Config(xmlConfig);
  if (!m_useAlias)
    {
      m_name = IdName ();
    }
  if (m_defaultAttribute.empty()) {
     m_defaultAttribute = IdName();
  }

  m_nameAppendIfnExist = xmlConfig.GetAttributeBool ("appendIfnExist", m_nameAppendIfnExist);
  m_appendFront = xmlConfig.GetAttributeBool ("appendInFront", m_appendFront);
  m_allowNodeNameAppend = xmlConfig.GetAttributeBool ("appendNodeName", m_allowNodeNameAppend);
  m_partition = xmlConfig.GetAttribute ("nodeNamePartition", m_partition);
  m_eachNodeUnique = xmlConfig.GetAttributeBool ("nodeNameUnique", m_eachNodeUnique);

}

void
UtilityNameChain::OnPktIngress (PktType & data)
{
  if (!((m_createEntryMask | m_updateEntryMask) & data.GetPacketPurpose ()))
    {
      return;
    }
  if (m_allowNodeNameAppend) {
    std::string path="";
    std::string nodeName= m_externalModule->GetDevice()->GetNodeName(); //FIXME TODO should be node name
    //std::string nodeName= m_externalModule->GetDevice().GetSelfNode()->Name();
    bool pathExists = data.GetNamedAttribute (m_defaultAttribute, path);
    if (pathExists) {


      size_t found = path.find(nodeName);
      if (m_eachNodeUnique && (found <= path.size())) { return; }

      if (m_appendFront) {
          std::string orig_path = path;
          path = nodeName;
          path.append(orig_path);
          path.append(m_partition);
      } else {
          path.append(nodeName);
          path.append(m_partition);
      }
        
        
     } else if (m_nameAppendIfnExist) {
          path.append(nodeName);
          path.append(m_partition);
       }
      data.SetNamedAttribute(m_defaultAttribute, path);
   }
}

double
UtilityNameChain::Value (const AcclContentName & name) const
{
   return m_defaultMissingReturnValue;
}

//MatchChainName
UtilityMatchChainName::UtilityMatchChainName (ConfigWrapper & xmlConfig)
  : m_checkOnlyFront(true),
  m_checkAnywhere(false),
  m_deleteMatch(false),
  m_partition("")
{
  Config (xmlConfig);
}

UtilityMatchChainName::~UtilityMatchChainName ()
{
   delete m_scratchpad;
}

void
UtilityMatchChainName::Config (ConfigWrapper & xmlConfig)
{
  UtilityHandlerBase::Config(xmlConfig);
  if (!m_useAlias)
    {
      m_name = IdName ();
    }
  if (m_defaultAttribute.empty()) {
     m_defaultAttribute = IdName();
  }
  m_checkOnlyFront = xmlConfig.GetAttributeBool ("checkOnlyFront", m_checkOnlyFront);
  m_checkAnywhere = xmlConfig.GetAttributeBool ("checkAnywhere", m_checkAnywhere);
  m_deleteMatch = xmlConfig.GetAttributeBool ("deleteMatch", m_deleteMatch);
  m_partition = xmlConfig.GetAttribute ("nodeNamePartition", m_partition);

  m_scratchpad = new StorageClass < AcclContentName, bool >;
  m_scratchpad->setStorageType (m_storageMethod);

}

void
UtilityMatchChainName::OnPktIngress (PktType & data)
{
  if (!((m_createEntryMask | m_updateEntryMask) & data.GetPacketPurpose ()))
    {
      return;
    }
  uint64_t value=0;
  data.SetNamedAttribute(m_defaultAttribute, value);
  m_scratchpad->SetData (data.GetAcclName (), true);

}



double
UtilityMatchChainName::Value (const AcclContentName & name) const
{
  bool cntMet;
  bool exists = m_scratchpad->ExistData (name, cntMet);
  if (!exists)
    {
      return m_defaultMissingReturnValue;
    }                      //send default value

   //check if content is stale (not fresh)
  if (cntMet) {
      return 1.0;
  } else {
      return 0.0 ;
    }
}


