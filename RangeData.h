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

#ifndef _RANGEDATA_T_
#define _RANGEDATA_T_

#include <iostream>
#include <exception>

template < class T > class RangeData;

template < class T > std::ostream & operator << (std::ostream &, const RangeData < T > &);

template < class T > std::istream & operator >> (std::istream &, RangeData < T > &);

template < class T > 
class RangeData
{
public:
  enum rangeComparIsonTypes_t
  {
    LeftRightInclusive = 0,
    LeftInclusive,
    RightInclusive,
    LeftRightExclusive,
    outOfRange
  };

  RangeData ();
  RangeData (const T & value);
  RangeData (const T & lower, T const &upper);
  RangeData (const RangeData < T > &range);

  virtual ~ RangeData ();

  void
  SetLowerBound (const T & rhs);
  void
  SetUpperBound (const T & rhs);
  T
  getLowerBound ();
  T
  getUpperBound ();
  void
  SetRangeCompare (const std::string & type);
  void
  SetRangeCompare (const rangeComparIsonTypes_t type);
  bool
  IsInRange (const T & other) const;
  bool
  IsExactMatch (const RangeData < T > &range) const;

  T & 
  operator= (const T & rhs);
  bool
  operator== (const RangeData < T > &rhs) const;
  bool
  operator!= (const RangeData < T > &rhs) const;
  bool
  operator< (const RangeData < T > &rhs) const;

  // FRIEND FUNCTIONS
  friend
    std::ostream &
  operator << <> (std::ostream & os, const RangeData < T > &rhs);
  friend
    std::istream &
  operator >> <> (std::istream & is, RangeData < T > &rhs);

private:
  T lowerBound, upperBound;
  rangeComparIsonTypes_t
    m_compareType;

};

template < class T > std::ostream & operator << (std::ostream & out, const RangeData < T > &rhs)
{
  switch (rhs.m_compareType)
    {
      case RangeData < T >::LeftRightExclusive:
        out << "(" << rhs.lowerBound << "," << rhs.upperBound << ")";
        break;
      case RangeData < T >::LeftInclusive:
        out << "(" << rhs.lowerBound << "," << rhs.upperBound << "]";
        break;
      case RangeData < T >::RightInclusive:
        out << "[" << rhs.lowerBound << "," << rhs.upperBound << ")";
        break;
      case RangeData < T >::LeftRightInclusive:
        out << "[" << rhs.lowerBound << "," << rhs.upperBound << "]";
        break;
      default:
        //illegal
        out << "illegal comparison parameter";
    }
  return (out);
}

template < class T > std::istream & operator >> (std::istream & in, RangeData < T > &rhs)
{
  char
    leftBracket,
    rightBracket,
    comma;

  in >> std::ws;
  in.get (leftBracket);
  rhs.lowerBound << in;
  in.get (comma);
  rhs.upperBound << in;
  in.get (rightBracket);
  if ('(' == leftBracket)
    {
      if (')' == rightBracket)
        {
          rhs.m_compareType = RangeData < T >::LeftRightInclusive;
        }
      else
        {
          rhs.m_compareType = RangeData < T >::LeftInclusive;
        }
    }
  else
    {
      if (')' == rightBracket)
        {
          rhs.m_compareType = RangeData < T >::RightInclusive;
        }
      else
        {
          rhs.m_compareType = RangeData < T >::LeftRightExclusive;
        }
    }
  return in;
}

template < class T > 
RangeData < T >::RangeData (const T & _lowerBound, const T & _higherBound)
:m_compareType (RangeData < T >::LeftRightInclusive)
{
  lowerBound = _lowerBound;
  upperBound = _higherBound;

}

template < class T > 
RangeData < T >::RangeData ()
:m_compareType (RangeData < T >::LeftRightInclusive)
{
}

template < class T > 
RangeData < T >::RangeData (const T & _number)
  : m_compareType (RangeData < T >::LeftRightInclusive)
{
  lowerBound = _number;
  upperBound = _number;

}

template < class T > 
RangeData < T >::RangeData (const RangeData < T > &_rangeData)
{
  lowerBound = _rangeData.lowerBound;
  upperBound = _rangeData.upperBound;
  m_compareType = _rangeData.m_compareType;

}

template < class T > 
RangeData < T >::~RangeData ()
{
}

template < class T > 
void RangeData < T >::SetLowerBound (const T & _lowerBound)
{
  lowerBound = _lowerBound;
}

template < class T > 
void RangeData < T >::SetUpperBound (const T & _upperBound)
{
  upperBound = _upperBound;
}

template < class T > 
T RangeData < T >::getLowerBound ()
{
  return lowerBound;
}

template < class T > 
T RangeData < T >::getUpperBound ()
{
  return upperBound;
}


template < class T > 
bool RangeData < T >::IsInRange (const T & other) const
{
  switch (m_compareType)
    {
      case RangeData < T >::LeftRightInclusive:
        if ((other >= lowerBound) && (other <= upperBound))
          {
            return true;
          }
        return
          false;

        break;
      case RangeData < T >::LeftInclusive:
        if ((other > lowerBound) && (other <= upperBound))
          {
            return true;
          }
        return false;
        break;
      case RangeData < T >::RightInclusive:
        if ((other >= lowerBound) && (other < upperBound))
          {
            return true;
          }
        return false;
        break;
      case RangeData < T >::LeftRightExclusive:
        if ((other > lowerBound) && (other < upperBound))
          {
            return true;
          }
        return false;
        break;
      default:
        throw std::range_error ("Invalid comparIson methodology");
        break;
    }
}

template < class T > 
bool RangeData < T >::IsExactMatch (const RangeData < T > &range) const 
{
  if ((range.getLowerBound == lowerBound) && (range.getUpperBound == upperBound) && (range.m_compareType == m_compareType))
    {
      return true;
    }

  return false;
}

template < class T > T & RangeData < T >::operator= (const T & rhs)
{
  // Check for self-assignment!
  if (this == &rhs)             // Same object?
    return *this;               // Yes, so skip assignment, and just return *thIs.

  lowerBound = rhs.getLowerBound ();
  upperBound = rhs.getUpperBound ();
  m_compareType = rhs.m_compareType;
  return *this;
}

template < class T > 
bool RangeData < T >::operator!= (const RangeData < T > &rhs) const
{
  return !((lowerBound == rhs.lowerBound) && (upperBound == rhs.upperBound) && (rhs.m_compareType == m_compareType));
}


template < class T >
bool RangeData < T >::operator== (const RangeData < T > &rhs) const
{

  return (lowerBound == rhs.lowerBound) && (upperBound == rhs.upperBound) && (rhs.m_compareType == m_compareType);
}

template < class T >
bool RangeData < T >::operator< (const RangeData < T > &rhs) const
   {
     if ((rhs.lowerBound < lowerBound))
       {
         return true;
       }
     return
       false;
   }

template < class T > 
void RangeData < T >::SetRangeCompare (const std::string & type)
{
  RangeData < T >::rangeComparIsonTypes_t range = RangeData < T >::outOfRange;
  if (!type.compare ("LeftRightInclusive"))
    {
      range = LeftRightInclusive;
    }
  else if (!type.compare ("LeftInclusive"))
    {
      range = LeftInclusive;
    }
  else if (!type.compare ("RightInclusive"))
    {
      range = RightInclusive;
    }
  else if (!type.compare ("LeftRightExclusive"))
    {
      range = LeftRightExclusive;
    }
  SetRangeCompare (range);
}


template < class T > 
void RangeData < T >::SetRangeCompare (const RangeData < T >::rangeComparIsonTypes_t type)
{
  if ((type < 0) || (type >= outOfRange))
    {
      throw
      std::range_error ("Invalid comparIson methodology");
    }
  m_compareType = type;
}

#endif
