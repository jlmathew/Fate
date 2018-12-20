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
#ifndef FATE_TABLE_BASE_H_
#define FATE_TABLE_BASE_H_


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
#include <tuple>

#define PhyName std::string
#define NodeName std::string
typedef  std::tuple<PhyName, NodeName> RouteKey_t;
typedef std::tuple<NodeName, RouteInfo> RouteInfo_t;
typedef std::tuple<RouteKey_t, RouteInfo_t> FullRouteInfo_t;

struct RouteInfo
{
    uint32_t m_hops;  //min hops
    double m_minBandwidth; //min bandwidth
    double m_ber;  //bit error rate, if avail
    double m_minPhysicalSecurity; //how physicall secure wifi < cable < fiber
    uint32_t m_minBufferFree; //how many entries free to buffer packets
    double m_maxNodeCongestion; //how congested is node in processing
    double m_minBatteryLife;   //for sensors
    uint32_t m_totalEnergyUsed; //divide by hops for energy efficiency
    std::tuple<double, double, double> m_location;
};


struct RouteCriteria
{
   RouteCriteria():
m_validHopsCriteria(false),m_validminBandwidthCriteria(false),m_validberCriteria(false),m_validminPhysicalSecurityCriteria(false),m_validminBufferFreeCriteria(false),m_validmaxNodeCongestionCriteria(false),m_validminBatteryLifeCriteria(false),m_validtotalEnergyUsed(false),m_validlocationCriteria(false) }
	//do we use the criteria or not?
    bool m_validHopsCriteria;
    bool m_validminBandwidthCriteria;
    bool m_validberCriteria;  //bit error rate, if avail
     bool m_validminPhysicalSecurityCriteria; //how physicall secure wifi < cable < fiber
     bool m_validminBufferFreeCriteria; //how many entries free to buffer packets
     bool m_validmaxNodeCongestionCriteria; //how congested is node in processing
     bool m_validminBatteryLifeCriteria;   //for sensors
     bool m_validtotalEnergyUsedCriteria; //divide by hops for energy efficiency
     bool m_validlocationCriteria; 

     //actual range of criteria to allow, if valid
    RangeData<uint32_t> m_hopsCriteria;  //min hops
     RangeData<double> m_minBandwidthCriteria; //min bandwidth
     RangeData<double> m_berCriteria;  //bit error rate, if avail
     RangeData<double> m_minPhysicalSecurityCriteria; //how physicall secure wifi < cable < fiber
     RangeData<uint32_t> m_minBufferFreeCriteria; //how many entries free to buffer packets
     RangeData<double> m_maxNodeCongestionCriteria; //how congested is node in processing
     RangeData<double> m_minBatteryLifeCriteria;   //for sensors
     RangeData<uint32_t> m_totalEnergyUsedCriteria; //divide by hops for energy efficiency
     std::tuple<RangeData<double>,RangeData<double>,RangeData<double> > m_locationCriteria; 

};

class TableBase
{
	public:
TableBase():m_tableId(0) {}
virtual ~TableBase() {}
void SetTableName(const std::string &name) { m_name=name;}
std::string GetTableName() { return m_name;}

	private:

   std::string m_name;
   uint64_t m_tableId;
 
};


class TableRouteInfo : public TableBase
{
	
	public:
    TableRouteInfo();
    virtual ~TableRouteInfo();

    UpdateTable(const PhyName &phyName, const NodeName &destination,const NodeName &neighbor,  const RouteInfo &updatedInfo);
    //GetEntries(const PhyName &phyName, const NodeName &neighbor, const NodeName &destination, std::list <RouteInfo> &updatedInfo);
    GetMatches(const std::list<PhyName> &phyList, const std::list<NodeName> &destList, const std::list<NodeName> neighborList, const std::list< RouteCriteria> &routeCriteriaList, std::list<FullRouteInfo_t> &routeMatchList);


};

class PhyProperties : public TableBase
{
    PhyProperties();
    virtual ~PhyProperties();
    void GetMedium();
    void GetDuplex();
    void GetBandwidthBps();
    void GetRangeinMeters();
    void GetFrequency();
    void GetPhyBusy();
};
#endif

