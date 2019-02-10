/*
MIT (Modified) License

Copyright (c) 2017-2019 jlmathew

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


/* 
 * File:   BaseFileStorage.h
 * Author: james mathewson
 *
 * Created on February 3, 2019, 11:49 AM
 */

#ifndef BASEFILESTORAGE_H
#define BASEFILESTORAGE_H

#include <vector>
#include "ContentName.h"
#include <cstdint>
#include <stdint.h>
#include <map>
//FIXME TODO NOTE TO SELF: quickie implementation, should be standardized and 
//merged with BaseStorage.h and StoreManager, to allow easier configurations
struct FileInfo_t
{
    uint32_t size;
    uint8_t *buffer;
    //wasteful, but quick
    uint8_t *validBytes;
    
};
class BaseFileStorage
{
    public:
    BaseFileStorage();
    virtual ~BaseFileStorage();
    
    void SetFileSize(const dataNameType_t &name, uint32_t size);
    //void SetFileStorageType();  //FIXME TODO change memory or disk based storage
    bool SetDataRange(const dataNameType_t &name,uint32_t start, uint32_t stop, const std::vector<uint8_t> &data);
    bool GetDataRange(const dataNameType_t &name,uint32_t start, uint32_t stop, std::vector<uint8_t> &data);
    bool DeleteFile(const dataNameType_t &name);
    //bool DeletePartialFile(const &name);
private:
    std::map< dataNameType_t, FileInfo_t *> m_fileMap;

        

};

#endif /* BASEFILESTORAGE_H */

