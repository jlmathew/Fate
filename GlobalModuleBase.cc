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


#include "GlobalModuleBase.h"
#include <cstdlib>
#include <stdint.h>
#include <cstdarg>


signed int
GlobalModuleTimer::TimeCompare (timer_struct_t & a, timer_struct_t & b)
{
  if (a.tv_sec == b.tv_sec)
    {
      if (a.tv_nsec == b.tv_nsec)
        {
          return 0;             // a == b
        }
      else if (a.tv_nsec < b.tv_nsec)
        {
          return -1;            // a < b
        }
      else
        {
          return 1;
        }                       // a > b

    }
  else if (a.tv_sec < b.tv_sec)
    {
      return -1;                //a < b
    }
  else
    {
      return 1;                 //a > b
    }
}

//return a double = nowTime - startTime / (expireTime - startTime)
// newesttime = no
double
GlobalModuleTimer::Ratio (const timer_struct_t & nowTime, const timer_struct_t & expireTime, const timer_struct_t & startTime)
{
  double top = (nowTime.tv_sec - startTime.tv_sec) + (nowTime.tv_nsec + 1 - startTime.tv_nsec) / 1000000000.0;
  double bottom = (expireTime.tv_sec - startTime.tv_sec) + (expireTime.tv_nsec + 1 - startTime.tv_nsec) / 1000000000.0;

  return top / bottom;
}

//need their own module
//uint64_t GlobalModule::GetTimeAsInt() {}; 
//double GlobalModule::GetTimeAsDbl() {}; 
void
GlobalModule::SetGlobalLog (const std::string & name, GlobalModuleLog * log)
{
  m_logs[name] = log;
}

GlobalModuleLog *
GlobalModule::GetGlobalLog (const std::string & name) 
{
  return m_logs[name];          //Check for error!
}

GlobalModuleTimer *
GlobalModule::GetGlobalTimer ()
{
  return m_timer;
}


void
GlobalModule::SetGlobalTimer (GlobalModuleTimer * timer)
{
  m_timer = timer;
}


//FIXME TODO, change to a more modern C++ << like ns3 uses.
void
CoutModuleLog::log (const char *a, ...)
{
  va_list (ap);
  va_start (ap, a);
  vprintf (a, ap);
  va_end (ap);

}
