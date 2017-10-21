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
#ifndef UTILITYCONFIGXML_H_
#define UTILITYCONFIGXML_H_

#include <string>
#include <stdint.h>
#include "UtilityConfigBase.h"
#include "pugixml.h"
#include "string.h"
#include <algorithm>
#include <exception>
#include <stdexcept>



class UtilityConfigXml:public ConfigWrapper
{

public:
  UtilityConfigXml ();
  UtilityConfigXml (const pugi::xml_node & xmlNode, const pugi::xml_document &doc);
  //UtilityConfigXml (const pugi::xml_node & xmlNode, const pugi::xml_document * doc);
    virtual ~ UtilityConfigXml ();
public:

    bool FirstModuleFileConfig (const std::string & fileName = "UtilityModule");
  bool FirstModuleStringConfig (const std::string & strConfig);
  bool FirstUtilityFileConfig (const std::string & fileName = "Utility");
  bool FirstUtilityStringConfig (const std::string & strConfig);
  bool FirstStoreFileConfig (const std::string & fileName = "Store");
  bool FirstStoreStringConfig (const std::string & strConfig = "Store");
  bool Config (const pugi::xml_node & xmlNode, const pugi::xml_document &doc);
  //bool Config (const pugi::xml_node & xmlNode, const pugi::xml_document * doc);

/*
    m_valueHigh = xmlConfig.attribute("highValue").as_double(m_valueHigh);

const char_t* xml_attribute::as_string(const char_t* def = "") const;

int xml_attribute::as_int(int def = 0) const;
unsigned int xml_attribute::as_uint(unsigned int def = 0) const;
double xml_attribute::as_double(double def = 0) const;
float xml_attribute::as_float(float def = 0) const;
bool xml_attribute::as_bool(bool def = false) const;
long long xml_attribute::as_llong(long long def = 0) const;
unsigned long long xml_attribute::as_ullong(unsigned long long def = 0) const;
   */

    std::string GetAttribute (const std::string & attrName, const std::string & defaultValue) const;
  double GetAttribute (const std::string & attrName, double defaultValue) const;
  int64_t GetAttribute (const std::string & attrName, int64_t defaultValue) const;
  uint64_t GetAttribute (const std::string & attrName, uint64_t defaultValue) const;
  bool GetAttributeBool (const std::string & attrName, bool defaultValue) const;

  bool FirstNodeFileConfig (const std::string & fileName);
  bool FirstNodeStringConfig (const std::string & strConfig);

/*Fix these to only use the correct parseOptions */
  ConfigWrapper *GetFirstChildUtility (const std::string & fileName = "Utility");
  ConfigWrapper *GetNextChildUtility (const std::string & fileName = "Utility");
  ConfigWrapper *GetFirstChildModule (const std::string & childName = "UtilityModule");
  ConfigWrapper *GetNextChildModule (const std::string & childName = "UtilityModule");
  ConfigWrapper *GetFirstChildStore (const std::string & childName = "Store");
  ConfigWrapper *GetNextChildStore (const std::string & childName = "Store");

  bool valid () const;
  void Print ();
  virtual ConfigWrapper* Clone();
private:
    pugi::xml_node m_xml;
    pugi::xml_document m_doc;
    //pugi::xml_document * m_doc;
};




#endif
