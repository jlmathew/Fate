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
#ifndef UTILITYFUNCTIONGENERATOR_H_
#define UTILITYFUNCTIONGENERATOR_H_

#include "UtilityHandlerBase.h"
class ModuleManager;

#include "UtilityConfigBase.h"
#include "ModuleManager.h"
#include "StoreManager.h"
#include <string>
#include <exception>
#include <stdexcept>
//#include "PacketTypeBase.h"




class UtilityGenerator
{
public:
  //UtilityGenerator();
  //~UtilityGenerator(); 
  static UtilityHandlerBase *CreateNewUtility (ConfigWrapper & config);
private:                       //only 1 copy, this is a factory/generator (singleton)
    UtilityGenerator ();
   ~UtilityGenerator ();
};

class ModuleGenerator
{
public:
  //ModuleGenerator();
  //~ModuleGenerator(); 
  static ModuleManager *CreateNewModule (ConfigWrapper & config);
private:                       //only 1 copy, this is a factory/generator (singleton)
    ModuleGenerator ()
  {
  }
  virtual ~ ModuleGenerator ()
  {
  }
};

class StoreGenerator
{
public:
  //ModuleGenerator();
  //~UtilityGenerator(); 
  static StoreManager *CreateNewStore (ConfigWrapper & config);
private:                       //only 1 copy, this is a factory/generator (singleton)
    StoreGenerator ()
  {
  }
  virtual ~ StoreGenerator ()
  {
  }
};


class UniqueStore
{
public:
  static StoreManager *GetGlobalStore (std::string key);
  static StoreManager *RegisterStore (std::string key, StoreManager * val);
private:                       //only 1 copy, this is a factory/generator (singleton)
    UniqueStore ()
  {
  }
  virtual ~ UniqueStore ()
  {
  }
  static RegisterTemplate < std::string, StoreManager * >m_allStores;
};

#endif
