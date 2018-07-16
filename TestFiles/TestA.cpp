

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
#include <vector>

int
main ()
{
  //Name Test
   IcnName < std::string > nameTest;
  nameTest.SetFullName ("/parc/test/sucks/jim.jpg");
  nameTest.SetUniqAttribute ("awesome", 9000);
  nameTest.SetUniqAttribute ("some", "4000:190A");
  std::cout << std::endl << nameTest << std::endl;

  std::cout << "path count:" << nameTest.GetPathCount () << std::endl;
  std::cout << "opt count :" << nameTest.GetAttributeCount () << std::endl;
  std::cout << "path      :" << nameTest.GetPathName () << std::endl;
  std::cout << "path2      :" << nameTest.GetPath () << std::endl;
  std::cout << "path2      :" << nameTest.GetPath () << std::endl;
  //std::cout << "path seg2   :" << nameTest.GetNthPathSegment (2) << std::endl;
  std::string testStr;
  bool result = nameTest.GetUniqAttribute ("awesome", testStr);

  std::cout << "awesome is (" << result << ")  :" << testStr << std::endl;
  result = nameTest.GetUniqAttribute ("some", testStr);
  std::cout << "some is (" << result << ")  :" << testStr << std::endl;
  result = nameTest.GetUniqAttribute ("cuky", testStr);
  std::cout << "cuky is (" << result << ")  :" << testStr << std::endl;

  IcnName < std::string > test2;
  IcnName < std::string > test3;
  IcnName < std::string > test4;
  test2.SetFullName (nameTest.GetFullName ());
  test2.SetAttributeDelim ("~");
  test2.SetPathDelim ("|");

  std::cout << std::endl << test2 << std::endl;

  test2.SetPathName ("|jack/joe|and|baby");
  std::cout << std::endl << test2 << std::endl;
  test4.SetFullName ("/jack/joe/and baby/dude=lady/sam=smart/me=lazy");


  //Packet Test
  PktType content1 (nameTest);
  content1.SetPacketPurpose (PktType::DATAPKT);
  content1.SetObjectCpyNamedAttribute("test1", 3.14159);
std::cout << "packet:\n" << content1 << "\n";
  int x[]={1,2,3,4,5};
  //content1.SetObjectCpyNamedAttribute("test2", x, true, x.size()*sizeof(int));
  int *z = new int [5] ;
//  content1.GetObjectCpyNamedAttribute("test2", *z, true, 5*sizeof(int));
  delete z;

  //Utility Test
 //create a block to test LRU temporal
  for (unsigned int i=0; i<6 ; i++) {
     PktType nextPacket = content1;
     nextPacket.SetPacketPurpose (PktType::DATAPKT);
      IcnName < std::string > nextName = nameTest;
      nextName.SetUniqAttribute ("part", i);
      nextPacket.SetName (nextName);

  }
  {
  PktType nextPacket = content1;
  nextPacket.SetPacketPurpose (PktType::INTERESTPKT);
  IcnName < std::string > nextName = nameTest;
  nextName.SetUniqAttribute ("part", 3);
  nextPacket.SetName (nextName);
  }
 {
  IcnName < std::string > nextName = nameTest;
  nextName.SetUniqAttribute ("part", 4);
  }

  {
  PktType nextPacket = content1;
  nextPacket.SetPacketPurpose (PktType::DATAPKT);
  IcnName < std::string > nextName = nameTest;
  nextName.SetUniqAttribute ("part", 2);
  nextPacket.SetName (nextName);
  }

  //read back packets
  std::cout.unsetf (std::ios::floatfield); // floatfield not set
  std::cout.precision (15);
  std::cout.setf (std::ios::floatfield);
  std::vector <double> results1(6);
 

}
