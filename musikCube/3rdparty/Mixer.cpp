//////////////////////////////////////////////////////////////////////
// Volume Controler
// by Whoo(whoo@isWhoo.com)
// Oct.1 2001

//////////////////////////////////////////////////////////////////////
// Mixer.cpp: implementation of the CMixer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "send.h"
#include "Mixer.h"

#include <mmsystem.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



CMixer::~CMixer()
{

}

#undef FAILED
#define FAILED(X) (MMSYSERR_NOERROR != X)
/////////////////////////////////////////////////
//  Component: value from dwComponentType member of MIXERLINE, eg://	MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE
//  Kind : 0 Wave ouput tunnle, 1 Wave input tunle, 2... other. 
CMixer::CMixer(DWORD ComponentType, DestKind dkKind): m_dwControlID(-1), m_bOK(false), m_dwChannels(0)
{
	HMIXER hMixer;
	HRESULT hr;
	hr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (FAILED(hr)) return;

	MIXERLINE mxl;
	MIXERCONTROL mc;
	MIXERLINECONTROLS mxlc;
	DWORD kind, count;

	if (dkKind == Play)
		kind = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
	else 
		kind = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = kind;

    hr = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
	if (FAILED(hr))
	{
		mixerClose(hMixer);
		return;
	}

	count = mxl.dwSource;
	for(UINT i = 0; i < count; i++)
	{
		mxl.dwSource = i;
		mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_GETLINEINFOF_SOURCE);
		if (mxl.dwComponentType == ComponentType)
		{
			m_dwChannels = mxl.cChannels;
			mc.cbStruct = sizeof(mc);
			mxlc.cbStruct = sizeof(mxlc);
			mxlc.dwLineID = mxl.dwLineID;
			mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
			mxlc.cControls = 1;
			mxlc.cbmxctrl = sizeof(MIXERCONTROL);
			mxlc.pamxctrl = &mc;
			hr = mixerGetLineControls((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);
			m_dwControlID = mc.dwControlID;
			break;
		};
	}
	mixerClose(hMixer);
	m_bOK = true;
}

void CMixer::SetVolume(DWORD dwVol)
{
	if (!m_bOK) return;
	HMIXER hMixer;
	HRESULT hr;
	hr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (FAILED(hr)) return;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED mxdu;

	mxdu.dwValue = dwVol;

	mxcd.cMultipleItems = 0;
	mxcd.cChannels = m_dwChannels;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = m_dwControlID;
	mxcd.cbDetails = sizeof(mxdu);
	mxcd.paDetails = &mxdu;
	hr = mixerSetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_SETCONTROLDETAILSF_VALUE);	
	
	mixerClose(hMixer);
}

DWORD CMixer::GetVolume()
{
	if (!m_bOK) return 0;
	HMIXER hMixer;
	HRESULT hr;
	hr = mixerOpen(&hMixer, 0, 0, 0, 0);
	if (FAILED(hr)) return 0;

	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED mxdu;


	mxcd.cMultipleItems = 0;
	mxcd.cChannels = m_dwChannels;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = m_dwControlID;
	mxcd.cbDetails = sizeof(mxdu);
	mxcd.paDetails = &mxdu;
	hr = mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);	
	
	mixerClose(hMixer);
	return mxdu.dwValue;
}
