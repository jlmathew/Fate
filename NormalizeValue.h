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
#ifndef NORMALIZED_VALUES_H_
#define NORMALIZED_VALUES_H_


#include <algorithm>
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
#include <set>
#include "RangeData.h"
#include <stdexcept>
#include "ContentName.h"
#include "UtilityConfigBase.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <cfloat>
#include <cfenv>
#include <type_traits>

template<class T>
class Normalize
{
public:
  Normalize(): m_divisor(1) { 
  
    if (std::is_floating_point<T>::value)
       { m_retMinType= std::nextafter(0.0,1.0); }
    else if (std::is_integral<T>::value )
          { m_retMinType=1.0; }
    else 
    {}
  }
  Normalize(ConfigWrapper &config) {
    Config(config);
  }
  virtual ~Normalize()
  {
  }
  virtual void Print(std::ostream &os) const {
    os << "_N_";
  }
  virtual void Config(ConfigWrapper &config) {
     m_divisor = config.GetAttribute("divisor", m_divisor);

  }
  virtual void InsertValue(T a)
  {
  }

  virtual void Clear() {}
  virtual bool DeleteValue(T a)
  {
    return true;
  }

  virtual
  double EvaluateValue(T) {
    return 0.0;
  }
  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("Normalized");
    return idName;
  }
  protected:
  double m_retMinType;
  uint64_t m_divisor;

};

template<class T>
class NormalizeValue : public Normalize<T>
{
public:
  NormalizeValue() {}
  NormalizeValue(ConfigWrapper &xml) {
    Config(xml);
  }
  virtual ~NormalizeValue()
  {
    m_values.clear();
  }

  void Clear() { m_values.clear(); }
  void Config(ConfigWrapper &config) {
    Normalize<T>::Config(config);
  }
  void Print(std::ostream &os) const {
    os << "_Nv_";
  }
  void InsertValue(T a)
  {
    m_values.insert(a);
  }
  bool DeleteValue(T a)
  {
    typename std::multiset<T>::iterator it;
    it = m_values.find(a);
    bool ret = true;
    if (it != m_values.end())
    {
      m_values.erase(it);
    } else {
      ret=false;
    }
    return ret;
  }

  double EvaluateValue(T) {
    throw std::invalid_argument( "baseclass called, should be a derived class" );
    return 0.0;
  }
  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("NormalizeValue");
    return idName;
  }

protected:
  std::multiset<T> m_values;
  T GetMinimum();
  T GetMaximum();

};

//evaluate if RangeData in range (A,C)
//used for impulse or QoS value matching
template <class T>
class NormalMatch : public Normalize<T>
{
public:
  NormalMatch() {}
  ~NormalMatch() {}
  NormalMatch(ConfigWrapper &config) {
    Config(config);
  }

  void Config(ConfigWrapper &config) {
    Normalize<T>::Config(config);
    double m_minValue = 0.0;
    double m_maxValue = 0.0;
    m_minValue = config.GetAttribute("matchLow", m_minValue);
    m_maxValue = config.GetAttribute("matchHigh", m_maxValue);
    RangeData<T> confMatch(static_cast<T>(m_minValue),static_cast<T>(m_maxValue));
    InsertMatchValue(confMatch);
  }
  void Print(std::ostream &os) const {
    os << "_Nm" << m_data ;
  }

  void
  InsertMatchValue(const RangeData<T> &match)
  {
    m_data=match;
  }
  double EvaluateValue(T val)
  {
    if (m_data.IsInRange(val)) {
      return 1.0;
    } else
      return 0.0;
  }
  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("NormalMatch");
    return idName;
  }

private:
  RangeData<T> m_data;
};

// 1/n valuation
// option to reduce, e.g. 5,19,24, becomes 1/(5-5+1), 1/(19-5+1), 1/(24-5+1)
template <class T>
class GeometricRanked : public NormalizeValue<T>
{
public:
  GeometricRanked(): m_invert(false) {};
  GeometricRanked(ConfigWrapper &config): m_invert(false), m_bias(false) {
    Config(config);
  };
  ~GeometricRanked() {};
  void SetInvert(bool inv) {
    m_invert = inv;
  }
  void Config(ConfigWrapper &config) {
    Normalize<T>::Config(config);
    m_invert = config.GetAttributeBool ("invertValue", m_invert);
    m_bias = config.GetAttributeBool ("biasLowVal", m_bias);
  }
  void Print(std::ostream &os) const {
    os << "_Ngi" << m_invert << "b" << m_bias;
  }

  //best if VAL is integer only, close FP numbers may give > 1
  double EvaluateValue(T val)
  {
    double ret = 1.0;
    if (this->m_values.empty())
      return 0.0;
    T min = *(this->m_values).cbegin(); //need something better?
    if (m_bias) {
        //ret = 1.0/(val-min+Normalize<T>::m_retMinType);
        //ret = 1.0/(val-min+this->m_retMinType);
        ret = (double) this->m_divisor/(val-min+this->m_retMinType);
        //ret = 1.0/(val-min+1.0);
    } else {
      if (val != 0.0) {
	ret = (double) this->m_divisor/val;
         if (ret > 1.0) { ret = 1.0; }
	//ret = 1.0/val;
      } else { ret = 1.0; }
    }
    if (m_invert) {
      ret = 1.0-ret;
    }
    assert(ret <= 1.0); 
    return ret;
  }
  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("GeometricMatch");
    return idName;
  }

private:
  bool m_invert;
  bool m_bias;
};

//normalized from lowest to highest value; 0.0 .33, .5, .8 1.0
template <class T>
class NormalRanked : public NormalizeValue<T>
{
public:
  enum normalRankedValueOption_t : uint16_t
  {
    inValid,
    floor,
    ceiling,
    fullRange
  };
  NormalRanked(): m_strVal("ceiling"), m_valOption(ceiling), m_invert(false) {}
  NormalRanked(ConfigWrapper &config):m_strVal("ceiling"), m_valOption(ceiling),m_invert(false)  {
    Config(config);
  }
  virtual ~NormalRanked() { }
  void Config(ConfigWrapper &config) {
    NormalizeValue<T>::Config(config);

    
    m_invert = config.GetAttributeBool ("invertValue", m_invert);

    m_strVal = config.GetAttribute("value_type", m_strVal);

    if (m_strVal == "ceiling") {
      m_valOption = ceiling;
    }
    else if (m_strVal == "floor") {
      m_valOption = floor;
    }
    else if (m_strVal == "fullRange") {
      m_valOption = fullRange;
    } else { throw("Unknown rnage type"); }

  }
  void Print(std::ostream &os) const {
    os << "_Nri" << m_invert << "o" << m_valOption;
  }
  double EvaluateValue(T val)
  {
    //should we check if the provided value exists?
    //if not exist, should we throw an exception
    if (this->m_values.empty())
      return 0.0;
    T min = *(this->m_values).cbegin(); //need something better?
    T max = *(this->m_values).crbegin();
    if (min == max)
      return 1.0;
    double typeval=1.0;
    switch (m_valOption) {
    case floor:  //can be 0.0
      typeval = (double) (static_cast<double>(val-min))/(max-min);
      break;
    case ceiling:  // can NOT be 0.0, better for integer values
      typeval = (double) (static_cast<double>(val-min)+this->m_retMinType)/(max-min+this->m_retMinType);
      //typeval = (double) (static_cast<double>(val-min)+Normalize<T>::m_retMinType)/(max-min+Normalize<T>::m_retMinType);
      //typeval = (double) (static_cast<double>(val-min)+1.0)/(max-min+1.0);
      break;
    case fullRange:  // can NOT be 0.0
      typeval = (double) (val)/(max);
      break;
      //FIXME TODO inverse should be separate property
    default:
      throw ("Unknown value option type");
    }
    if (m_invert) {
      typeval = 1.0-typeval;
    }
    assert(typeval<=1);
    return typeval;
  }

  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("NormalRanked");
    return idName;
  }

private:
  std::string m_strVal;
  normalRankedValueOption_t m_valOption;
  bool m_invert;
};


//step normalized from 0 to 1 (0, .33, .66, 1.0)
template <class T>
class StepRanked: public NormalizeValue<T>
{
public:
  StepRanked() : m_rankHigh(true), m_invert(true) {}
  StepRanked(ConfigWrapper &config) : m_rankHigh(true), m_invert(true) {
    Config(config);
  }
  ~StepRanked() { }
  void Config(ConfigWrapper &config) {
    NormalizeValue<T>::Config(config);
    m_rankHigh = config.GetAttributeBool("rank_high", m_rankHigh);
    m_invert = config.GetAttributeBool("invert", m_invert);


  }
  static const dataNameType_t & IdName (void)
  {
    static const dataNameType_t idName ("StepRanked");
    return idName;
  }
  void Print(std::ostream &os) const {
    os << "_Nsi" << m_invert << "r" << m_rankHigh ;
  }

  double EvaluateValue(T val)
  {
    uint32_t totalSize = this->m_values.size();
    if (0 == totalSize)
      return 0.0;

    if (1 == totalSize)
      return 1.0;
    double retValue=-1.0;
    if (m_rankHigh) { //values tends towards higher
      auto count = totalSize;
      for (auto i=this->m_values.crbegin(); i != this->m_values.crend(); i++) {
        if (val == *i) {
          retValue= (double) count/totalSize;
//std::cout << "  normalized:" << count << "/" << totalSize << "\n";
          break;
        } else {
          --count;
        }
      }
      if (-1.0 == retValue) {throw ("Value not found");}
    } else { //value tends towards lower
      auto count = 1;
      for (auto i=(this->m_values.cbegin()); i != this->m_values.cend(); i++) {
        if (val == *i) {
          retValue= (double) count/totalSize;
          break;
        } else {
          ++count;
        }
      }
      if (-1.0 == retValue) {throw ("Value not found");}
    }
    if (m_invert)
    {return 1.0-retValue;}
    else
    {return retValue;}
  }
private:
  bool m_rankHigh;
  bool m_invert;

};
//class StepRanked (even parsing)
#endif
