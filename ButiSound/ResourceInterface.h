#ifndef RESOURCE_INTERFACE_H
#define RESOURCE_INTERFACE_H


#ifdef BUTISOUND_EXPORTS
#define BUTISOUND_API __declspec(dllexport)
#else
#define BUTISOUND_API __declspec(dllimport)
#endif

#pragma once
#include"ButiMemorySystem/ButiMemorySystem/ButiPtr.h"
namespace ButiEngine {
class IBinaryReader;
}

namespace ButiSound {
class WaveDatas;
class IResource_Sound {
public:
	virtual ButiEngine::Value_ptr<WaveDatas> GetData()=0;
};
using Resource_Sound_t = ButiEngine::Value_ptr<IResource_Sound>;
class ISoundObject {
public:
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual void Destroy() = 0;
	virtual void SetVolume(const float arg_volume) = 0;
	virtual float GetVolume() const = 0;
	virtual bool IsLoop() const = 0;
	virtual void ExitLoop() const = 0;
	virtual bool IsEnd() const = 0;
};
using SoundObject_t = ButiEngine::Value_ptr<ISoundObject>;
class ISoundManager {
public:
	virtual void Update() = 0;
	/// <summary>
	/// SEを再生する
	/// </summary>
	/// <param name="arg_sound">SEのリソース</param>
	/// <param name="arg_volume">音量</param>
	virtual SoundObject_t PlaySE(Resource_Sound_t arg_sound, const float arg_volume, const bool arg_isLoop = false) = 0;
	/// <summary>
	/// BGMを再生する
	/// </summary>
	/// <param name="arg_sound">BGMのリソース</param>
	/// <param name="arg_volume">音量</param>
	virtual SoundObject_t PlayBGM(Resource_Sound_t arg_sound, const  float arg_volume) = 0;
	/// <summary>
	/// マスターボリュームの再生
	/// </summary>
	/// <param name="arg_masterVolume"></param>
	virtual void SetMasterVolume(const float arg_masterVolume) = 0;
	/// <summary>
	/// マスターボリュームの取得
	/// </summary>
	/// <returns></returns>
	virtual float GetMasterVolume()const = 0;
	/// <summary>
	/// 全てのSEを停止する
	/// </summary>
	virtual void StopSE() = 0;
	/// <summary>
	/// BGMを停止する
	/// </summary>
	virtual void StopBGM() = 0;
	/// <summary>
	/// SEを開始する
	/// </summary>
	virtual void RestartSE() = 0;
	/// <summary>
	/// BGMを開始する
	/// </summary>
	virtual void RestartBGM() = 0;
	/// <summary>
	/// SEを終了させる
	/// </summary>
	virtual void DestroySE() = 0;
	/// <summary>
	/// BGMを終了させる
	/// </summary>
	virtual void DestroyBGM() = 0;
	/// <summary>
	/// BGMの音量を設定する
	/// </summary>
	/// <param name="volume"></param>
	virtual void SetBGMVolume(const float volume) = 0;
	/// <summary>
	/// 解放処理
	/// </summary>
	virtual void Release() = 0;
	/// <summary>
	/// 現在再生しているBGMの取得
	/// </summary>
	/// <returns>現在再生しているBGM</returns>
	virtual Resource_Sound_t GetCurrentPlayBGM()const = 0;
	/// <summary>
	/// BGMの音量の取得
	/// </summary>
	/// <returns>音量</returns>
	virtual float GetBGMVolume()const = 0;
};
BUTISOUND_API ButiEngine::Value_ptr<IResource_Sound> CreateSound(ButiEngine::Value_ptr<ButiEngine::IBinaryReader> arg_reader);
BUTISOUND_API ButiEngine::Value_ptr<ISoundManager> CreateSoundManager();
}

#endif // !RESOURCE_INTERFACE_H