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
#include "UtilityAggregateBase.h"

UtilityAggregateBase::UtilityAggregateBase ()
{
}
UtilityAggregateBase::UtilityAggregateBase (ConfigWrapper & config)
  :UtilityHandlerBase::UtilityHandlerBase (config)
{
  Config (config);
}

void
UtilityAggregateBase::Config (ConfigWrapper & config)
{
  ConfigWrapper *childConfig = config.GetFirstChildUtility ("Utility");

  while (childConfig->valid ())
    {
      functionalFactors.push_back (UtilityGenerator::CreateNewUtility (*childConfig));
      childConfig->GetNextChildUtility ("Utility");

      //std::cout << "utility-" << childConfig->GetAttribute("name", "notfound") << std::endl;
    }

   delete childConfig;
}

UtilityAggregateBase::~UtilityAggregateBase ()
{
  
  std::list < UtilityHandlerBase * >::iterator it = functionalFactors.begin();
  for(;it != functionalFactors.end(); it++) {
     delete (*it);
  }
  functionalFactors.clear ();
}

bool
UtilityAggregateBase::SelfTest ()
{
  std::list < UtilityHandlerBase * >::iterator it = functionalFactors.begin ();
  bool success = true;

  while (it != functionalFactors.end ())
    {
      success &= (*it)->SelfTest ();
      it++;
    }
  return success;
}


bool
UtilityAggregateBase::OnInit (UtilityExternalModule * blockDataPtr)
{
  std::list < UtilityHandlerBase * >::iterator it = functionalFactors.begin ();
  bool success = true;

  while (it != functionalFactors.end ())
    {
      success &= (*it)->OnInit (blockDataPtr);
      it++;
    }
  return success;
}

double
     UtilityAggregateBase::Value (const AcclContentName & name) const
     {
       std::list < UtilityHandlerBase * >::const_iterator it = functionalFactors.begin ();
       double value = 1.0;
       double secValue;

         value = (*it)->Value (name);
       if (it != functionalFactors.end ())
         {
           it++;
         }
       while (it != functionalFactors.end ())
         {
           secValue = (*it)->Value (name);
           value = FunctionValue (secValue, value);
           it++;
         }
       return value;
     }

void
UtilityAggregateBase::DoDelete (const AcclContentName & name)
{
  std::list < UtilityHandlerBase * >::iterator it = functionalFactors.begin ();
  while (it != functionalFactors.end ())
    {
      (*it)->DoDelete (name);
      it++;
    }
}


void
UtilityAggregateBase::OnPktEgress (PktType & pkt, const PktTxStatus & status)
{
  std::list < UtilityHandlerBase * >::iterator it = functionalFactors.begin ();
  while (it != functionalFactors.end ())
    {
      (*it)->OnPktEgress (pkt, status);
      it++;
    }
}
void
UtilityAggregateBase::OnPrePktEgress (PktType & pkt)
{
  std::list < UtilityHandlerBase * >::iterator it = functionalFactors.begin ();
  while (it != functionalFactors.end ())
    {
      (*it)->OnPrePktEgress (pkt);
      it++;
    }
}

void
UtilityAggregateBase::OnPktIngress (PktType & pkt)
{
  std::list < UtilityHandlerBase * >::iterator it = functionalFactors.begin ();
  while (it != functionalFactors.end ())
    {
      (*it)->OnPktIngress (pkt);
      it++;
    }
}

uint64_t
     UtilityAggregateBase::EstMemoryUsed (void) const
     {
       uint64_t sum = 0;

       std::list < UtilityHandlerBase * >::const_iterator it = functionalFactors.begin ();
       while (it != functionalFactors.end ())
         {
           sum += (*it)->EstMemoryUsed ();
           it++;
         }
       return sum;
     }

void
UtilityAggregateBase::ComputeAndValue (std::list < PktType > &pktList)
{
  std::list < UtilityHandlerBase * >::iterator it = functionalFactors.begin ();
  while (it != functionalFactors.end ())
    {
      (*it)->ComputeAndValue (pktList);
      it++;
    }

}

void
UtilityAggregateBase::Compute ()
{
  std::list < UtilityHandlerBase * >::iterator it = functionalFactors.begin ();
  while (it != functionalFactors.end ())
    {
      (*it)->Compute ();
      it++;
    }

}

void
UtilityAggregateBase::Compute (const AcclContentName & name)
{
  std::list < UtilityHandlerBase * >::iterator it = functionalFactors.begin ();
  while (it != functionalFactors.end ())
    {
      (*it)->Compute (name);
      it++;
    }

}

void
     UtilityAggregateBase::Print (std::ostream & os, bool endParen) const
     {
       if (m_useAlias & !m_useProxyName)
         {
           UtilityHandlerBase::Print (os, endParen);
           return;
         }

       std::list < UtilityHandlerBase * >::const_iterator it = functionalFactors.begin ();

       //FIXME TODO check for alias, dont go deeper if it exists
       os << Name ();
       os << "[";

       if (it != functionalFactors.end ())
         {
           (*it)->Print (os, endParen);
           it++;
         }
       while (it != functionalFactors.end ())
         {
           os << ", ";
           (*it)->Print (os, endParen);
           it++;
         }
       if (endParen)
         {
           os << "]";
         }

     }

void
     UtilityAggregateBase::Print (std::ostream & os, const AcclContentName & tlvName, double &value) const
     {
       if (m_useAlias)
         {
           UtilityHandlerBase::Print (os, tlvName, value);
           return;
         }

       std::list < UtilityHandlerBase * >::const_iterator it = functionalFactors.begin ();

       os << Name ();
       os << "[";
       double childValue = 0.0;

       if (functionalFactors.size () == 1)
         {
           (*it)->Print (os, tlvName, value);
           value = FunctionValue (value);
         }
       else
         {
           if (it != functionalFactors.end ())
             {
               (*it)->Print (os, tlvName, value);
               it++;
             }
           while (it != functionalFactors.end ())
             {
               os << ", ";
               (*it)->Print (os, tlvName, childValue);
               it++;
               value = FunctionValue (value, childValue);
             }

         }
       os << "]={";             //output.append(" ]=");
       os << value;             //std::ostringstream strs;
       os << "}";

     }

double
     UtilityAggregateBase::FunctionValue (double param1) const
     {
       return param1;
     }

     double UtilityAggregateBase::FunctionValue (double param1, double param2) const
     {
       throw std::invalid_argument ("UtilityAggregateBase::FunctionValue should never be called");
         return 0.0;
     }                          //should never be called


     double UtilityAggregateBase::FunctionValue (double param1, double param2, double param3) const
     {
         throw std::invalid_argument ("UtilityAggregateBase::FunctionValue should never be called");
         return 0.0;
     }                          //should never be called


