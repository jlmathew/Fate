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


#ifndef GLOBALMODULEBASE_H_
#define GLOBALMODULEBASE_H_
#include <time.h>
#include <sys/types.h>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <map>
#include <cstdlib>
#include <ostream>
#include <stdexcept>
#include <string>
#include <sstream>

class NodeManager;

typedef timespec timer_struct_t ;
class GlobalModuleTimer
{
public:
  virtual timer_struct_t GetTime () = 0;
  virtual int NanoSleep (const timer_struct_t & timeAdv) = 0;
  static double Ratio (const timer_struct_t & oldTime, const timer_struct_t & newestTime, const timer_struct_t & interTime);
  static signed int TimeCompare (timer_struct_t & a, timer_struct_t & b);       // -1, a < b, 0 a== b, 1 a > b


};

class GlobalModuleLog
{
public:
  virtual void log (const char *a, ...) = 0;

  template < typename T > void tLog (T t)
  {
  }

};

class CoutModuleLog:public GlobalModuleLog
{

  virtual void log (const char *a, ...);

  template < typename T > void tLog (T t)
  {
    std::cout << t;
  }
};

class NullLog:public GlobalModuleLog
{

  virtual void log (const char *a, ...);
};

//need their own module
class GlobalModule
{
public:
  //uint64_t GetTimeAsInt(); 
  //virtual double GetTimeAsDbl(); 
  GlobalModuleTimer * GetGlobalTimer ();
  void SetGlobalTimer (GlobalModuleTimer *);

  GlobalModuleLog *GetGlobalLog (const std::string & name);
  void SetGlobalLog (const std::string & name, GlobalModuleLog *);


void *dnsEntry; //fix to make generic, not ns3 specific

private:
    GlobalModuleTimer * m_timer;
    std::map < std::string, GlobalModuleLog * >m_logs;

};

#endif
