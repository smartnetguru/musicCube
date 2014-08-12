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

#if !defined(__MUSIKCORE_CONVERSION_H__)
#define __MUSIKCORE_CONVERSION_H__

///////////////////////////////////////////////////

#include <musikCore/Config.h>
#include <musikCore/StdString.h>
#include <vector>
#include <sstream>

using namespace std;

///////////////////////////////////////////////////

namespace musik { namespace Core {

///////////////////////////////////////////////////

typedef CStdString String;
typedef std::vector<String> StringArray;
typedef std::vector<int> IntArray;

///////////////////////////////////////////////////

extern DLLEXPORT std::wstring utf8to16(const std::string& src);
extern DLLEXPORT std::string utf16to8(const String& src, bool unicode = true);
extern DLLEXPORT String q_str(String src);
extern DLLEXPORT String IntToString(int n);
extern DLLEXPORT int StringToInt(const string& str);

///////////////////////////////////////////////////

template <typename T>
inline int              StringToInt(const std::basic_string<T>& source)
{
    std::basic_istringstream<T> stringStream;
    stringStream.str(source.c_str());

    int result;
    stringStream >> result;

    return result;
}

template <typename T>
inline int              StringToInt(const T* source)
{
    return StringToInt(std::basic_string<T>(source));
}

inline int              StringToInt(const String& source)
{
    std::basic_istringstream<String::value_type> stringStream(source);

    int result;
    stringStream >> result;

    return result;
}

template <typename T>
inline double           StringToFloat(const T* source)
{
    return StringToFloat(std::basic_string<T>(source));
}

template <typename T>
inline double           StringToFloat(const std::basic_string<T>& source)
{
    std::basic_istringstream<T> stringStream(source);

    double result;
    stringStream >> result;

    return result;
}

///////////////////////////////////////////////////

} } // namespace

///////////////////////////////////////////////////

#endif  // #define(__MUSIKCORE_CONVERSION_H__)

///////////////////////////////////////////////////