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

#include "UtilityConfigXml.h"
#include <iostream>

UtilityConfigXml::UtilityConfigXml ()
 // : m_doc(NULL)
{
}

UtilityConfigXml::UtilityConfigXml (const pugi::xml_node & xmlNode, const pugi::xml_document &xmlDoc)
//  : m_doc(NULL)
{
  m_valid = Config (xmlNode, xmlDoc);
  if (!m_valid)
    {
      //std::cout << "failed utilityconfigxml config:" << std::endl;
    }
}

UtilityConfigXml::~UtilityConfigXml ()
{
//  if (m_doc)
//      delete m_doc;
}

bool
UtilityConfigXml::FirstUtilityFileConfig (const std::string & fileName)
{
  m_valid = false;
//  if (m_doc)
 //     delete m_doc;
//  m_doc = new pugi::xml_document;

  pugi::xml_parse_result result = m_doc.load_file (fileName.c_str ());
  //pugi::xml_parse_result result = m_doc->load_file (fileName.c_str ());
  if (!result)
    {
      //bad parse, error
      std::cout << result.description () << std::endl;
    }
  m_xml = m_doc.child ("Utility");
  //m_xml = m_doc->child ("Utility");
  if (m_xml)
    {
      m_valid = true;
    }


  return m_valid;               //fix
}

bool UtilityConfigXml::FirstUtilityStringConfig (const std::string & strConfig)
{
  m_valid = false;
  //if (m_doc)
  //    delete m_doc;
  //m_doc = new pugi::xml_document;
  pugi::xml_parse_result result = m_doc.load (strConfig.c_str ());
  //pugi::xml_parse_result result = m_doc->load (strConfig.c_str ());
  if (!result)
    {
      //bad parse, error
      std::cout << result.description () << std::endl;
    }
  //m_xml = m_doc->child ("Utility");
  m_xml = m_doc.child ("Utility");
  if (m_xml)
    {
      m_valid = true;
    }
  return m_valid;
}

bool
UtilityConfigXml::FirstNodeFileConfig (const std::string & fileName)
{
  m_valid = false;
  //m_doc = new pugi::xml_document;
  //pugi::xml_parse_result result = m_doc->load_file (fileName.c_str ());
  pugi::xml_parse_result result = m_doc.load_file (fileName.c_str ());
  if (!result)
    {
      //bad parse, error
      std::cout << result.description () << std::endl;
    }
  m_xml = m_doc.child ("NodeModule");  //.child("Utility");
  //m_xml = m_doc->child ("NodeModule");  //.child("Utility");
  if (m_xml)
    {
      m_valid = true;
    }

  //Print();
  return m_valid;               //fix
}

bool
UtilityConfigXml::FirstNodeStringConfig (const std::string & strConfig)
{
  m_valid = false;
  //m_doc = new pugi::xml_document;
  //pugi::xml_parse_result result = m_doc->load (strConfig.c_str ());
  pugi::xml_parse_result result = m_doc.load (strConfig.c_str ());
  if (!result)
    {
      //bad parse, error
      std::cout << result.description () << std::endl;
    }
  //m_xml = m_doc->child ("NodeModule");
  m_xml = m_doc.child ("NodeModule");
  if (m_xml)
    {
      m_valid = true;
    }
  return m_valid;
}

bool
UtilityConfigXml::FirstModuleFileConfig (const std::string & fileName)
{
  m_valid = false;
  //m_doc = new pugi::xml_document;
  //pugi::xml_parse_result result = m_doc->load_file (fileName.c_str ());
  pugi::xml_parse_result result = m_doc.load_file (fileName.c_str ());
  //m_doc.print(std::cout);
  if (!result)
    {
      //bad parse, error
      std::cout << result.description () << std::endl;
    }
  //m_xml = m_doc->child("UtilityNetworkConfig").child("Utility");
  m_xml = m_doc.child ("UtilityModule");
  //m_xml = m_doc->child ("UtilityModule");
  if (m_xml)
    {
      m_valid = true;
    }


  //m_xml.print(std::cout);
  return m_valid;               //fix
}

bool
UtilityConfigXml::FirstModuleStringConfig (const std::string & strConfig)
{
  m_valid = false;
  //m_doc = new pugi::xml_document;
  //pugi::xml_parse_result result = m_doc->load (strConfig.c_str ());
  pugi::xml_parse_result result = m_doc.load (strConfig.c_str ());
  if (!result)
    {
      //bad parse, error
      std::cout << result.description () << std::endl;
    }
  m_xml = m_doc.child ("UtilityModule");
  //m_xml = m_doc->child ("UtilityModule");
  if (m_xml)
    {
      m_valid = true;
    }
  return m_valid;
}

bool
UtilityConfigXml::valid () const
{
  return m_valid;
}

bool UtilityConfigXml::Config (const pugi::xml_node & xmlConfig, const pugi::xml_document &doc)
{
  m_valid = false;
  m_xml = xmlConfig;
  //m_doc = const_cast < pugi::xml_document * >(doc);
  m_doc.reset(doc);
  if (m_xml)
    {
      m_valid = true;
    }                           //else { std::cout << "config fails " << m_valid << std::endl; }
  return m_valid;

}

std::string UtilityConfigXml::GetAttribute (const std::string & attrName, const std::string & defaultValue) const
{
  return m_xml.attribute (attrName.c_str ()).as_string (defaultValue.c_str ());
}

double
UtilityConfigXml::GetAttribute (const std::string & attrName, double defaultValue) const
{
  return m_xml.attribute (attrName.c_str ()).as_double (defaultValue);
}

int64_t
UtilityConfigXml::GetAttribute (const std::string & attrName, int64_t defaultValue) const
{
  return m_xml.attribute (attrName.c_str ()).as_llong (defaultValue);
}

uint64_t
UtilityConfigXml::GetAttribute (const std::string & attrName, uint64_t defaultValue) const
{
  return m_xml.attribute (attrName.c_str ()).as_ullong (defaultValue);
}

bool
UtilityConfigXml::GetAttributeBool (const std::string & attrName, bool defaultValue) const
{
  std::string defaultValueStr;
  if (defaultValue)
    {
      defaultValueStr = "TRUE";
    }
  else
    {
      defaultValueStr = "FALSE";
    }

  std::string boolStr = m_xml.attribute (attrName.c_str ()).as_string (defaultValueStr.c_str ());
  // make it all capitalize, easier to check
  std::transform (boolStr.begin (), boolStr.end (), boolStr.begin (),::toupper);
  if (!boolStr.compare ("TRUE"))
    {
      return true;
    }
  else if (!boolStr.compare ("FALSE"))
    {
      return false;
    }
  else
    {                      //dont know
      throw std::invalid_argument ("Unknown boolean type in config");
    }
}

ConfigWrapper *
UtilityConfigXml::GetFirstChildModule (const std::string & childName)
{
  //return new UtilityConfigXml(m_xml.child(childName.c_str()).child("UtilityModule"), m_doc);
  return new UtilityConfigXml (m_xml.child ("UtilityModule"), m_doc);
}

ConfigWrapper *
UtilityConfigXml::GetNextChildModule (const std::string & childName)
{
  m_xml = m_xml.next_sibling (childName.c_str ());
// m_xml.print(std::cout);     
  if (!m_xml)
    {
      m_valid = false;
    }

  return this;
}


ConfigWrapper *
UtilityConfigXml::GetFirstChildUtility (const std::string & childName)
{
  return new UtilityConfigXml (m_xml.child (childName.c_str ()), m_doc);
}

ConfigWrapper *
UtilityConfigXml::GetNextChildUtility (const std::string & childName)
{
  m_xml = m_xml.next_sibling (childName.c_str ());
  if (!m_xml)
    {
      m_valid = false;
    }

  return this;
}

void
UtilityConfigXml::Print ()
{

  m_xml.print (std::cout);
}


bool
UtilityConfigXml::FirstStoreFileConfig (const std::string & fileName)
{
  return false;
}

bool
UtilityConfigXml::FirstStoreStringConfig (const std::string & strConfig)
{
  return false;
}

ConfigWrapper *
UtilityConfigXml::GetFirstChildStore (const std::string & childName)
{
  return new UtilityConfigXml (m_xml.child (childName.c_str ()), m_doc);
}

ConfigWrapper *
UtilityConfigXml::GetNextChildStore (const std::string & childName)
{
  m_xml = m_xml.next_sibling (childName.c_str ());
  if (!m_xml)
    {
      m_valid = false;
    }

  return this;
}

ConfigWrapper *
UtilityConfigXml::Clone()
{
   UtilityConfigXml *newConfig = new UtilityConfigXml;
   newConfig->m_xml = m_xml;
   (newConfig->m_doc).reset(m_doc);
   return newConfig;
}
