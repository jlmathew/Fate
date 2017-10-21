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


#ifndef EVENTHANDLERBASE_H_
#define EVENTHANDLERBASE_H_

#include<list>
#include "ContentName.h"
#include "PacketTypeBase.h"

class UtilityExternalModule;
class ConfigWrapper;


//doxygen

/**
  *
  *
  */
class EventHandlerBase
{
public:

  virtual void ComputeAndValue (std::list < PktType > &pktList) = 0;

  //Interest, Data, Discovery, Interest Response
  virtual void OnPktIngress (PktType & pkt) = 0;
  virtual void OnPktEgress (PktType & pkt, const PktTxStatus & status) = 0;
  virtual void OnPrePktEgress(PktType &data) = 0;

  virtual void DoDelete (const AcclContentName & name) = 0;
  virtual bool OnInit (UtilityExternalModule *) = 0;

  virtual void Config (ConfigWrapper & config) = 0;

  virtual void Compute (const AcclContentName & name) = 0;

  virtual void Compute () = 0;

  virtual double Value (const AcclContentName & name) const = 0;

  virtual uint64_t EstMemoryUsed (void) const = 0;

  virtual void Print (std::ostream & os, bool endParen = true) const = 0;

  virtual void Print (std::ostream & os, const AcclContentName & tlvName, double &value) const = 0;

  virtual const dataNameType_t & Name () const = 0;

  //add new event types here
};

#endif
