#pragma once

// =======================================================================================
//                                      Sound
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	Contains functionality to wrap XAudio2 source voice info.
///        
/// # Sound
/// Detailed description.....
/// Created on: 7-12-2012 
///---------------------------------------------------------------------------------------
#include <XAudio2fx.h>
#include <XAudio2.h>
#include "XAudio2Exception.h"

class Sound
{
public:
	Sound(IXAudio2SourceVoice* p_sourceVoice, const XAUDIO2_BUFFER& p_buffer);
	virtual ~Sound();

	///-----------------------------------------------------------------------------------
	/// Stops the source voice and also flushes the source buffers
	/// \returns HRESULT
	///-----------------------------------------------------------------------------------
	HRESULT stop();

	///-----------------------------------------------------------------------------------
	/// Pauses the sound but doesn't clear the source buffers
	/// \returns HRESULT
	///-----------------------------------------------------------------------------------
	HRESULT pause();

	///-----------------------------------------------------------------------------------
	/// Submits the original buffer and restarts the sound
	/// \returns HRESULT
	///-----------------------------------------------------------------------------------
	HRESULT restart();

	///-----------------------------------------------------------------------------------
	/// Only asks the source voice to begin playing it's sound no matter where the 
	/// buffer pointer. If the sound has never been played the buffer location will be at
	/// the begining.
	/// \returns HRESULT
	///-----------------------------------------------------------------------------------
	HRESULT resumeOrPlay();
protected:
	XAUDIO2_BUFFER			m_buffer;
	IXAudio2SourceVoice*	m_sourceVoice;
};