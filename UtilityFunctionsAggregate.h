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


#ifndef UTILITYFUNCTIONSAGGREGATE_H_
#define UTILITYFUNCTIONSAGGREGATE_H_

class PktType;
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
#include "UtilityAggregateBase.h"
#include "ContentName.h"
#include "UtilityConfigBase.h"
#include "UtilityFunctionGenerator.h"
#include "RangeData.h"

//this is for decision making between multiple options, e.g. forwarding
//each decision branch is created as a normal utility, then 'mux'
/*class UtilityMux : public UtilityAggregateBase
{
   public:
   UtilityMux (ConfigWrapper &config);
   virtual ~UtilityMux();
   static const std::string &IdName(void)
   {
      static const std::string idName("MUX");
      return idName;
   }
   double GetLastEval() const;
   std::string  GetLastIndex() const;
   SetUtilityEval(const std::string &name, UtilityHandlerBase *utilityEvals );
   SetUtilityEval(const std::string &name, ConfigWrapper &config );
   UtilityHandlerBase * GetUtilityEval(const std::string &name);
   void SetComparator(void *);  //how to compare, with doubles
   void DelUtilityEval(const std::string &name);
   void ComputeAndValue(const std::string &name, std::list < PktType> &pktList);   //void Print(std::ostream &os, const AcclContentName &tlvName, double &value, std::string &nameIndex); 
   //double FunctionValue(double param1) const;
   //double FunctionValue(double param1,param2) const;
   std::tuple<std::string, double> ValueAndIndex(const AcclContentName &name) const;
   private:
     void ComputeAndValue(std::list < PktType> &pktList) { assert(0);} 
     double Value(const AcclContentName &name) const { assert(0);}

     double m_lastEvalValue;
     std::string m_lastEvalName;
};
*/
class UtilityMax:public UtilityAggregateBase
{

public:
  UtilityMax (ConfigWrapper & config);

  virtual ~ UtilityMax ();

  static const std::string & IdName (void)
  {
    static const std::string idName ("MAX");
      return idName;
  }
protected:
    virtual double FunctionValue (double a1, double a2) const;

};


class UtilityMin:public UtilityAggregateBase
{

public:
  UtilityMin (ConfigWrapper & config);

  virtual ~ UtilityMin ();
  static const std::string & IdName (void)
  {
    static const std::string idName ("MIN");
      return idName;
  }
protected:
    virtual double FunctionValue (double a1, double a2) const;

};

class UtilitySum:public UtilityAggregateBase
{

public:
  UtilitySum (ConfigWrapper & config);

  virtual ~ UtilitySum ();

  static const std::string & IdName (void)
  {
    static const std::string idName ("SUM");
      return idName;
  }
  void Print (std::ostream & os, const AcclContentName & tlvName, double &value) const;
  void Print (std::ostream & os, bool endParen = true) const;

protected:
  virtual double FunctionValue (double a1, double a2) const;


};

class UtilityMult:public UtilityAggregateBase
{

public:
  UtilityMult (ConfigWrapper & config);

  virtual ~ UtilityMult ();
  static const std::string & IdName (void)
  {
    static const std::string idName ("MULT");
      return idName;
  };
  void Print (std::ostream & os, const AcclContentName & tlvName, double &value) const;
  void Print (std::ostream & os, bool endParen = true) const;

protected:
  virtual double FunctionValue (double a1, double a2) const;

};

 //step function
 //<Utility name="STEP_FN" lowWatermark="0.0" highWatermark="1.0" />
class UtilityStepFn:public UtilityAggregateBase
{
public:
  UtilityStepFn (ConfigWrapper & config);
  ~UtilityStepFn ();
  virtual void Config (ConfigWrapper & config);
  static const std::string & IdName (void)
  {
    static const std::string idName ("STEP_FN");
      return idName;
  };

protected:
  virtual double FunctionValue (double a1, double a2) const;
  virtual double FunctionValue (double a1) const;
  RangeData < double >m_watermarkRange;

  std::string m_rangeType;

};

class UtilityBlock:public UtilityAggregateBase
{
public:
  UtilityBlock (ConfigWrapper & config);
  ~UtilityBlock ();

  static const std::string & IdName (void)
  {
    static const std::string idName ("UTILITYBLOCK");
      return idName;
  };

protected:
  virtual double FunctionValue (double a1, double a2) const;

private:
  bool m_tempTag;
};

#endif
