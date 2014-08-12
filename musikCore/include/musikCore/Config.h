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

#if !defined(__MUSIKCORE_CONFIG_H__)
#define __MUSIKCORE_CONFIG_H_

///////////////////////////////////////////////////

#include <musikCore/Conversion.h>

///////////////////////////////////////////////////

#ifndef DLLEXPORT
    #if defined(WIN32)
        #define DLLEXPORT __declspec(dllexport)
    #else
        #define DLLEXPORT
    #endif
#endif

///////////////////////////////////////////////////

#if !defined(UNICODE) && !defined(_UNICODE)
    #define UNICODE
    #define _UNICODE
#endif

///////////////////////////////////////////////////

#if !defined(WIN32) && !defined(__MINGW32__)
    #define USE_XINE
    #undef USE_BASS
#else
    #define USE_BASS
    #undef USE_XINE
#endif

///////////////////////////////////////////////////

#if defined(WIN32) && !defined(assert)
    #define assert(x) ASSERT(x)
#endif

///////////////////////////////////////////////////

#if defined (WIN32) && !defined(__MINGW32__)
    #pragma warning (disable : 4311)
    #pragma warning (disable : 4267)
    #pragma warning (disable : 4244)
    #pragma warning (disable : 4251)    // export warnings
    #pragma warning (disable : 4275)    // export warnings
#endif

///////////////////////////////////////////////////

// If we're using MSVC we may need to create some mappings.

#if !defined(WIN32) || defined(__MINGW32__)
    #if !defined(__MINGW32__)
        #include <unistd.h>
    #endif

    #if !defined(TRUE)
        #define TRUE 1
    #endif

    #if !defined(FALSE)
        #define FALSE 0
    #endif

    #if !defined(UNICODE) && !defined(_UNICODE)
        #define _T(x) x
    #else
        #if !defined(_T)
            #define _T(x) L ## x
        #endif
    #endif

    #if !defined(__MINGW32__) && !defined(Sleep)
        #define Sleep(x) usleep(x * 1000)
    #endif

    #if !defined(ASSERT)
        #define ASSERT(x) assert(x)
    #endif
#endif

///////////////////////////////////////////////////

#endif  // #define(__MUSIKCORE_CONFIG_H__)

///////////////////////////////////////////////////
