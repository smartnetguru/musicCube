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
#include <musikCore/EqualizerSettings.h>

///////////////////////////////////////////////////

namespace musik { namespace Core {

    float s_PreampLevel = 48.0;
    float GetEQPreampLevel(){ return musik::Core::s_PreampLevel; }
    void SetEQPreampLevel(float level){ musik::Core::s_PreampLevel = level; }


    int g_EQFreqs[18] =
        { 55, 77, 110, 156, 220, 311, 
        440, 622, 880, 1244, 1760, 2489, 
        3520, 4978, 7040, 10056, 14080, 19912 };

    int* GetEQFreqs(){ return g_EQFreqs; }

} } // namespace

///////////////////////////////////////////////////

using namespace musik::Core;

float s_PreAmpLevel = 48.0;

///////////////////////////////////////////////////

EqualizerSettings::EqualizerSettings()
{
    m_ID = -1;

    for (size_t i = 0; i < EqualizerBandCount; i++)
    {
        m_Left[i] = 48.0f;
        m_Right[i] = 48.0f;
    }

    musik::Core::SetEQPreampLevel(96.0);
}

///////////////////////////////////////////////////

void EqualizerSettings::Set(EqualizerBand which_band, float values[])
{
    for (size_t i = 0; i < EqualizerBandCount; i++)
    {
        switch (which_band)
        {
        case EqualizerBandLeft:
            m_Left[i] = values[i];
            break;

        case EqualizerBandRight:
            m_Right[i] = values[i];
            break;

        case EqualizerBandBoth:
            m_Left[i] = values[i];
            m_Right[i] = values[i];
            break;
        }
    }
}

///////////////////////////////////////////////////

