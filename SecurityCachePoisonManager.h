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
#include "ModuleManager.h"
#include <set>

class SecurityCachePoisonManager:public ModuleManager
{
public:
  SecurityCachePoisonManager ();

  SecurityCachePoisonManager (ConfigWrapper & config);
  virtual ~ SecurityCachePoisonManager ();


  virtual const std::string & Name () const;

  //delete Configs by name or number

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("SecurityCachePoisonManager");
      return idName;
  }

   virtual bool OnInit (UtilityExternalModule *);

  virtual void Config (ConfigWrapper & config);

  virtual bool AddConfig (UtilityHandlerBase * uc, uint16_t position = 0);
  void OnPktIngress (PktType & interest);       //Rx
  void OnPktEgress (PktType & data, const PktTxStatus & status);        //Rx

  //store connectivity

  //default actions
private:
  //default actions
  void OnDataPktIngress (PktType & interest);
  void OnInterestPktIngress (PktType & interest);
  void OnControlPktIngress (PktType & control);
  void OnDebugPktIngress (PktType & debug);
  std::string m_statsNumDataPkt; 
  std::string m_statsNumIntPkt; 
  std::string m_statsNumIntRespPkt; 
  std::string m_statsNumCtrlPkt; 
  std::string m_statsNumDebugPkt; 
  double m_dropValue;
  //record seen interests
  std::set< std::string> m_seenInterestsBefore; 
};
