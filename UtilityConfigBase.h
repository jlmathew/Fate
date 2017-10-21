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
#ifndef UTILITYCONFIGBASE_H_
#define UTILITYCONFIGBASE_H_

#include <string>
#include <stdint.h>
#include <stdexcept>

class StoreAction;


class ConfigWrapper
{
public:
  virtual ~ConfigWrapper() {};
  virtual bool FirstUtilityFileConfig (const std::string & fileName) = 0;
  virtual bool FirstUtilityStringConfig (const std::string & strConfig) = 0;
  virtual bool FirstModuleFileConfig (const std::string & fileName) = 0;
  virtual bool FirstModuleStringConfig (const std::string & strConfig) = 0;

  virtual bool FirstNodeFileConfig (const std::string & fileName) = 0;
  virtual bool FirstNodeStringConfig (const std::string & strConfig) = 0;
  virtual bool FirstStoreFileConfig (const std::string & fileName) = 0;
  virtual bool FirstStoreStringConfig (const std::string & strConfig) = 0;

  virtual std::string GetAttribute (const std::string & attrName, const std::string & defaultValue) const = 0;
  virtual double GetAttribute (const std::string & attrName, double defaultValue) const = 0;
  virtual int64_t GetAttribute (const std::string & attrName, int64_t defaultValue) const = 0;
  virtual uint64_t GetAttribute (const std::string & attrName, uint64_t defaultValue) const = 0;
  /*
     string array "example" keeps getting demoted to boolean, not string, so a name change to easily fix it 
   */
  virtual bool GetAttributeBool (const std::string & attrName, bool defaultalue) const = 0;

  virtual ConfigWrapper *GetFirstChildUtility (const std::string & childName = "") = 0;
  virtual ConfigWrapper *GetNextChildUtility (const std::string & childName = "") = 0;
  virtual ConfigWrapper *GetFirstChildModule (const std::string & childName = "") = 0;
  virtual ConfigWrapper *GetNextChildModule (const std::string & childName = "") = 0;
  virtual ConfigWrapper *GetFirstChildStore (const std::string & childName = "") = 0;
  virtual ConfigWrapper *GetNextChildStore (const std::string & childName = "") = 0;
  virtual bool valid () const = 0;
  virtual void Print () = 0;
  virtual ConfigWrapper * Clone()=0;
protected:
    bool m_valid;


};

#endif
