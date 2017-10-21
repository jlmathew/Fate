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
#include "StoreManager.h"

StoreManager::StoreManager ()
:m_storageLimit (0)
{
}

StoreManager::StoreManager (ConfigWrapper & config)
:m_storageLimit (0)
{
}

StoreManager::~StoreManager ()
{
}

bool
StoreManager::OnInit (UtilityExternalModule *)
{
  return true;
}

void
StoreManager::Config (ConfigWrapper & config)
{
  m_storageLimit = config.GetAttribute ("size", m_storageLimit);
  std::string storeMethod = config.GetAttribute ("storageMethod", "None");
  if (!storeMethod.compare ("None"))
    {
    }
  else if (!storeMethod.compare ("MemMap"))
    {
      m_storageMethod = Storage_MemMap;
    }
  else if (!storeMethod.compare ("DBStore"))
    {
      m_storageMethod = Storage_MemMap;
    }
  else
    {
      assert (0);
    }                           //invalid configure }
  m_name = config.GetAttribute ("name", "illegalname");
}

const std::string &
StoreManager::Name () const
{
 return m_name;
}

uint64_t 
StoreManager::GetStorageLimit () const
{
  return m_storageLimit;
}

void
StoreManager::SetStorageLimit (uint64_t limit)
{
  m_storageLimit = limit;
}

void
StoreManager::OnAsyncEvent (AsyncEvent & eventType)
{
}


void
StoreManager::OnPktEgress (PktType & data, const PktTxStatus & status)
{
}                               //Rx
void
StoreManager::OnPrePktEgress (PktType & data)
{
}  
void
StoreManager::DoDelete (const AcclContentName & name)
{
  LocalStoreDelete (name);
}

void
StoreManager::Compute ()
{
}

void
StoreManager::Compute (const AcclContentName & name)
{
}

void
StoreManager::ComputeAndValue (std::list < PktType > &pktList)
{
}


  //ICN specific
void
StoreManager::OnPktIngress (PktType & interest)
{
}                               //Rx

void
StoreManager::Print (std::ostream & os, bool endParen) const
{
}

void StoreManager::Print (std::ostream & os, const AcclContentName & tlvName, double &value) const
{
}

uint64_t StoreManager::EstMemoryUsed (void) const
{
  return 0;
}

void 
StoreManager::LocalStoreDelete (const AcclContentName & name)
{
}

double
StoreManager::Value (const AcclContentName & name) const
{
  return 0.0;              //fixme jlm
}
