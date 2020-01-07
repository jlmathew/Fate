/*
MIT (Modified) License

Copyright (c) 2018 jlmathew

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
#include "ForwardBasicManagerPktQueue.h"


ForwardBasicManagerPktQueue::ForwardBasicManagerPktQueue ()
{
}

ForwardBasicManagerPktQueue::ForwardBasicManagerPktQueue (ConfigWrapper & config)
 // : ModuleManager::ModuleManager(config)
{
  Config(config);
}

ForwardBasicManagerPktQueue::~ForwardBasicManagerPktQueue ()
{
}

void
ForwardBasicManagerPktQueue::Config (ConfigWrapper & config)
{
   m_delayConfig = config.Clone(); //FIXME  
//  ModuleManager::Config(config);
//  if (!m_useAlias)
//    {
//      m_name = IdName ();
//    }
}


bool
ForwardBasicManagerPktQueue::OnInit (UtilityExternalModule * module)
{

 bool status =  ModuleManager::OnInit (module);
std::set<std::string> phyNames;
  //loop through known phy names
  ModuleManager::Config(*m_defaultConfig, &phyNames);
  if (!m_useAlias)
    {
      m_name = IdName ();
    }
 
  return status;
}
bool
ForwardBasicManagerPktQueue::AddConfig (UtilityHandlerBase * uc, uint16_t position)
{
  return false;
}

void ForwardBasicManagerPktQueue::OnPktIngress (PktType & pkt)
{
  switch (pkt.GetPacketPurpose ())
    {
      case PktType::DATAPKT:
      case PktType::INTERESTPKT:
        OnDataInterestPktIngress (pkt);
        break;
      case PktType::CONTROLPKT:
        OnControlPktIngress (pkt);
        break;
      case PktType::DEBUGPKT:
        OnDebugPktIngress (pkt);
        break;
      default:
        return;
        break;
    }
}

void
ForwardBasicManagerPktQueue::OnDebugPktIngress (PktType & debug)
{
}

void
ForwardBasicManagerPktQueue::OnControlPktIngress (PktType & control)
{
}

void
ForwardBasicManagerPktQueue::ForwardPkts(std::list < PktType > &pktList)
{
    if (0 == m_utilityEval.size())
       return;

    Compute();
    //dont choose expired packets
    //RangeData < double >valueRange (0.0, m_dropValue);
    //choose each phy
  std::vector < UtilityHandlerBase * >::const_iterator it = m_utilityEval.begin ();

  typedef std::tuple<double, PktType, std::string> fwdTuple_t;
  std::list<fwdTuple_t> fwdPerPhy[m_utilityEval.size()];
  uint32_t cnt=0;
  while (it != m_utilityEval.end ())
    {
       //auto it2 = m_pendingTxPkts.begin();
          //while (it2 != m_pendingTxPkts.end()) {
            //auto it3 = neighborPerPhy.begin();
              //while (it3 != neighborPerPhy.end()) {
              //choose all 1-hop neighbors per phy
              //eval each pkt against each 1-hop neighbor
              //double val = EvalFwdPkts (name, it, neighbor);
              //add temp value to packet in phy if in range
              //fwdPhyp[cnt].push_back(std::make_tuple(val, PktType, *it3));
              //store in per phy queue
              //it3++;
            //}
            
            //it2++;
          //}
          it++;
          cnt++;
    }

   //
  
}


void
ForwardBasicManagerPktQueue::OnDataInterestPktIngress (PktType & interest)
{ 
  ModuleManager::OnPktIngress (interest);       //let utilities judge it
    bool unicast = true; //need method to detect if multi/uni
   if (unicast) {
      //EnqueueUnicastPkt(interest);
   } else {
      //EnqueueMulticastPkt(interest);
   }
}

void ForwardBasicManagerPktQueue::EnqueueUnicastPkt( PktType &data) {
    /*m_unicast_queue.pushback(data);
    if (all interfaces are not busy)
 	TxPkts();*/

}
void ForwardBasicManagerPktQueue::EnqueueMulticastPkt(PktType &data) {
    //need to pair with a list of destinations
    //m_multicast_queue.pushback(data);
}


void
ForwardBasicManagerPktQueue::PktSent(PktType &data, PktTxStatus &status, uint32_t interface)
{
   TxPkts();
}
//conditions
//return prior packet and status
void
ForwardBasicManagerPktQueue::TxPkts ()
{

 /* m_node->notifyPkt(data, status);
  //evaluate packet type, evaluate and 'send' it to the wire
  for_each (list of interfaces)
	  if interface is busy, continue //save processing
	 for_each (neighbor list per interface)
	      getValuation; 
 		save highestest value : interface : neighbor pair IF meet minimum criteria

   //napsack problem	
*/	       	
 // ModuleManager::OnPktEgress (data, status);    //let utilities judge it

}


