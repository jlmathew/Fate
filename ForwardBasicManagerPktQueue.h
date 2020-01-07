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
#ifndef FORWARDMANAGERBASEPKTQUEUE_H_
#define FORWARDMANAGERBASEPKTQUEUE_H_

#include "ModuleManager.h"
#include <tuple>

class ForwardBasicManagerPktQueue:public ModuleManager
{
public:
  ForwardBasicManagerPktQueue ();

  ForwardBasicManagerPktQueue (ConfigWrapper & config);
  virtual ~ ForwardBasicManagerPktQueue ();



  //virtual const std::string & Name () const;
  virtual void Config (ConfigWrapper & config);
  virtual void ForwardPkts(std::list < PktType > &pktList);
   static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("ForwardBasicManagerPktQueue");
      return idName;
  }
   virtual bool OnInit (UtilityExternalModule *);

  virtual bool AddConfig (UtilityHandlerBase * uc, uint16_t position = 0);
  void OnPktIngress (PktType & interest);       //Rx
  void OnPktEgress (PktType & data, const PktTxStatus & status);  
  //store connectivity

  //default actions
  //
  //pkt actions
  void EnqueueUnicastPkt(PktType &pkt);
  void EnqueueMulticastPkt(PktType &pkt);
  
  void CallbackTxUnicast();
  void CallbackTxMulticast();
  void TxPkts();
  void PktSent(PktType &data, PktTxStatus &status, uint32_t interface);
private:
  //default actions
  void OnDataInterestPktIngress (PktType & interest);
  void OnControlPktIngress (PktType & control);
  void OnDebugPktIngress (PktType & debug);
  ConfigWrapper *m_delayConfig;
  //uint64_t m_statsMiss;
 // uint64_t m_statsHit;
  //uint64_t m_statsHitExpired;

  //double m_dropValue;

  std::vector< PktType > m_multicast_queue;
  std::vector< PktType > m_unicast_queue;


};

#endif
