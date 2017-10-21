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
#ifndef __PHYDEF__
#define __PHYDEF__

#include <cstdint>
#include <vector>
#include <string>
//make a Phy definition

//generic how the signal is propagated
enum class PropMedium_t : uint8_t {UNKNOWN,FIBER,COPPER,AIR};

//actually manufactured physical type-properties
enum class PhyType_t : uint16_t {UNKNOWN,SERIALCABLE,BASE10T,BASE100T,BASE1000T,W802_11_G,W802_11_N,W802_11_B};

//Constants for a physical defintion
class PhyDef 
{
public:
   PhyDef();
   virtual ~PhyDef();
   PhyDef(const PhyDef &phy);
   double GetSpeedMps() const ;
   void SetSpeedMps(double speed);
   void SetMedium(PropMedium_t medium);
   PropMedium_t GetMedium() const;
   void SetSpecificPhyType(PhyType_t type);
   PhyType_t GetSpecificPhyType();
   //need more properties,like width
   //assume same value freq interfere, e.g. 2.4ghz wifi interfers with 2.4ghz bluetooth
   //assume different values dont interfere (clear channels), e.g. 2.42ghz wifi wont interfere with 2.4ghz blutooth.
   //this is to avoid using both bluetooth and wifi at the same time.
   double GetFreq() const;
   void SetFreq(double);

private:
  double m_speed;
  PropMedium_t m_medium;
  PhyType_t type;
};


class NetworkPhy {
   std::string GetPhyName() const;
   void SetPhyName(const std::string &name);
   bool IsUp() const;
   PhyDef * GetPhy() const;
   void SetPhy(const PhyDef *phy);
   //user defined
   double GetBitErrorRate() const;
   void SetBitErrorRate(double rate);
   uint32_t GetNumNeighbors() const;
   void SetNumNeighbors(uint32_t numNeighbor);
   double GetTimeUpRatio();
   void SetTimeUpRatio(double ratio);
  private:
    PhyDef m_cPhy;
};

#endif
