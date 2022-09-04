#include"stdafx.h"
#include"SoundHelper.h"
#include"WaveDatas.h"
#include <xaudio2.h>	
#include <xaudio2fx.h>
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "xaudio2.lib" )
#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif
using namespace::ButiEngine;

#ifdef _XBOX //Big-Endian#define fourccRIFF 'RIFF'#define fourccDATA 'data'#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'#endif#ifndef _XBOX //Little-Endian#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'#define fourccXWMA 'AMWX'#define fourccDPDS 'sdpd'
#endif
struct Chunk
{
	char	id[4]; // �`�����N����ID
	int32_t	size;  // �`�����N�T�C�Y
};

// RIFF�w�b�_�[
struct RiffHeader
{
	Chunk	chunk;   // "RIFF"
	char	type[4]; // "WAVE"
};

// FMT�`�����N
struct FormatChunk
{
	Chunk		chunk; // "fmt "
	WAVEFORMAT	fmt;   // �g�`�t�H�[�}�b�g
};
Value_ptr<ButiSound::WaveDatas> ButiSound::SoundHelper::CreateFromBinary(Value_ptr<IBinaryReader> arg_reader)
{
	//auto outsound = make_value<Sound>();
	XAUDIO2_BUFFER buffer = { 0 };

	HRESULT hr;

#ifdef _XBOX
	char* strFileName = "game:\\media\\filePath";
#else
#endif
	// RIFF�w�b�_�[�̓ǂݍ���
	auto riff = arg_reader->ReadVariable<RiffHeader>();

	// Format�`�����N�̓ǂݍ���
	auto format = arg_reader->ReadVariable<FormatChunk>();

	// Data�`�����N�̓ǂݍ���
	if (!arg_reader->Skip("data")) {
		arg_reader->Skip("DATA");
	}
	auto data = arg_reader->ReadVariable<Chunk>();

	// Data�`�����N�̃f�[�^���i�g�`�f�[�^�j�̓ǂݍ���
	char* pBuffer = (char*)arg_reader->ReadData(data.size);

	WAVEFORMATEX wfex{};
	// �g�`�t�H�[�}�b�g�̐ݒ�
	memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;

	buffer.pAudioData = (BYTE*)pBuffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = data.size;

	return make_value<WaveDatas>(buffer, wfex);
}
