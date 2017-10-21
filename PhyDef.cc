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
#include "PhyDef.h"
#include <vector>

PhyDef::PhyDef() {}
PhyDef::~PhyDef() {}
PhyDef::PhyDef(const PhyDef &phy) {}
   double PhyDef::GetSpeedMps() const { return 0.0;}
   void PhyDef::SetSpeedMps(double speed){}
   void PhyDef::SetMedium(PropMedium_t medium){}
   PropMedium_t PhyDef::GetMedium() const{ return PropMedium_t::UNKNOWN;}
   void PhyDef::SetSpecificPhyType(PhyType_t type){}
   PhyType_t PhyDef::GetSpecificPhyType(){ return PhyType_t::UNKNOWN;}
   double PhyDef::GetFreq() const{return 0.0;}
   void PhyDef::SetFreq(double){}


   std::string NetworkPhy::GetPhyName() const{ return "";}
   void  NetworkPhy::SetPhyName(const std::string &name){}
   bool  NetworkPhy::IsUp() const{ return false;}
   PhyDef *  NetworkPhy::GetPhy() const{ return nullptr;}
   void  NetworkPhy::SetPhy(const PhyDef *phy){}
   //user defined
   double  NetworkPhy::GetBitErrorRate() const{ return 0.0;}
   void  NetworkPhy::SetBitErrorRate(double rate){}
   uint32_t  NetworkPhy::GetNumNeighbors() const{ return 0.0;}
   void  NetworkPhy::SetNumNeighbors(uint32_t numNeighbor){}
   double  NetworkPhy::GetTimeUpRatio(){return 0.0;}
   void  NetworkPhy::SetTimeUpRatio(double ratio){}

