#ifndef WAVEDATA_H
#define WAVEDATA_H
#pragma once
#include <xaudio2.h>	
namespace ButiSound {
struct WaveDatas 
{
public:
	WaveDatas(const XAUDIO2_BUFFER& arg_buffer, const WAVEFORMATEX& arg_format) :m_buffer(arg_buffer), m_format(arg_format) {};
	~WaveDatas(){delete m_buffer.pAudioData;}
	XAUDIO2_BUFFER m_buffer;
	WAVEFORMATEX m_format;
};
}

#endif // !WAVEDATA_H