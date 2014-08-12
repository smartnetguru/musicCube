///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright © 2002-2006, Casey Langen
//
// Sources and Binaries of: musikCore musikCube musikBox
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may 
//      be used to endorse or promote products derived from this software 
//      without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include <musikCore/Conversion.h>
#include <musikCore/unicode.h>

#include <string>

///////////////////////////////////////////////////

using namespace musik::Core;

///////////////////////////////////////////////////

std::wstring            musik::Core::utf8to16(const std::string& src)
{
    size_t bufferSize = src.size() + 1;
    Unicode::UTF8  *sourceBuffer = new Unicode::UTF8[bufferSize];
    Unicode::UTF16 *targetBuffer = new Unicode::UTF16[bufferSize];

    size_t i = 0;
    for(; i < src.size(); i++)
      sourceBuffer[i] = Unicode::UTF8(src[i]);
    sourceBuffer[i] = 0;

    const Unicode::UTF8 *source = sourceBuffer;
    Unicode::UTF16 *target = targetBuffer;

    Unicode::ConversionResult result =
      Unicode::ConvertUTF8toUTF16(&source, sourceBuffer + bufferSize,
                                  &target, targetBuffer + bufferSize,
                                  Unicode::lenientConversion);

    if(result != Unicode::conversionOK)
    {
        return std::wstring();  // should throw
    }

    int newSize = (target != targetBuffer) ? (target - targetBuffer - 1) : 0;

    std::wstring ret;
    ret.resize(newSize);

    for(i = 0; i < (unsigned int)newSize; i++)
      ret[i] = targetBuffer[i];

    delete [] sourceBuffer;
    delete [] targetBuffer;

    return ret;
}

std::string             musik::Core::utf16to8(const String& src, bool unicode)
{
    std::string ret;
    ret.resize(src.size());

    if(!unicode) 
    {
        for (size_t i = 0; i < src.size(); i++)
        {
            ret[i] = (char)src[i];
        }

        return ret;
    }

    const size_t outputBufferSize = src.size() * 3 + 1;

    Unicode::UTF16 *sourceBuffer = new Unicode::UTF16[src.size() + 1];
    Unicode::UTF8  *targetBuffer = new Unicode::UTF8[outputBufferSize];

    // take the wstring and build a valid UTF16
    for(unsigned int i = 0; i < src.size(); i++)
        sourceBuffer[i] = Unicode::UTF16(src[i]);

    const Unicode::UTF16 *source = sourceBuffer;
    Unicode::UTF8 *target = targetBuffer;

    Unicode::ConversionResult result =
        Unicode::ConvertUTF16toUTF8(&source, sourceBuffer + src.size(),
                                     &target, targetBuffer + outputBufferSize,
                                     Unicode::lenientConversion);

    if(result != Unicode::conversionOK)
        printf("utf16to8() failled\n");

    int newSize = target - targetBuffer;
    ret.resize(newSize);
    targetBuffer[newSize] = 0;

    ret = (char *) targetBuffer;

    delete [] sourceBuffer;
    delete [] targetBuffer;

    return ret;
}

inline String           musik::Core::q_str(String src)
{
    src.Replace(_T("'"), _T("''"));
    return src;
}


inline String           musik::Core::IntToString(int n)
{
    ostringstream aStream;
    aStream << n << "\0";

    string str = aStream.str().c_str();
    return str;
}