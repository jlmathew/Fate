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

#ifndef UTILITYDEVICEMODULE_H_
#define UTILITYDEVICEMODULE_H_

#include "EventHandlerBase.h"
#include "IcnName.h"
#include "ContentName.h"
#include "UtilityConfigBase.h"
#include "GlobalModuleBase.h"
#include "UtilityHandlerBase.h"
//#include "UniqueRegister.h"
#include "BaseStorage.h"
#include "PhyDef.h"
#include <vector>

class ModuleManager;

class NodeStats {
public:
    virtual void IncStats(const std::string &name)=0;
    virtual void DecStats(const std::string &name)=0;
    virtual void GetStats(const std::string &name, double &val)  =0;
    virtual void GetStats(const std::string &name, uint64_t &val)  =0;
    virtual void SetStats(const std::string &name, double newStat)=0;
    virtual void SetStats(const std::string &name, uint64_t newStat)=0;
    virtual void DumpStats(std::ostream &os, const std::string &delim=" = ",const std::string &eol="\n") const = 0;
    virtual void PrintStats() { DumpStats(std::cout); }

};
union statType_t {
    double dStat;
    uint64_t uStat;
};
struct statStruct_t {
    enum structType_t {
       doubleType=0,
       uint64Type,
       maxTypes
    };
    structType_t type;
    statType_t value;

};
class CustomStats : public NodeStats {
    private:
    memMap<std::string, statStruct_t> m_stats;

    public:

    CustomStats() {};
    virtual ~CustomStats() { };
    virtual void IncStats(const std::string &name) {
        statStruct_t value;
        bool exist = m_stats.ExistData(name, value);
        if (!exist) {
          throw  ::std::range_error("stat not intializied");
        }
        if (value.type == statStruct_t::doubleType) {
            value.value.dStat += 1.0;  
        } else {
            value.value.uStat += 1;  

        }
        SetStats(name, value);
    }

    virtual void DecStats(const std::string &name) {
        statStruct_t value;
        bool exist = m_stats.ExistData(name, value);
        if (!exist) {
          throw  ::std::range_error("stat not intializied");
        }
        if (value.type == statStruct_t::doubleType) {
            value.value.dStat -= 1.0;  
        } else {
            value.value.uStat -= 1;  
        }
        SetStats(name, value);


    }

    virtual void GetStats(const std::string &name, double &retvalue)  {
        statStruct_t value;
        bool exist = m_stats.ExistData(name, value);
        if (!exist) {
          throw  ::std::range_error("stat not intializied");
        }
        if (value.type == statStruct_t::doubleType) {
           retvalue = value.doubleType;  
        } else {
           throw ::std::invalid_argument("Stat not type double");
        }
    }

    virtual void GetStats(const std::string &name, uint64_t &retvalue)  {
        statStruct_t value;
        bool exist = m_stats.ExistData(name, value);
        if (!exist) {
          throw  ::std::range_error("stat not intializied");
        }
        if (value.type == statStruct_t::doubleType) {
           throw ::std::invalid_argument("Stat not type uint64");
        } else {
           retvalue = value.uint64Type;  
        }
    }

    virtual void SetStats(const std::string &name, double newStat) {
        statStruct_t value;
        value.type = statStruct_t::doubleType;
        value.value.dStat=newStat;
          m_stats.SetData(name, value);
    }
    virtual void SetStats(const std::string &name, uint64_t newStat) {
        statStruct_t value;
        value.type = statStruct_t::uint64Type;
        value.value.uStat=newStat;
          m_stats.SetData(name, value);
    }
    virtual void SetStats(const std::string &name, const statStruct_t &newStat) {
             m_stats.SetData(name, newStat);
    }
    virtual void DumpStats(std::ostream &os, const std::string &delim,const std::string &eol) const {
      std::map< std::string, statStruct_t> *printStat = m_stats.GetDataAsMap();

      typename std::map < std::string, statStruct_t >::const_iterator it;

    for (it = printStat->begin (); it != printStat->end (); it++)
    {
        os << it->first << delim;
        if (it->second.type == statStruct_t::uint64Type)
          os << (it->second).value.uStat << eol;
        else {
          os << (it->second).value.dStat << eol;
        }
    }
    delete printStat;
  }
   
};
 
class DeviceModule
{
private:
  std::string m_myName;
  NodeManager *m_nodeSelf; //node 
  ModuleManager *m_moduleSelf; //immediate module
  NodeStats *m_stats;
  std::map < std::string, GlobalModuleLog * > m_localLogs;
  std::vector < NetworkPhy > m_networkPhys;
 
public:
  DeviceModule() :
  m_nodeSelf(NULL),
  m_moduleSelf(NULL),
  m_stats(NULL)
  {}

  void GetLocationAsInt (uint64_t &x, uint64_t &y, uint64_t &z)
  {
  }

void
AddPhy(const NetworkPhy &phy) {
   m_networkPhys.push_back(phy);
}
void
SetAllPhys(const std::vector<NetworkPhy> &allPhys)
{
   m_networkPhys.clear();
   m_networkPhys.insert(m_networkPhys.begin(), allPhys.begin(), allPhys.end());
}
void
GetAllPhys(std::vector<NetworkPhy> &networkPhys)
{
  networkPhys.clear();
  networkPhys.insert(networkPhys.begin(), m_networkPhys.begin(), m_networkPhys.end()); 
}
std::string
GetNodeName() const
{
  return m_myName;
}

void
SetNodeName(const std::string &name)
{
  m_myName = name;
}

ModuleManager *
GetSelfModule () const 
{
    return m_moduleSelf;
}

void 
SetSelfModule (ModuleManager * self)
{
  m_moduleSelf = self;
}

NodeManager *
GetSelfNode () const 
{
    return m_nodeSelf;
}

void 
SetSelfNode (NodeManager * self)
{
  m_nodeSelf = self;
}

void
SetNodeStats( NodeStats *stats)
{
   m_stats = stats;
}

NodeStats *
GetNodeStats()
{
  return m_stats;
}

void
SetLocalLog(const std::string &name, GlobalModuleLog *log)
{
   m_localLogs[name]=log;
} 
GlobalModuleLog *
GetLocalLog(const std::string &name) 
{
   return m_localLogs[name];
}
};


#endif
