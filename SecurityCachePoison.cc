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
#ifndef SECURITYCACHEPOISONMANAGER_H_
#define SECURITYCACHEPOISONMANAGER_H_
#include "SecurityCachePoisonManager.h"


SecurityCachePoisonManager::SecurityCachePoisonManager ()
{
}

SecurityCachePoisonManager::SecurityCachePoisonManager (ConfigWrapper & config)
  //: ModuleManager::ModuleManager(config)
{
  Config(config);
}

SecurityCachePoisonManager::~SecurityCachePoisonManager ()
{
}


const std::string &
SecurityCachePoisonManager::Name () const
{
  return m_name;
}

void
SecurityCachePoisonManager::Config (ConfigWrapper & config)
{
  ModuleManager::Config(config);
  if (!m_useAlias)
    {
      m_name = IdName ();
    }
}
bool
SecurityCachePoisonManager::OnInit (UtilityExternalModule * module)
{
  bool status = ModuleManager::OnInit (module);
  m_statsNumDataPkt= createHeirarchicalStatName("NumIngressDataPkts");
  m_statsNumIntPkt= createHeirarchicalStatName("NumIngressIntrestPkts");
  m_statsNumIntRespPkt= createHeirarchicalStatName("NumIngressIntrestRespPkts");
  m_statsNumCtrlPkt= createHeirarchicalStatName("NumIngressCtrlPkts");
  m_statsNumDebugPkt= createHeirarchicalStatName("NumIngressDebugPkts");
  m_stats->SetStats(m_statsNumDataPkt, (uint64_t) 0);
  m_stats->SetStats(m_statsNumIntPkt,(uint64_t) 0);
  m_stats->SetStats(m_statsNumIntRespPkt,(uint64_t) 0);
  m_stats->SetStats(m_statsNumCtrlPkt,(uint64_t) 0);
  m_stats->SetStats(m_statsNumDebugPkt,(uint64_t) 0);
  return status;
}
bool
SecurityCachePoisonManager::AddConfig (UtilityHandlerBase * uc, uint16_t position)
{
  return false;
}
void
SecurityCachePoisonManager::OnPktIngress (PktType & pkt)
{
  
        OnDataInterestPktIngress (pkt);
//std::cout << "security:" << pkt << "\n";
        //count incoming packet types
//JLM FIXME TODO below should go in moduleManager
        if (m_stats) {
  switch (pkt.GetPacketPurpose ())
    {
      case PktType::DATAPKT:
          m_stats->IncStats(m_statsNumDataPkt);
        break;
      case PktType::INTERESTPKT:
          m_stats->IncStats(m_statsNumIntPkt);
        break;
     case PktType::INTERESTRESPONSEPKT:
          m_stats->IncStats(m_statsNumIntRespPkt);
        break;
      case PktType::CONTROLPKT:
          m_stats->IncStats(m_statsNumCtrlPkt);
        break;
      case PktType::DEBUGPKT:
          m_stats->IncStats(m_statsNumDebugPkt);
        break;
      default:
        return;
        break;
    } 

    //count specific attributes, like size
   }
}

void
SecurityCachePoisonManager::OnDebugPktIngress (PktType & debug)
{
}

void
SecurityCachePoisonManager::OnControlPktIngress (PktType & control)
{
}



void
SecurityCachePoisonManager::OnDataInterestPktIngress (PktType & interest)
{
  ModuleManager::OnPktIngress (interest);       //let utilities judge it

}


void
SecurityCachePoisonManager::OnPktEgress (PktType & data, const PktTxStatus & status)
{

  ModuleManager::OnPktEgress (data, status);    //let utilities judge it
}


#endif
