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

Template<class T>
class Normalize
{
  public:
    Normalize() {}
    ~Normalize() 
    {
       m_values.clear();
    }
    InsertValue(T a)
    {
       m_values.insert(a);
    }
    InsertNearFirstValue(T a)
    {
       m_values.insert(m_values.begin(),a);
    }
    InsertNearLastValue(T a)
    {
       m_values.insert(m_values.rbegin(),a);
    }

    DeleteValue(T a)
    {
	std::multiset<t>::iterator it;
	it = m_values.find(a);
	if (it != m_values.end())
           m_values.erase(it);
    }
   
    double EvaluateValue(T);
  private:
    std::multiset<T>::m_values;
    T GetMinimum();
    T GetMaximum();
};

Template <B, T>
class NormalMatch : public Normalize<T>
{
    NormalMatch();
    ~NormalMatch();
    //dont need to track all values
    InsertValue(uint64_T a) {};
    InsertNearFirstValue(B a) {};
    InsertNearLastValue(B a) {};
    DeleteValue(B a) {};
    
    MatchValue(const RangeData &match)
    {
        m_data=match;
    }
    double EvaluateValue(B val)
    {
       if (m_data.InRange(val)) {
           return 1.0;
       } else 
	   return 0.0;
    }
	private:
    RangeData<B> m_data;
};

Template <B, T>
class NormalRanked : public Normalized<T>
{
    NormalRanked();
    ~NormalRanked();
    double EvaluateValue(B val) 
    {
	if (m_data.empty())
		return 0.0;
        B min = *m_data.cbegin(); //need something better
	B max = *m_data.crbegin();
	if (min == max)
		return 1.0;
	return (double) (val-min)/(max-min);

    }
};

//class StepRanked (even parsing)
#endif
