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


#ifndef UTILITYAGGREGATEBASE_H_
#define UTILITYAGGREGATEBASE_H_

class UtilityExternalModule;

#include <cstdlib>
#include <stdint.h>
#include <ostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <list>
#include <exception>
#include <vector>
#include <map>
#include "UtilityHandlerBase.h"
#include "ContentName.h"
#include "PacketTypeBase.h"
#include "UtilityConfigXml.h"
#include "UtilityFunctionGenerator.h"
#include "EventHandlerBase.h"

class UtilityAggregateBase:public UtilityHandlerBase
{
protected:
  UtilityAggregateBase ();
public:

  /**
    *
    * \brief 
    * \param ConfigWrapper
    * \see Config()
    *
    */
  UtilityAggregateBase (ConfigWrapper & config);   
  virtual ~ UtilityAggregateBase ();

  /**
    *
    *
    * \param
    * \return 
    */
  virtual bool OnInit (UtilityExternalModule *);

  void ComputeAndValue (std::list < PktType > &pktList);

  virtual void Config (ConfigWrapper & config);

  virtual void Compute (const AcclContentName & name);

  virtual void Compute ();

  virtual double Value (const AcclContentName & name) const;

  virtual uint64_t EstMemoryUsed (void) const;

  virtual void Print (std::ostream & os, bool endParen = true) const;

  virtual void Print (std::ostream & os, const AcclContentName & tlvName, double &value) const;

  virtual void DoDelete (const AcclContentName &);

  static const std::string & IdName ()
  {
    throw::std::invalid_argument ("Aggregate has no id name");
  };

  virtual void OnPktIngress (PktType & data);   //Rx
  virtual void OnPktEgress (PktType & data, const PktTxStatus & status);        //Rx
virtual void
OnPrePktEgress (PktType & pkt);
   virtual bool SelfTest(void);

protected:
  std::list < UtilityHandlerBase * >functionalFactors;
  virtual double FunctionValue (double param1) const;
  virtual double FunctionValue (double param1, double param2) const;


};

#endif
