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


bool
sort_lru (const std::pair < dataNameType_t, LruData > &left, const std::pair < dataNameType_t, LruData > &right)
{
  return (left.second.m_objectTimestamp < right.second.m_objectTimestamp);
}
bool
sort_lfu (const std::pair < dataNameType_t, LfuData > &left, const std::pair < dataNameType_t, LfuData > &right)
{
  return (left.second.m_weight < right.second.m_weight);
}

UtilityLruSp::UtilityLruSp ()
  : m_timeChange(false)
{
}

UtilityLruSp::UtilityLruSp (ConfigWrapper & xmlConfig)
  : m_timeChange(false)
{

  Config (xmlConfig);

}

UtilityLruSp::~UtilityLruSp ()
{
  delete m_scratchpad;
}

/*
void
UtilityLruSp::Print (std::ostream & os, const AcclContentName & name, double &value)  const
{
  Print (os, false);
  LruDataSpatial data64;

  bool exist = m_scratchpad->ExistData (name, data64);
  os << "(" << data64.m_position <<  ")/(" <<  m_scratchpad->size () << ")=";

  value = Value (name);
  os << std::setprecision (15) << value;
  os << ")";

} */

void
UtilityLruSp::Config (ConfigWrapper & xmlConfig)
{
  UtilityHandlerBase::Config(xmlConfig);
  if (!m_useAlias)
    {
      m_name = IdName ();
    }

  m_scratchpad = new StorageClass < AcclContentName, LruDataSpatial >;
  m_scratchpad->setStorageType (m_storageMethod);
}

void
UtilityLruSp::DoDelete (const AcclContentName & name)
{

     m_timeChange = true;
  m_scratchpad->EraseData (name);
}

void
UtilityLruSp::OnPktIngress (PktType & data)
{
  if (!((m_createEntryMask | m_updateEntryMask) & data.GetPacketPurpose ()))
    {
      return;
    }

  timer_struct_t timeStamp = m_externalModule->GetGlobalModule ()->GetGlobalTimer ()->GetTime ();

  LruDataSpatial lruData, lruData2;
  const AcclContentName name = data.GetAcclName ();

  //bool valid = data.GetName(name).GetFullName();
  lruData.m_objectTimestamp = timeStamp;
  if (m_createEntryMask & data.GetPacketPurpose ())
    {
      m_scratchpad->SetData (name, lruData);
      //update for interest pkts, only if the data exists
    }
  else if (m_scratchpad->ExistData (name, lruData2))
    {
      m_scratchpad->SetData (name, lruData);
    }
    m_timeChange = true;
}


bool
UtilityLruSp::OnInit (UtilityExternalModule * outsideData)
{
  m_externalModule = outsideData;
  bool status = true;
  if ((m_externalModule == NULL) || (m_externalModule->GetGlobalModule () == NULL) || (m_externalModule->GetGlobalModule ()->GetGlobalTimer () == NULL)) {
     status = false;
  }
  if (!status) {
     throw std::invalid_argument("Timer not specified in OnInit!"); 
  }
  return status;
}


        //call Compute before Value, in case need to adjust values in relation to itself
void
UtilityLruSp::Compute (const AcclContentName & name)
{
  Compute ();
}

void
UtilityLruSp::Compute ()
{
  if (!m_timeChange)
    {                           //no change, no need to recalc
      return;
    }
  m_timeChange = false;

  //uint32_t position = 0;
  std::list < std::pair < dataNameType_t, LruDataSpatial > >*temppad = m_scratchpad->GetDataAsList ();
  if (temppad->empty ())
    {
      delete temppad;
      return;
    }                           //no data

  std::list < std::pair < dataNameType_t, LruDataSpatial > >::iterator it = temppad->begin ();

      int i = 0;

      //std::sort(temppad->begin(), temppad->end(), this->sort_pred);
      temppad->sort (sort_lru);
      for (std::list < std::pair < dataNameType_t, LruDataSpatial > >::iterator it2 = temppad->begin (); it2 != temppad->end (); it2++)
        {
          (it2->second).m_position = ++i;
        }

      //put values back into storage
      m_scratchpad->SetDataAsList (*temppad);

  delete temppad;

//debug
}

double
UtilityLruSp::Value (const AcclContentName & name) const
{

//std::cout << "scratchpad size: " << m_scratchpad.size() << std::endl;
  if (m_scratchpad->empty ())
    {
      return m_defaultValue;
    }
  if (1 == m_scratchpad->size ())
    {
      return 1.0;
    }
  LruDataSpatial lruTime;
  bool found = m_scratchpad->ExistData (name, lruTime);
  if (!found)
     return m_defaultValue; 

  uint64_t numEntries = m_scratchpad->size ();

  uint64_t lruValue = lruTime.m_position;

  return ((double) (lruValue) / (numEntries));       //0-1 values
}


uint64_t
UtilityLruSp::EstMemoryUsed (void) const
{
  return m_scratchpad->size ();    // * (sizeof(LruData) + 30 ); //EstMemoryUsed();   
}

bool
UtilityLruSp::SelfTest()
{
   //send 10 packets to Lru, using temporal ordering
   //check if packets return correct value

   //access the 2nd packet.   check for correct values

  //delete 2nd packet.  check for correct values

   //add new packet, access '3rd' packet, check for correct values


  return true;   //jlm FIXME TODO until implemented
}

UtilityLfuSp::UtilityLfuSp ()
 : m_spaceChange(false)
{

}

UtilityLfuSp::UtilityLfuSp (ConfigWrapper & xmlConfig)
 : m_spaceChange(false)
{

  Config (xmlConfig);

}

UtilityLfuSp::~UtilityLfuSp ()
{
  delete m_scratchpad;
}

void
UtilityLfuSp::Config (ConfigWrapper & xmlConfig)
{
  UtilityHandlerBase::Config(xmlConfig);
  if (!m_useAlias)
    {
      m_name = IdName ();
    }
  m_scratchpad = new StorageClass < AcclContentName, LfuDataSpatial >;
  m_scratchpad->setStorageType (m_storageMethod);
}


void
UtilityLfuSp::DoDelete (const AcclContentName & name)
{
    LfuDataSpatial data;
  bool exist = m_scratchpad->ExistData (name, data);
  if (exist) {
     m_spaceChange = true; 
 
  } 


  m_scratchpad->EraseData (name);
}

void
UtilityLfuSp::OnPktIngress (PktType & data)
{
  if (!((m_createEntryMask | m_updateEntryMask) & data.GetPacketPurpose ()))
    {
      return;
    }
   


  //uint64_t numEntries = m_scratchpad->size ();

  const AcclContentName name = data.GetAcclName ();

  //bool valid = data.GetName(name).GetFullName();
  LfuDataSpatial data64;
  data64.m_weight=0;
  bool exist = m_scratchpad->ExistData (name, data64);
  if (m_createEntryMask & data.GetPacketPurpose ())
    {
      if (exist) {  //update entry
        if (UINTMAX_MAX != data64.m_weight) {
          data64.m_weight++;
        }
        m_scratchpad->SetData (name, data64);

        } else {  //first entry
          data64.m_weight = 1;
          m_scratchpad->SetData (name, data64);
          exist = true;
        }
      //update for interest pkts, only if the data exists
    }
  else if (exist) //m_updateEntryMask() is true, and the data exists, e.g. Interest packet
    {
      if (UINTMAX_MAX != data64.m_weight) {
         data64.m_weight++;
      }
      m_scratchpad->SetData (name, data64);
    }

   //if the data is valid (exist) and updated, check limits
   m_spaceChange=true;
}



        //call Compute before Value, in case need to adjust values in relation to itself
void
UtilityLfuSp::Compute (const AcclContentName & name)
{
  Compute ();
}

void
UtilityLfuSp::Compute ()
{
  if (!m_spaceChange || (!m_scratchpad->size()))
    {                           //no change, no need to recalc
      return;
    }
  m_spaceChange = false;

  //JLM FIXME TODO LRU/LFU need option to use pointers to avoid unnecessary copying
  std::list < std::pair < dataNameType_t, LfuDataSpatial > >*temppad = m_scratchpad->GetDataAsList ();
  temppad->sort(sort_lfu);
  //temppad->reverse();
  uint64_t i=temppad->size();
  std::list < std::pair < dataNameType_t, LfuDataSpatial > >::reverse_iterator it2 = temppad->rbegin ();
  uint64_t lcv = it2->second.m_weight;
  uint64_t count= i;
  for (  ;it2 != temppad->rend (); it2++)
        {
           uint64_t cv = it2->second.m_weight;

          if (cv != lcv) { lcv = cv; count = i;} 
          (it2->second).m_position = count;
          --i;
        }

      //put values back into storage
      m_scratchpad->SetDataAsList (*temppad);


  delete temppad; 
  

//debug
}

bool
UtilityLfuSp::SelfTest()
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
      double ans[5]={.2,.6,1,.4,.6};
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
         results &= ( rng.IsInRange(val) ); //should use a range data item, FIXME TODO JLM

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
UtilityLfuSp::Value (const AcclContentName & name) const
{

//std::cout << "scratchpad size: " << m_scratchpad.size() << std::endl;
  if (m_scratchpad->empty ())
    {
      return m_defaultValue;
    }
  if (1 == m_scratchpad->size ())
    {
      return 1.0;
    }
  LfuDataSpatial lfu;
  bool found = m_scratchpad->ExistData (name, lfu);
  if (!found)
     return m_defaultValue; 

  double ret;
  ret = (double) (lfu.m_position)/(m_scratchpad->size()); 
  return ret;
}

/*
void
UtilityLfuSp::Print (std::ostream & os, const AcclContentName & name, double &value)  const
{
  Print (os, false);
  LfuDataSpatial data64;
  data64.m_weight=0;
  bool exist = m_scratchpad->ExistData (name, data64);
  os << "(" << data64.m_position << ")/("  << m_scratchpad->size() << ")=";
  value = Value (name);
  os << std::setprecision (15) << value;
  os << ")";

}*/

uint64_t
UtilityLfuSp::EstMemoryUsed (void) const
{
  return m_scratchpad->size ();    // * (sizeof(LruData) + 30 ); //EstMemoryUsed();   
}



//LFU
UtilityLfu::UtilityLfu ()
 : m_spaceChange(false)
{
  m_lfu.m_weight=1;
  m_mfu.m_weight=0;

}

UtilityLfu::UtilityLfu (ConfigWrapper & xmlConfig)
 : m_spaceChange(false)
{
  m_lfu.m_weight=1;
  m_mfu.m_weight=0;

  Config (xmlConfig);

}

UtilityLfu::~UtilityLfu ()
{
  delete m_scratchpad;
}

void
UtilityLfu::Config (ConfigWrapper & xmlConfig)
{
  UtilityHandlerBase::Config(xmlConfig);
  if (!m_useAlias)
    {
      m_name = IdName ();
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
    if ((data.m_weight <= m_lfu.m_weight) || (data.m_weight >= m_mfu.m_weight)) { m_spaceChange = true; }
 
  } 


  m_scratchpad->EraseData (name);
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
  data64.m_weight=0;
  bool exist = m_scratchpad->ExistData (name, data64);
  if (m_createEntryMask & data.GetPacketPurpose ())
    {
      if (exist) {  //update entry
        if (UINTMAX_MAX != data64.m_weight) {
          data64.m_weight++;
        }
        m_scratchpad->SetData (name, data64);

        } else {  //first entry
          data64.m_weight = 1;
          m_scratchpad->SetData (name, data64);
          exist = true;
        }
      //update for interest pkts, only if the data exists
    }
  else if (exist) //m_updateEntryMask() is true, and the data exists, e.g. Interest packet
    {
      if (UINTMAX_MAX != data64.m_weight) {
         data64.m_weight++;
      }
      m_scratchpad->SetData (name, data64);
    }

   //if the data is valid (exist) and updated, check limits
   if (exist) {
//std::cout << "inc pkt " << name << " -- " << data64.m_weight << "\n";
    if (m_mfu.m_weight < data64.m_weight) { m_mfu = data64;}
    if (m_lfu.m_weight > data64.m_weight) { m_lfu = data64;}
    if (m_lfu.m_weight == (data64.m_weight-1)) { m_spaceChange=true;} //min changed?
   }
}



        //call Compute before Value, in case need to adjust values in relation to itself
void
UtilityLfu::Compute (const AcclContentName & name)
{
  Compute ();
}

void
UtilityLfu::Compute ()
{
  if (!m_spaceChange)
    {                           //no change, no need to recalc
      return;
    }
  m_spaceChange = false;

  //JLM FIXME TODO LRU/LFU need option to use pointers to avoid unnecessary copying
  std::list < std::pair < dataNameType_t, LfuData > >*temppad = m_scratchpad->GetDataAsList ();
  std::list < std::pair < dataNameType_t, LfuData > >::const_iterator cit = temppad->begin ();
  if (cit != temppad->end()) {
    m_mfu.m_weight=0;
    m_lfu.m_weight=UINTMAX_MAX;
  }
  for(;cit != temppad->end(); cit++)
  {
//std::cout << cit->first << " :: " << cit->second.m_weight << "\n";
     if (cit->second.m_weight < m_lfu.m_weight) { m_lfu = cit->second; }
     if (cit->second.m_weight > m_mfu.m_weight) { m_mfu = cit->second; }
  }
  delete temppad; 
  

//debug
}

/*
void
UtilityLfu::Print (std::ostream & os, const AcclContentName & name, double &value)  const
{
  Print (os, false);
  LfuData data64;
  data64.m_weight=0;
  bool exist = m_scratchpad->ExistData (name, data64);
  os << "(" << data64.m_weight << "-" << m_lfu.m_weight << "+1)/(" << m_mfu.m_weight << "-" << m_lfu.m_weight << "+1)=";
  value = Value (name);
  os << std::setprecision (15) << value;
  os << ")";

} */

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
      double ans[5]={.2,.6,1,.4,.6};
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

//std::cout << "scratchpad size: " << m_scratchpad.size() << std::endl;
  if (m_scratchpad->empty ())
    {
      return m_defaultValue;
    }
  if (1 == m_scratchpad->size ())
    {
      return 1.0;
    }
  LfuData lfu;
  bool found = m_scratchpad->ExistData (name, lfu);
  if (!found)
     return m_defaultValue; 

  double ret;
  ret = (double) (lfu.m_weight-m_lfu.m_weight+1)/(m_mfu.m_weight-m_lfu.m_weight+1); 
  //lfu should have 2 variants of calulations, one based upon value/(max-min) values, the other based upon spatial location JLM FIXME TODO
  return ret;
}


uint64_t
UtilityLfu::EstMemoryUsed (void) const
{
  return m_scratchpad->size ();    // * (sizeof(LruData) + 30 ); //EstMemoryUsed();   
}

/*
    m_valueHigh = xmlConfig.attribute("highValue").as_double(m_valueHigh);

const char_t* xml_attribute::as_string(const char_t* def = "") const;

int xml_attribute::as_int(int def = 0) const;
unsigned int xml_attribute::as_uint(unsigned int def = 0) const;
double xml_attribute::as_double(double def = 0) const;
float xml_attribute::as_float(float def = 0) const;
bool xml_attribute::as_bool(bool def = false) const;
long long xml_attribute::as_llong(long long def = 0) const;
unsigned long long xml_attribute::as_ullong(unsigned long long def = 0) const;
   */
  //step function
 //<Utility name="LRU" scaling="relative_age" | "fixed_increment" (default) />
  //both 'scaling' sets the oldest value to zero, and the newest to '1'.
  //but relative_age returns a value in ratio to that.  e.g. if the
  //oldest is 10 and the youngest is 20 timestamp, a value of 12
  //returns (12-10)/(20-10) = 0.2
  //
  //fixed increment returns an equal value, based upon how many are 
  //lower/higher than itself.
  //e.g. if 11 items are inserted, and 5 are below it, and 5 are
  //higher, it will have a value of 0.5, regardless of timestamp.

  //NOTE: FIXME TODO, what if another Data/Interest packet comes in, which was prior received, but contains different information??  e.g. Different freshess (content metadata), or different intra-nodal communications (system metadata).

UtilityLru::UtilityLru ()
  : m_useNowAsTimeLimit(false)
  , m_timeChange(false)
{
}

UtilityLru::UtilityLru (ConfigWrapper & xmlConfig)
  : m_useNowAsTimeLimit(false)
  , m_timeChange(false)
{

  Config (xmlConfig);

}

UtilityLru::~UtilityLru ()
{
  delete m_scratchpad;
}

void
UtilityLru::Config (ConfigWrapper & xmlConfig)
{
  UtilityHandlerBase::Config(xmlConfig);
  if (!m_useAlias)
    {
      m_name = IdName ();
    }
 
  m_useNowAsTimeLimit = xmlConfig.GetAttributeBool ("compareToNow", m_useNowAsTimeLimit);
  //JLM FIXME TODO, put in default
  m_scratchpad = new StorageClass < AcclContentName, LruData >;
  m_scratchpad->setStorageType (m_storageMethod);
}

void
UtilityLru::DoDelete (const AcclContentName & name)
{
  //if temporal lru and we didnt plan to update
  if (!m_timeChange) 
  {
    LruData lruTime = m_scratchpad->GetData (name);
    //check for time change
   if(  m_externalModule->GetGlobalModule ()->GetGlobalTimer ()->TimeCompare(lruTime.m_objectTimestamp,m_oldestTime) == 1) {
    //if (lruTime.m_objectTimestamp <= m_oldestTime) { 
      m_timeChange = true;
    } 
    else if ( (lruTime.m_objectTimestamp > m_newestTime) ) 
    {
      m_newestTime = lruTime.m_objectTimestamp;
    }
  } 
  m_scratchpad->EraseData (name);
}
/*void
UtilityLru::Print (std::ostream & os, const AcclContentName & name, double &value)  const
{
  Print (os, false);
  LruData data64;
  bool exist = m_scratchpad->ExistData (name, data64);
  os << "(" << data64.m_objectTimestamp << "-" << m_oldestTime << ")/(" << m_newestTime << "-" << m_oldestTime << ")=";
  value = Value (name);
  os << std::setprecision (15) << value;
  os << ")";

} */
void
UtilityLru::OnPktIngress (PktType & data)
{
  if (!((m_createEntryMask | m_updateEntryMask) & data.GetPacketPurpose ()))
    {
      return;
    }

  timer_struct_t timeStamp = m_externalModule->GetGlobalModule ()->GetGlobalTimer ()->GetTime ();
  uint64_t numEntries = m_scratchpad->size ();

  if (0 == numEntries)
    {
      m_oldestTime = timeStamp;
    }
  m_newestTime = timeStamp;
  LruData lruData, lruData2;
  const AcclContentName name = data.GetAcclName ();

  //bool valid = data.GetName(name).GetFullName();
  lruData.m_objectTimestamp = timeStamp;
  if (m_createEntryMask & data.GetPacketPurpose ())
    {
      m_scratchpad->SetData (name, lruData);
      //update for interest pkts, only if the data exists
    }
  else if (m_scratchpad->ExistData (name, lruData2))
    {
      m_scratchpad->SetData (name, lruData);
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


        //call Compute before Value, in case need to adjust values in relation to itself
void
UtilityLru::Compute (const AcclContentName & name)
{
  Compute ();
}

void
UtilityLru::Compute ()
{
  if (!m_timeChange)
    {                           //no change, no need to recalc
      return;
    }
  m_timeChange = false;

  //uint32_t position = 0; TODO FIXME change to iterator
  std::list < std::pair < dataNameType_t, LruData > >*temppad = m_scratchpad->GetDataAsList ();
  if (temppad->empty ())
    {
      delete temppad;
      temppad = NULL;
      return;
    }                           //no data
  std::list < std::pair < dataNameType_t, LruData > >::iterator it = temppad->begin ();
      //O(n)
      LruData lruTime = (it->second);
      timer_struct_t theirTime = lruTime.m_objectTimestamp;     //FIXME TODO change to time typedef

      m_oldestTime = theirTime;
      m_newestTime = theirTime;
      it++;
      for (; it != temppad->end (); it++)
        {
          lruTime = (it->second);

          theirTime = lruTime.m_objectTimestamp;
          //int comp = m_externalModule->GetGlobalModule ().GetGlobalTimer ()->TimeCompare (theirTime, m_oldestTime);

          if (theirTime < m_oldestTime) //(-1 == comp)
            {
              m_oldestTime = theirTime;
            }
          //comp = m_externalModule->GetGlobalModule ().GetGlobalTimer ()->TimeCompare (theirTime, m_newestTime);

          if (theirTime > m_newestTime) //(1 == comp)
            {
              m_newestTime = theirTime;
            }
        }
  delete temppad;

//debug
}

double
UtilityLru::Value (const AcclContentName & name) const
{

//std::cout << "scratchpad size: " << m_scratchpad.size() << std::endl;
  if (m_scratchpad->empty ())
    {
      return m_defaultValue;
    }
  if (1 == m_scratchpad->size ())
    {
      return 1.0;
    }
  LruData lruTime;
  bool found = m_scratchpad->ExistData (name, lruTime);
  if (!found)
     return m_defaultValue; 

      timer_struct_t newestTime;

      if (m_useNowAsTimeLimit)
      {
        newestTime = m_externalModule->GetGlobalModule ()->GetGlobalTimer ()->GetTime ();
      } else
      {
        newestTime = m_newestTime;
      }
      timer_struct_t pktRxTime = lruTime.m_objectTimestamp;
      double ret = m_externalModule->GetGlobalModule ()->GetGlobalTimer ()->Ratio (pktRxTime, newestTime, m_oldestTime);

      return ret;

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
  else { assert(0); }

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
  else { assert(0); }

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
{ throw std::invalid_argument("Data hashing not implemented yet!"); }


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
  /*
     std::string incrementType = xmlConfig.GetAttribute ("scaling", std::string ("temporal_increment"));  
  if (incrementType.compare ("temporal_increment"))
    {
      m_useFixedIncrement = true;
      if (!m_useAlias)
        {
          m_name.append ("_sI");
        }
    }
  else
    {                           
      m_useFixedIncrement = false;
      if (!m_useAlias)
        {
          m_name.append ("_tI");
        }
    }

  m_useNowAsTimeLimit = xmlConfig.GetAttributeBool ("nowTimeCeiling", m_useNowAsTimeLimit);
  if (!m_useAlias && m_useNowAsTimeLimit)
    {
      m_name.append ("l");
    }
  //JLM FIXME TODO, put in default
  m_scratchpad = new StorageClass < AcclContentName, LruData >;
  m_scratchpad->setStorageType (m_storageMethod);
  */
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


UtilityU32ValuationEval::UtilityValuationEval(): m_normalize(nullptr) {}
UtilityU32ValuationEval::UtilityValuationEval(ConfigWrapper &config) : m_normalize(nullptr) {
    Config(config);
}
UtilityU32ValuationEval::~UtilityValuationEval() {}
   void UtilityValuationEval::OnPktIngress (PktType &data) {
	   std::string strVal;
   bool attribExists = data.GetNamedAttribute (m_defaultAttribute, strVal);
   if (attribExists) {
      
   }
}

   void UtilityU32ValuationEval::DoDelete (const AcclContentName & name) {
      uint64_t retValue;
      bool exist = m_scratchpad->ExistData (name, retValue);
      if (exist) {
         m_scratchpad->EraseData(name); 
         if (m_normalize) {
	    m_normalize->DeleteValue(retValue);
	 }
      }
  }

   void UtilityU32ValuationEval::Config (ConfigWrapper & config) {
      UtilityHandlerBase::Config(xmlConfig);
  	if (!m_useAlias)
    	{
      	m_name = IdName ();
    	}
   
   }
   double  UtilityU32ValuationEval::Value (const AcclContentName & name) const {
     uint64_t retValue;
      bool exist = m_scratchpad->ExistData (name, retValue);
      if (exist) {
	    m_normalize->EvaluateValue(retValue);
      } else { //data not found!
          return m_defaultValue;
      }

  
  }

   uint64_t UtilityU32ValuationEval::EstMemoryUsed (void) const { return 0;}

