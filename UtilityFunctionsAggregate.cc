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

#include "UtilityFunctionsAggregate.h"

UtilityBlock::UtilityBlock (ConfigWrapper & config):UtilityAggregateBase (config), m_tempTag (false)
{
  if (!m_useAlias)
    {
      m_name = IdName ();
    }
  //Config(config);
}

UtilityBlock::~UtilityBlock ()
{

}


double
UtilityBlock::FunctionValue (double a1, double a2) const
{
  throw std::invalid_argument ("UtilityBlock is only valid for a single parameter");
}

  //array< std::pair< name, double > > *
  //UtilityBlock::GetLowestNValues(uint32_t lowestN) {}
  //
  //array< std::pair< name, double > > *
  //UtilityBlock::GetLowestThresholdValues(double thresholdValue) {}

UtilityStepFn::UtilityStepFn (ConfigWrapper & config):
UtilityAggregateBase (config),
m_watermarkRange (0.0, 0.5),
m_rangeType ("LeftRightInclusive")
{
  if (!m_useAlias)
    {
      m_name = IdName ();
    }

  Config (config);
}

UtilityStepFn::~UtilityStepFn ()
{
}





void
UtilityStepFn::Config (ConfigWrapper & config)
{
  m_watermarkRange.SetLowerBound (config.GetAttribute ("thresholdLow", m_watermarkRange.getLowerBound ()));
  m_watermarkRange.SetUpperBound (config.GetAttribute ("thresholdHigh", m_watermarkRange.getUpperBound ()));
  m_rangeType = config.GetAttribute ("inclusiveRange", m_rangeType);
  m_watermarkRange.SetRangeCompare (m_rangeType);
}

double
UtilityStepFn::FunctionValue (double a1, double a2) const
{
  throw std::invalid_argument ("UtilityStepFn is only valid for a single parameter");
  return FunctionValue (a1);       //not really defined ..., avoid compiler warning
}

double 
UtilityStepFn::FunctionValue (double a1) const
{
  if (m_watermarkRange.IsInRange (a1))
  {
    return 1.0;
  }
  return 0.0;

}


UtilitySum::UtilitySum (ConfigWrapper & config):UtilityAggregateBase (config)
{
  if (!m_useAlias)
    {
      m_name = IdName ();
    }

}

UtilitySum::~UtilitySum ()
{
}


void
UtilitySum::Print (std::ostream & os, bool endParen)  const
{
       if (!m_usePrettyPrint)
         {
           UtilityAggregateBase::Print (os);
           return;
         }
       int i = 1;

       std::list < UtilityHandlerBase * >::const_iterator it = functionalFactors.begin ();

       int end = functionalFactors.size ();

       os << "(";
       while (it != functionalFactors.end ())
         {
           (*it)->Print (os);
           if (end != i)
             {
               os << " + ";
             }
           i++;
           it++;
         }
       os << ")";

}

void
UtilitySum::Print (std::ostream & os, const AcclContentName & Name, double &value) const
{
       if (!m_usePrettyPrint)
         {
           UtilityAggregateBase::Print (os);
           return;
         }
       int i = 1;

       std::list < UtilityHandlerBase * >::const_iterator it = functionalFactors.begin ();

       int end = functionalFactors.size ();

       os << "(";
       double childValue = 0.0;

       if (it != functionalFactors.end ())
         {
           (*it)->Print (os, Name, value);
           it++;
           if (end != i)
             {
               os << " + ";
             }
           i++;
         }
       while (it != functionalFactors.end ())
         {
           (*it)->Print (os, Name, childValue);
           value = FunctionValue (value, childValue);
           if (end != i)
             {
               os << " + ";
             }
           i++;
           it++;
         }
       os << "=[";
       os << value;
       os << "])";


     }

double
     UtilitySum::FunctionValue (double a1, double a2) const
     {
       return a1 + a2;
     }

 //Multiply
UtilityMult::UtilityMult (ConfigWrapper & config):
UtilityAggregateBase (config)
{
  if (!m_useAlias)
    {
      m_name = IdName ();
    }

}

UtilityMult::~UtilityMult ()
{
}


void
     UtilityMult::Print (std::ostream & os, bool endParen)  const
     {
       if (!m_usePrettyPrint)
         {
           UtilityAggregateBase::Print (os);
           return;
         }
       int i = 1;

       std::list < UtilityHandlerBase * >::const_iterator it = functionalFactors.begin ();

       int end = functionalFactors.size ();

       os << "(";
       while (it != functionalFactors.end ())
         {
           (*it)->Print (os);
           if (end != i)
             {
               os << "*";
             }
           i++;
           it++;
         }
       if (endParen)
         {
           os << ")";
         }

     }

void
     UtilityMult::Print (std::ostream & os, const AcclContentName & tlvName, double &value) const
     {

       if (!m_usePrettyPrint)
         {
           UtilityAggregateBase::Print (os);
           return;
         }
       int i = 1;

       std::list < UtilityHandlerBase * >::const_iterator it = functionalFactors.begin ();

       int end = functionalFactors.size ();

       os << "(";
       double childValue = 0.0;

       if (it != functionalFactors.end ())
         {
           (*it)->Print (os, tlvName, value);
           it++;
           if (end != i)
             {
               os << "*";
             }
           i++;
         }
       while (it != functionalFactors.end ())
         {
           (*it)->Print (os, tlvName, childValue);
           value = FunctionValue (value, childValue);
           if (end != i)
             {
               os << "*";
             }
           i++;
           it++;
         }
       os << "=[";
       os << value;
       os << "])";


     }


double
     UtilityMult::FunctionValue (double a1, double a2) const
     {
       return a1 * a2;
     }


 //Min
UtilityMin::UtilityMin (ConfigWrapper & config):
UtilityAggregateBase (config)
{
  if (!m_useAlias)
    {
      m_name = IdName ();
    }

}

UtilityMin::~UtilityMin ()
{
}



double
     UtilityMin::FunctionValue (double a1, double a2) const
     {
       if (a1 < a2)
         {
           return a1;
         }
       else
         {
           return a2;
         }
     }


 //Max
UtilityMax::UtilityMax (ConfigWrapper & config):UtilityAggregateBase (config)
{
  std::string tmpName = IdName ();
  tmpName.append (m_name);
  m_name = tmpName;

}

UtilityMax::~UtilityMax ()
{
}



double
UtilityMax::FunctionValue (double a1, double a2) const
{
  if (a1 < a2)
    {
      return a2;
    }
  else
    {
      return a1;
    }
}

//add if/then/else
//add switch/case

