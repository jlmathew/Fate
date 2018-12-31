
#include "../UtilityConfigBase.h"
#include "../UtilityConfigXml.h"

#include "../UtilityHandlerBase.h"
#include "../UtilityFunctionGenerator.h"
#include "../IcnName.h"
#include <iostream>
#include <ostream>
#include <string>
#include "../PacketTypeBase.h"
#include "../GlobalModulesLinux.h"
#include "../BaseStorage.h"
#include <unistd.h>
#include "../NodeManager.h"
#include "../ForwardBasicManager.h"

int
main ()
{
  //Create a Node
  UtilityConfigXml config;
  bool status = config.FirstNodeFileConfig ("Ns3-node-configE.xml");

  config.Print ();
  if (!status)
    std::cout << "config failed:" << std::endl;

  NodeManager mm1 (config);
  NodeManager *mm2 = new NodeManager(config);
  NodeManager *mm3 = new NodeManager(config);

  //Node 3 will be done manually
  //UtilityHandlerBase *uc3 = UtilityGenerator::CreateNewUtility (config);

  //Node 4 will use a different config

  //Node 5 will use modules of node1, and create a new node


  //Test name

  //setup external modules 
  UtilityExternalModule *mod = new UtilityExternalModule;
  GlobalModuleTimerLinux *timer = new GlobalModuleTimerLinux;
  NodeStats *stats = new CustomStats;
  GlobalModule *global = new GlobalModule;
  global->SetGlobalTimer (timer);
  global->SetGlobalLog ("default", new CoutModuleLog);

  DeviceModule *devMod = new DeviceModule;
  devMod->SetNodeName("Node1");
  devMod->SetNodeStats(stats);
  mod->SetDevice(devMod);
  mod->SetGlobalModule(global);
  //set up first 2 as same module
  mm1.OnInit(mod);

  UtilityExternalModule *mod2 = new UtilityExternalModule;
  NodeStats *stats2 = new CustomStats;
  GlobalModule *global2 = new GlobalModule;
  global2->SetGlobalTimer (timer);
  global2->SetGlobalLog ("default", new CoutModuleLog);
  DeviceModule *devMod2 = new DeviceModule;
  devMod2->SetNodeName("Node2");
  devMod2->SetNodeStats(stats2);
  mod2->SetDevice(devMod2);
  mod2->SetGlobalModule(global2);
  mm2->OnInit(mod2);

  stats->DumpStats(std::cout);
  stats2->DumpStats(std::cout);
std::cout << "\nNode1 config:\n";
  mm1.Print(std::cout);

std::cout << "\nNode2 config:\n";
  mm2->Print(std::cout);

std::cout << "Self Test result: " << mm3->SelfTest() << "\n";
  //packet test


    PktType test[6];
    IcnName < std::string > nameTest, dummyTest;
    nameTest.SetFullName("/sample/test");
    dummyTest.SetFullName("/dummy");
    PktType dumdum;
    dumdum.SetPacketPurpose (PktType::DATAPKT);
    dumdum.SetName(dummyTest);
    for(unsigned int i=0; i< 6; i++)
    {
      test[i].SetPacketPurpose (PktType::DATAPKT);
      nameTest.SetUniqAttribute("part", i);
      test[i].SetName(nameTest);
        mm1.OnPktIngress(test[i]); //1,2,3,4,5 times
      for(unsigned int j=1; j<=i; j++) {
       mm1.OnPktIngress(test[i]); //1,2,3,4,5 times
        mm1.Compute();
      }
    }
    //mm1.Compute();
    for(unsigned int i=0; i< 3; i++)
    {
      test[i].SetPacketPurpose (PktType::INTERESTPKT);
      for(unsigned int j=0; j<i+2; j++)
      {
        mm1.OnPktIngress(test[i]); //3,5,7,4,5 times
      }
    }
      mm1.Compute();
      mm1.OnPktIngress(dumdum);
      mm1.Compute();

    mm1.DoDelete(test[2].GetAcclName());
    mm1.Compute();
    mm1.Print(std::cout);
  //send a packet and test it
  //PktType *interest1 = new PktType;

  //interest1->SetName (test4);
  //uc2->OnPktIngress(*interest1);



  /*status = config4.FirstNodeFileConfig ("xmlDefinitions/Ns3-node-configA.xml");
  //status = config4.FirstNodeFileConfig("xmlDefinitions/Ns3-node-configB.xml");
  //config4.Print();
  if (!status)
    std::cout << "config4 failed:" << std::endl;
  NodeManager node1 (config4);

//need to have store register itself
  node1.OnInit (mod);
  ModuleManager *cacheMod = node1.GetModule ("CacheModule");

  cacheMod->Print (std::cout);
  //node1.OnPktIngress(content2);
  //node1.Compute();

  std::cout << "node has " << node1.retNumModules () << " modules\n";
  //add a utilityblock to a module

  //add a module
  //add 3packets
  //check store if right size

  content1.SetPacketPurpose (PktType::DATAPKT);
  content1.SetNamedAttribute ("Print test", 1.23456789);
  //std::cout << "c1:" << content1 << std::endl;
  content1.DeleteAllAttributes (true);
  content1.DeleteAllAttributes (false);
  content2.SetPacketPurpose (PktType::DATAPKT);
  content2.SetPacketPurpose (PktType::INTERESTPKT);
  content3.SetPacketPurpose (PktType::DATAPKT);
  content4.SetPacketPurpose (PktType::DATAPKT);
  node1.OnPktIngress (content2);
  std::cout << "miss:" << content2 << std::endl;
  content2.SetPacketPurpose (PktType::DATAPKT);
  node1.OnPktIngress (content2);
  content2.SetPacketPurpose (PktType::INTERESTPKT);
  node1.OnPktIngress (content2);
  node1.OnPktIngress (content3);
  node1.OnPktIngress (content2);
  std::cout << "hit:" << content2 << std::endl;
  node1.OnPktIngress (content4);
  node1.Compute ();
  node1.Print (std::cout);
  double value;

  node1.Print (std::cout, test4.GetFullName (), value);
  //get highest value of cache module
  ModuleManager *first = node1.GetModule (0);
  ModuleManager *second = node1.GetModule (1);

  std::cout << "value is " << value << ", cachemod name is:" << cacheMod->Name () << " , module0:" << first->Name () <<
    " , second node is " << second->Name () << "\n";
  first->Print (std::cout);

  uint32_t numStores = node1.retNumStores ();

  std::cout << " there are " << numStores << " stores\n";
  StoreManager *store1 = node1.GetStore (0);
  TypicalCacheStore *store2 = dynamic_cast < TypicalCacheStore * >(node1.GetStore ("DataStore"));

  std::cout << "store name:" << store1->Name () << std::endl;
  std::cout << "store2 name:" << store2->Name () << std::endl;
  //cacheMod.Debug(COUT_LOG, PRINT_VALUES, TOP_10);
  //get highest value of security module
  //node1.Debug(COUT_LOG, PRINT_VALUES, TOP_10, "CacheModule");
 //debug look at all packets
  //add 1 packet, replace lowest one.  did it work?
  //debug look at all packets
  std::cout << "there are " << store2->size () << "elements in the cache\n";
  node1.OnPktIngress (content4);
  //node1.Compute();

  //Note, at this point, security/caching should be done.
  //Need to implement it in ns-3, hooking up the ingress/egress ports.
  IcnName < std::string > name2;
  nameTest.SetFullName ("/parc/jim.jpg");

  PktType interestBase;         // interest1 = new PktType;

  interestBase.SetName (nameTest);

  PktType copyPkt1 = interestBase;

  copyPkt1.SetUnsignedNamedAttribute ("uint64_t", 5555ULL, false);
  copyPkt1.SetNamedAttribute ("double", 55.55555, false);
  copyPkt1.SetNamedAttribute ("string", "jim:55.55555", false);
  copyPkt1.SetSignedNamedAttribute ("int64_t", -5556LL, true);
  copyPkt1.SetPacketPurpose (PktType::DATAPKT);
  testSt_t testStruct;

  testStruct.a = 5;
  testStruct.b = -3.14;
  testStruct.c = 'c';
  copyPkt1.SetObjectCpyNamedAttribute ("memcopy", testStruct, true);
  copyPkt1.SetPrintedNamedAttribute ("printcopy", 12.3456, false);

  //std::cout << "cout test\n" << copyPkt1;
  PktType copyPkt2;

  std::stringstream ss;
  ss << copyPkt1;
  ss >> copyPkt2;
  //std::cout << "cout test\n" << copyPkt2;
  testSt_t testStruct2;

  copyPkt2.GetObjectCpyNamedAttribute ("memcopy", testStruct2, true);
  double a = 0.0;

  copyPkt2.GetPrintedNamedAttribute ("printcopy", a, false);
  //std::cout << "double is 12.3456 - " << a << std::endl;
  //std::cout << "memcopy is (5,-3.14,c) " << testStruct2.a << ", " << testStruct2.b << "," << testStruct2.c << std::endl;
  std::vector < uint8_t > serialData;
  copyPkt2.Serialize (serialData);
  //std::cout << "serial size is:" << serialData.size() << std::endl;

  PktType copyPkt3;

  copyPkt3.Deserialize (serialData);
  //std::cout << "cout test3\n" << copyPkt3;
  copyPkt3.GetObjectCpyNamedAttribute ("memcopy", testStruct2, true);
  copyPkt3.GetPrintedNamedAttribute ("printcopy", a, false);
  //std::cout << "double is 12.3456 - " << a << std::endl;
  //std::cout << "memcopy is (5,-3.14,c) " << testStruct2.a << ", " << testStruct2.b << "," << testStruct2.c << std::endl;

  PktType copyPkt4 (copyPkt3);

  copyPkt4.DeleteAllAttributes (true);
  copyPkt4.DeleteAllAttributes (false);
  //std::cout << "Original:" << copyPkt3 << std::endl;
  //std::cout << "copy,delted:" << copyPkt4 << std::endl;
  //uc2->OnPktIngress(*interest1);
  for (int i = 1; i < 100000; i++)
    {
      PktType nextInterest;

      nextInterest.SetPacketPurpose (PktType::DATAPKT);
      IcnName < std::string > nextName = nameTest;
      nextName.SetUniqAttribute ("part", i);
      nextInterest.SetName (nextName);
      node1.OnPktIngress (nextInterest);
    }

  std::cout << "\nthere are " << store2->size () << "elements in the cache\n";
*/
  //next step, map ingress port data as named-attributes

  //implement forwarding

  //implement discovery and prefetch

  //add python support

  //add utility design program to automatically create xml files
}

