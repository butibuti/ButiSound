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
	char	id[4]; // チャンク毎のID
	int32_t	size;  // チャンクサイズ
};

// RIFFヘッダー
struct RiffHeader
{
	Chunk	chunk;   // "RIFF"
	char	type[4]; // "WAVE"
};

// FMTチャンク
struct FormatChunk
{
	Chunk		chunk; // "fmt "
	WAVEFORMAT	fmt;   // 波形フォーマット
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
	// RIFFヘッダーの読み込み
	auto riff = arg_reader->ReadVariable<RiffHeader>();

	// Formatチャンクの読み込み
	auto format = arg_reader->ReadVariable<FormatChunk>();

	// Dataチャンクの読み込み
	if (!arg_reader->Skip("data")) {
		arg_reader->Skip("DATA");
	}
	auto data = arg_reader->ReadVariable<Chunk>();

	// Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = (char*)arg_reader->ReadData(data.size);

	WAVEFORMATEX wfex{};
	// 波形フォーマットの設定
	memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;

	buffer.pAudioData = (BYTE*)pBuffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = data.size;

	return make_value<WaveDatas>(buffer, wfex);
}
