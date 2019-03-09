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

#include "BaseFileStorage.h"
#include <assert.h>


BaseFileStorage::BaseFileStorage() { m_size = 14000;}
BaseFileStorage::BaseFileStorage(uint32_t size) { m_size = size;}
    BaseFileStorage::~BaseFileStorage() {}
   
    void BaseFileStorage::SetMaxSize(uint32_t size) { m_size = size;} 
    void BaseFileStorage::SetFileSize(const dataNameType_t &name, uint32_t size) {
        auto it = m_fileMap.find(name);
        if (it == m_fileMap.end()) {
            FileInfo_t *fi = new FileInfo_t;
            fi->size = size;
            fi->buffer = new uint8_t[size];
	    //not space efficient, just temp to make it work
            fi->validBytes = new uint8_t[size];
	    for(uint32_t i=0; i< size; i++) {
		fi->validBytes[i]=0;
	    }
            m_fileMap.insert( std::make_pair(name,fi));
        }
        
    }
    //void SetFileStorageType();  //FIXME TODO change memory or disk based storage
    bool BaseFileStorage::SetDataRange(const dataNameType_t &name,uint32_t start, uint32_t stop, const std::vector<uint8_t> &data) 
    {
	    bool retVal = false;
       auto it = m_fileMap.find(name);
        if (it == m_fileMap.end()) { 
            SetFileSize(name, m_size );
        it = m_fileMap.find(name);
	} //should exist
	else { retVal = true; }
            auto fi = it->second;
            for(auto i = start; i<=stop; i++) {
                fi->buffer[i]=data[i-start];
                fi->validBytes[i]= 1;
        }
	return retVal;
    }

    bool BaseFileStorage::GetDataRange(const dataNameType_t &name,uint32_t start, uint32_t stop, std::vector<uint8_t> &data) {
        auto it = m_fileMap.find(name);
        if (it == m_fileMap.end()) { return false; } //not found
        else {
            auto fi = it->second;
            for(auto i = start; i<=stop; i++) {
                data[i-start] = fi->buffer[i];
                if (!(fi->validBytes[i]))
                    return false; //incomplete data
            }
	    return true;
            
        }   
	return false;
    }

    bool BaseFileStorage::DeleteFile(const dataNameType_t &name) {
        auto it = m_fileMap.find(name);
        if (it != m_fileMap.end()) {
            delete [](it->second->buffer);
            delete [](it->second->validBytes);
            m_fileMap.erase(it);
	    return true;
        }
	return false;
    }
