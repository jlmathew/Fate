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

template<class T>
class Normalize
{
  public:
    Normalize() {
    }
    ~Normalize() 
    {
    }
    void InsertValue(T a)
    {
    }
    void InsertNearFirstValue(T a)  //for always first inserts
    {
    }
    void InsertNearLastValue(T a)  //for always last inserts, like time()
    {
    }

    bool DeleteValue(T a)
    {
    }
   
    double EvaluateValue(T) {
    }
};

template<class T>
class NormalizeValue : public Normalize<T>
{
  public:
    NormalizeValue() {}
    ~NormalizeValue() 
    { m_values.clear();}
    void InsertValue(T a)
    {
       m_values.insert(a);
    }
    void InsertNearFirstValue(T a)  //for always first inserts
    {
       m_values.insert(m_values.begin(),a);
    }
    void InsertNearLastValue(T a)  //for always last inserts, like time()
    {
       m_values.insert(m_values.rbegin(),a);
    }

    bool DeleteValue(T a)
    {
typename std::multiset<T>::iterator it;
	it = m_values.find(a);
	if (it != m_values.end())
        {
           m_values.erase(it);
           return true;
        } else {
            return false;
        }
    }
   
    double EvaluateValue(T) {
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
    

    void
    InsertMatchValue(const RangeData<T> &match)
    {
        m_data=match;
    }
    double EvaluateValue(T val)
    {
       if (m_data.InRange(val)) {
           return 1.0;
       } else 
	   return 0.0;
    }

  private:
    RangeData<T> m_data;
};

template <class T>
class GeometricRanked : public Normalize<T>
{
	public:
    GeometricRanked(): m_invert(false)
      	{};
    ~GeometricRanked() {};
    void SetInvert(bool inv) { m_invert = inv;}

    double EvaluateValue(T val)
    {
       double ret = 1.0;
       if (val != 0.0) {ret = 1.0/val;}
       if (m_invert) {
          ret = 1.0-ret;
       } 
       return ret;
    }
	private:
    bool m_invert;
};

//normalized from lowest to highest value; 0.0 .33, .5, .8 1.0
template <class T>
class NormalRanked : public NormalizeValue<T>
{
	public:
    NormalRanked() {}
    ~NormalRanked() { }
    double EvaluateValue(T val) 
    {
	if (this->m_values.empty())
		return 0.0;
        T min = *(this->m_values).cbegin(); //need something better?
	T max = *(this->m_values).crbegin();
	if (min == max)
		return 1.0;
	return (double) (val-min)/(max-min);

    }
};


//step normalized from 0 to 1 (0, .33, .66, 1.0)
template <class T>
class StepRanked: public NormalizeValue<T>
{
	public:
   StepRanked() {}
   ~StepRanked() { } 

    double EvaluateValue(T val) 
    {
std::invalid_argument( "received negative value" );
    }

    
};
//class StepRanked (even parsing)
#endif
