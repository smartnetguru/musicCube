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

#if !defined(__MUSIKCORE_EQUALIZER_SETTINGS_H__)
#define __MUSIKCORE_EQUALIZER_SETTINGS_H__

///////////////////////////////////////////////////

#include <musikCore/Config.h>

///////////////////////////////////////////////////

namespace musik { namespace Core {

///////////////////////////////////////////////////

DLLEXPORT enum EqualizerBand
{
    EqualizerBandLeft = 0,
    EqualizerBandRight,
    EqualizerBandBoth
};

static const int EqualizerBandCount = 18;

///////////////////////////////////////////////////

extern int g_EQFreqs[EqualizerBandCount];
DLLEXPORT int* GetEQFreqs();
extern float s_PreampLevel;
DLLEXPORT float GetEQPreampLevel();
DLLEXPORT void SetEQPreampLevel(float level);

///////////////////////////////////////////////////

/**
Similar to CrossfadeSettings, EqualizerSettings is an object that 
contains equalizer settings. Library can be used to manage 
presets of them. Player uses them for settings.

@version alpha 0.7
*/
class DLLEXPORT EqualizerSettings
{
public:

    /**
     * Default constructor.
     */
    EqualizerSettings();
    ~EqualizerSettings(){}

    /**
     * Sets the equalizer band settings.
     *
     * \param which EqualizerBandLeft, EqualizerBandRight, or EqualizerBandBoth
     * \param values[EqualizerBandCount] the band settings, specified as an array of floats
     */
    void Set(EqualizerBand which_band, float values[EqualizerBandCount]);
    /**
     * Left channel settings.
     */
    float m_Left[EqualizerBandCount];
    /**
     * Right channel settings.
     */
    float m_Right[EqualizerBandCount];
    /**
     * String name of the preset
     */
    String m_Name;
    /**
     * ID of the equalizer according to Library
     */
    int m_ID;
};

///////////////////////////////////////////////////

} }     // namespace musik::Core

///////////////////////////////////////////////////

#endif  // #if !defined(__MUSIKCORE_EQUALIZER_SETTINGS_H__)

///////////////////////////////////////////////////
