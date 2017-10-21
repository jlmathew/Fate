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
#ifndef UNIQUEREGISTER_H_
#define UNIQUEREGISTER_H_

#include <utility>
#include <map>
#include <vector>
#include "ContentName.h"
#include "PacketTypeBase.h"

//Just make it a static function ...

template < class Key, class Value > 
class RegisterTemplate
{
public:
RegisterTemplate ()
  :m_initialCount (static_cast < Value > (0))
  {
  };
  virtual ~ RegisterTemplate ()
  {
  };
  Value Register (const Key & key, Value & value)
  {
    typename std::map < Key, Value >::iterator it;

    it = registerMap.find (key);
    if (it == registerMap.end ())
      {
        registerMap.insert (make_pair (key, value));
        return value;
      }
    else
      {
        return (it->second);
      }
  }

  Value Register (const Key & key)
  {
    typename std::map < Key, Value >::iterator it;

    it = registerMap.find (key);
    if (it == registerMap.end ())
      {
        ++m_initialCount;
        registerMap.insert (std::make_pair (key, m_initialCount));
        return m_initialCount;
      }
    else
      {
        return (it->second);
      }
  }

  Key 
  DeRegister (const Value & value)
  {
    typename std::map < Key, Value >::iterator it;

    for (it = registerMap.begin (); it != registerMap.end (); it++)
      {
        if (it->second == value)
          {
            break;
          }

      }
    if (it == registerMap.end ())
      {
        throw "unregistered key";
      }
    else
      {
        return (it->first);
      }
  }

  bool Exist (const Key & key)
  {
    typename std::map < Key, Value >::const_iterator cit;

    cit = registerMap.find (key);
    return (cit != registerMap.end ());

  } 

  void Print(std::ostream &os, const std::string &delim=" = ",const std::string &eol="\n" ) const {
    typename std::map < Key, Value >::const_iterator it;

    for (it = registerMap.begin (); it != registerMap.end (); it++)
    {
        os << it->first << delim << it->second << eol;
    }
  }
  //LockKeys - do not allow new keys to be created on the fly
  //UnlockKeys - allow new keys to be created on the fly
  //KeyStatus - locked/unlock status

private:
  std::map < Key, Value > registerMap;
  Value m_initialCount;
};


//single global attribute registery

/*
template<class Key>
class RegisterTemplate<Key, uint64_t>
{
   RegisterTemplate() {};
   virtual ~RegisterTemplate() {};
   uint64_t
   Register( Key &key) {
     typename std::map< Key, uint64_t >::const_iterator it;
       it = registerMap.find(key);
       if (it == registerMap.nd()) {
         ++m_initialCount;
         registerMap.insert(std::make_pair(key, m_initialCount));
         return m_initialCount;
       } else {
         return it->second; 
       }
   }
   bool
   Exist(Key &key) const {
    typename std::map< Key, uint64_t >::const_iterator cit;
       cit  = registerMap.find(key);
       return (cit != registerMap.end()); 

   }
  private:
   std::map< Key, uint64_t > registerMap;
   static uint64_t m_initialCount;
};
template<class Key>
uint64_t RegisterTemplate<Key, uint64_t>::m_initialCount = 0;
*/




#endif
