
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
 double values[]={5.0,9.0,17.0,23.0,31.0,32.0};


 {  //Create a Node
  UtilityConfigXml config;
  bool status = config.FirstNodeFileConfig ("NetNormalize1oN.xml");

  config.Print ();
  if (!status)
    std::cout << "config failed:" << std::endl;

  NodeManager *mm1 = new NodeManager(config);

 
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
  mm1->OnInit(mod);


  //stats->DumpStats(std::cout);
std::cout << "\nNode1 GeometricRanked bias low true- NetNormalize1oN.xml config:\n";
  mm1->Print(std::cout);



    PktType test[6];
    IcnName < std::string > nameTest, dummyTest;
    nameTest.SetFullName("/sample/test");
    ModuleManager *testModule = mm1->GetModule ("postprocess");
    for(unsigned int i=0; i< 6; i++)
    {
      test[i].SetPacketPurpose (PktType::DATAPKT);
      nameTest.SetUniqAttribute("part", i);
      test[i].SetName(nameTest);
      test[i].SetUnsignedNamedAttribute("oneOverN", values[i]);
        mm1->OnPktIngress(test[i]);
	mm1->Compute();
	auto fname = test[i].GetName().GetFullName();
	double val = testModule->Value(fname);
	std::cout << fname << " :GeometricRanked Sent hops of " << values[i] <<  " recieved " << val << "\n";
    }
    mm1->Print(std::cout);
    }

 {  //Create a Node
  UtilityConfigXml config;
  bool status = config.FirstNodeFileConfig ("NetNormalize1oN2.xml");

  config.Print ();
  if (!status)
    std::cout << "config failed:" << std::endl;

  NodeManager *mm1 = new NodeManager(config);

 
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
  mm1->OnInit(mod);


  //stats->DumpStats(std::cout);
std::cout << "\nNode1 GeometricRanked bias low true - NetNormalize1oN.xml config:\n";
  mm1->Print(std::cout);



    PktType test[6];
    IcnName < std::string > nameTest, dummyTest;
    nameTest.SetFullName("/sample/test");
    ModuleManager *testModule = mm1->GetModule ("postprocess");
    for(unsigned int i=0; i< 6; i++)
    {
      test[i].SetPacketPurpose (PktType::DATAPKT);
      nameTest.SetUniqAttribute("part", i);
      test[i].SetName(nameTest);
      test[i].SetUnsignedNamedAttribute("oneOverN", values[i]);
        mm1->OnPktIngress(test[i]);
	mm1->Compute();
	auto fname = test[i].GetName().GetFullName();
	double val = testModule->Value(fname);
	std::cout << fname << " :GeometricRanked Sent hops of " << values[i] <<  " recieved " << val << "\n";
    }
    mm1->Print(std::cout);
    }


exit;

   {
    std::cout << "\n\n\nGeometricMatch lowBias\n";
   //Create a Node
  UtilityConfigXml config;
  bool status = config.FirstNodeFileConfig ("NetNormalize1oN2.xml");

  config.Print ();
  if (!status)
    std::cout << "config failed:" << std::endl;

  NodeManager *mm1 = new NodeManager(config);

 
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
  mm1->OnInit(mod);


  //stats->DumpStats(std::cout);
std::cout << "\n\n\nNode1 config:\n";
  mm1->Print(std::cout);
 


    PktType test[6];
    IcnName < std::string > nameTest, dummyTest;
    nameTest.SetFullName("/sample/test");
    ModuleManager *testModule = mm1->GetModule ("postprocess");
    for(unsigned int i=0; i< 6; i++)
    {
      test[i].SetPacketPurpose (PktType::DATAPKT);
      nameTest.SetUniqAttribute("part", i);
      test[i].SetName(nameTest);
      test[i].SetUnsignedNamedAttribute("normalmatch", values[i]);
        mm1->OnPktIngress(test[i]); //1,2,3,4,5 times
        mm1->Compute();
	auto fname = test[i].GetName().GetFullName();
	double val = testModule->Value(fname);
	if (i<3 || i>4) {
	std::cout << fname << " :Sent value of " << i << " expect value of " << 0 << " recieved " << val << "\n";
	} else {
	std::cout << fname << " :Sent value of " << i << " expect value of " << 1 << " recieved " << val << "\n";
	} 
    }
     mm1->Print(std::cout);
   }

   {
   std::cout << "NormalRanked\n";
    //Create a Node
  UtilityConfigXml config;
  bool status = config.FirstNodeFileConfig ("NetNormalNRanked.xml");

  config.Print ();
  if (!status)
    std::cout << "config failed:" << std::endl;


  NodeManager *mm1 = new NodeManager(config);
  
 
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
  mm1->OnInit(mod);


  //stats->DumpStats(std::cout);
std::cout << "\n\n\nNode1 config:\n";
  mm1->Print(std::cout);



    PktType test[6];
    IcnName < std::string > nameTest, dummyTest;
    nameTest.SetFullName("/sample/test");
    ModuleManager *testModule = mm1->GetModule ("postprocess");
    for(unsigned int i=0; i< 6; i++)
    {
      test[i].SetPacketPurpose (PktType::DATAPKT);
      nameTest.SetUniqAttribute("part", i);
      test[i].SetName(nameTest);
      test[i].SetUnsignedNamedAttribute("normalranked", values[i]);
        mm1->OnPktIngress(test[i]); //1,2,3,4,5 times
    }
        for(unsigned int i=0; i< 6; i++)
    {
      test[i].SetPacketPurpose (PktType::DATAPKT);
      nameTest.SetUniqAttribute("part", i);
      test[i].SetName(nameTest);
      test[i].SetUnsignedNamedAttribute("normalstep", values[i]);
        mm1->Compute();
	auto fname = test[i].GetName().GetFullName();
	double val = testModule->Value(fname);
	std::cout << fname << " :Sent hops of " << i*4+1 << " expect value of " <<(i*4.0+1.0)/(21.0) << " recieved " << val << "\n";
    }

    mm1->Print(std::cout);
  }

  {
  std::cout << "StepRanked\n";
  //Create a Node
  UtilityConfigXml config;
  bool status = config.FirstNodeFileConfig ("NetNormalStep.xml");

  config.Print ();
  if (!status)
    std::cout << "config failed:" << std::endl;

  NodeManager *mm1 = new NodeManager(config);

 
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
  mm1->OnInit(mod);


  //stats->DumpStats(std::cout);
std::cout << "\nNode1 config:\n";
  mm1->Print(std::cout);



    PktType test[6];
    IcnName < std::string > nameTest, dummyTest;
    nameTest.SetFullName("/sample/test");
    ModuleManager *testModule = mm1->GetModule ("postprocess");
    for(unsigned int i=0; i< 6; i++)
    {
      test[i].SetPacketPurpose (PktType::DATAPKT);
      nameTest.SetUniqAttribute("part", i);
      test[i].SetName(nameTest);
      test[i].SetUnsignedNamedAttribute("normalstep", i*4+1);
        mm1->OnPktIngress(test[i]); //1,2,3,4,5 times
    }
    for(unsigned int i=0; i< 6; i++)
    {
      test[i].SetPacketPurpose (PktType::DATAPKT);
      nameTest.SetUniqAttribute("part", i);
      test[i].SetName(nameTest);
      test[i].SetUnsignedNamedAttribute("normalstep", values[i]);
        mm1->Compute();
	auto fname = test[i].GetName().GetFullName();
	double val = testModule->Value(fname);
	std::cout << fname << " :Sent hops of " << i*4+1 << " expect value of " << (i+1)/6.0 << " recieved " << val << "\n";
    }
    mm1->Print(std::cout);
   }

}

