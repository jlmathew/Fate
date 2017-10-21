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
#include "UtilityHandlerBase.h"


UtilityHandlerBase::UtilityHandlerBase ()
:m_usePrettyPrint (true),
m_externalModule (NULL),
m_useAlias (false),
m_useAliasName (false),
m_name (""),
m_defaultValue (0.0),
m_useProxyName (false),
m_storageMethod (Storage_defaultStore),
m_attributeInsert (false),
m_defaultMissingReturnValue (0.0),
m_defaultInsertValue ("0.0"), 
m_createEntryMask (PktType::DATAPKT), 
m_updateEntryMask (PktType::DATAPKT | PktType::INTERESTPKT),
m_defaultAttribute("")
{
}

UtilityHandlerBase::UtilityHandlerBase (ConfigWrapper & config)
:m_usePrettyPrint (true),
m_externalModule (NULL),
m_useAlias (false),
m_useAliasName (false),
m_name (""),
m_defaultValue (0.0),
m_useProxyName (false),
m_storageMethod (Storage_defaultStore),
m_attributeInsert (false),
m_defaultMissingReturnValue (0.0),
m_defaultInsertValue ("0.0"), 
m_createEntryMask (PktType::DATAPKT), 
m_updateEntryMask (PktType::DATAPKT | PktType::INTERESTPKT),
m_defaultAttribute("")
{
  UtilityHandlerBase::Config (config);
}

UtilityHandlerBase::~UtilityHandlerBase ()
{

}

bool
UtilityHandlerBase::OnInit (UtilityExternalModule * outsideData)
{
  m_externalModule = outsideData;
  return true;
}

void
UtilityHandlerBase::Config (ConfigWrapper & config)
{

  m_defaultAttribute = config.GetAttribute("attribName",m_defaultAttribute);
  //common configurations, e.g. alias/base name, default value
  std::string new_name = config.GetAttribute ("aliasName", "");
  if (!new_name.empty ())
    {
      m_useAliasName = true;
      m_name = new_name;
    }
  else
    {
      new_name = config.GetAttribute ("proxyName", "");
      if (!new_name.empty ())
        {
          m_useProxyName = true;
          m_name = new_name;
        }
    }
  //jlm FIXME TODO should be nice enum value or string 'e.g. INTEREST | DATA'
  m_createEntryMask = config.GetAttribute ("createEntryPktType", m_createEntryMask);
  m_updateEntryMask = config.GetAttribute ("updateEntryPktType", m_updateEntryMask);
  
  //default packet attribute name to use
  //m_defaultXmlTag = confg.GetAttribute("defaultXmlTag", m_defaultXmlTag);

  m_useAlias = m_useProxyName | m_useAliasName;

  m_defaultValue = config.GetAttribute ("defaultValue", m_defaultValue);
  m_usePrettyPrint = config.GetAttributeBool ("usePrettyPrint", m_usePrettyPrint);
  std::string storeMethod = config.GetAttribute ("storageMethod", "None");
  if (!storeMethod.compare ("None"))
    {
    }
  else if (!storeMethod.compare ("MemMap"))
    {
      m_storageMethod = Storage_MemMap;
    }
  else if (!storeMethod.compare ("DBStore"))
    {
      m_storageMethod = Storage_MemMap;
    }
  else
    {
      assert (0);
    }                           //invalid configure }

  m_defaultMissingReturnValue = config.GetAttribute ("missingAttributeReturnValue", m_defaultMissingReturnValue);
  m_attributeInsert = config.GetAttributeBool ("addMissingAttribute", m_attributeInsert);
  if (m_attributeInsert)
    {
      m_defaultInsertValue = config.GetAttribute ("missingAttributeDefaultValue", "Missing");

    }
  if (!m_defaultInsertValue.compare ("Missing"))
    {
      assert (0);
    }
}


const std::string &
UtilityHandlerBase::Name () const
{
  return m_name;
}

void UtilityHandlerBase::Print (std::ostream & os, bool endParen) const
{
  std::ios::fmtflags saveFlags = os.flags ();

  os << Name ();

  os << "(";
  if (endParen)
    {
      os << ")";
    }
  os.flags (saveFlags);

}


void
UtilityHandlerBase::Print (std::ostream & os, const AcclContentName & name, double &value)  const
{
  Print (os, false);
  os << "=";
  value = Value (name);
  os << std::setprecision (15) << value;
  os << ")";

}

//Default 
void UtilityHandlerBase::ComputeAndValue (std::list < PktType > &pktList)
{
}

void
UtilityHandlerBase::OnPktIngress (PktType & data)
{
}                               
 
void
UtilityHandlerBase::DoDelete (const AcclContentName & name)
{
}                               

void
UtilityHandlerBase::OnPktEgress (PktType & data, const PktTxStatus & status)
{
}                               
void
UtilityHandlerBase::OnPrePktEgress (PktType & data)
{
}   

uint64_t
UtilityHandlerBase::EstMemoryUsed (void) const
{
  return 0;
}

void UtilityHandlerBase::Compute (const AcclContentName & name)
{
}

void
UtilityHandlerBase::Compute ()
{
}

bool
UtilityHandlerBase::SelfTest()
{
  return true;
}
