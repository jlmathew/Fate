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
#ifndef SECURITYBASICMANAGER_H_
#define SECURITYBASICMANAGER_H_
#include "PreCacheManager.h"
#include <list>

PreCacheManager::PreCacheManager ()
  : m_storageLimit(0)
{
}

PreCacheManager::PreCacheManager (ConfigWrapper & config)
  //: ModuleManager::ModuleManager(config)
  : m_storageLimit(0)
{
  Config(config);
}

PreCacheManager::~PreCacheManager ()
{
}


const std::string &
PreCacheManager::Name () const
{
  return m_name;
}

void
PreCacheManager::Config (ConfigWrapper & config)
{
  ModuleManager::Config(config);
  if (!m_useAlias)
    {
      m_name = IdName ();
    }
  config.GetAttribute("waterMark", m_storageLimit);
}

bool
PreCacheManager::OnInit (UtilityExternalModule * module)
{
  bool status = ModuleManager::OnInit (module);
  return status;
}
bool
PreCacheManager::AddConfig (UtilityHandlerBase * uc, uint16_t position)
{
  return false;
}
void
PreCacheManager::OnPktIngress (PktType & pkt)
{
  
        OnDataInterestPktIngress (pkt);
//std::cout << "security:" << pkt << "\n";
        //count incoming packet types
//JLM FIXME TODO below should go in moduleManager
        if (m_stats) {
  switch (pkt.GetPacketPurpose ())
    {
      case PktType::DATAPKT:
        break;
      case PktType::INTERESTPKT:
        break;
     case PktType::INTERESTRESPONSEPKT:
        break;
      case PktType::CONTROLPKT:
        break;
      case PktType::DEBUGPKT:
        break;
      default:
        return;
        break;
    } 

    //count specific attributes, like size
   }
}

void
PreCacheManager::OnDebugPktIngress (PktType & debug)
{
}

void
PreCacheManager::OnControlPktIngress (PktType & control)
{
}



void
PreCacheManager::OnDataInterestPktIngress (PktType & interest)
{
  ModuleManager::OnPktIngress (interest);       //let utilities judge it

    //Delete extra up to lower watermark
    if (m_PktNames.size() > m_storageLimit) {
        std::list < std::pair<double, AcclContentName>> list;
        GetLowestNPackets(1, list);
	while (! list.empty()) {
	  auto it = list.begin();
          m_PktNames.erase(it->second);
	}
    }
    Compute(interest.GetAcclName());
    interest.SetNamedAttribute("LFU-EVAL",Value(interest.GetAcclName()),true); //temp
      
}


void
PreCacheManager::OnPktEgress (PktType & data, const PktTxStatus & status)
{

  ModuleManager::OnPktEgress (data, status);    //let utilities judge it
}


#endif
