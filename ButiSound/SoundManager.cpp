#include"stdafx.h"
#include<wrl.h>
#include<xaudio2.h>
#include<vector>
#include"ButiUtil/ButiUtil/Util.h"
#include"ButiUtil/ButiUtil/Timer.h"
#include"WaveDatas.h"
namespace ButiSound {
class SoundObject :public ISoundObject{
public:
	SoundObject(Resource_Sound_t arg_sound, const float arg_volume, const bool arg_isIsolate,const bool arg_isLoop) {
		m_vlp_data = arg_sound->GetData();
		isLoop = arg_isLoop;
		if (isLoop) {
			m_vlp_data->m_buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
		}
		m_volume = arg_volume;
	}
	bool Initialize(Microsoft::WRL::ComPtr< IXAudio2>& arg_xAudio) {
		HRESULT hr = arg_xAudio->CreateSourceVoice(&m_p_sourceVoice, &m_vlp_data->m_format);
		if (hr != S_OK) { return false; }
		hr = m_p_sourceVoice->SubmitSourceBuffer(&(m_vlp_data->m_buffer));
		if (hr != S_OK) { return false; }
		m_p_sourceVoice->SetVolume(m_volume);
		return true;
	}
	void Start() override {
		if (!m_p_sourceVoice)return;
		m_p_sourceVoice->Start();
	}
	void Stop() override {
		if (!m_p_sourceVoice)return;
		m_p_sourceVoice->Stop();
	}
	void Destroy() override
	{
		if (!m_p_sourceVoice)return;
		m_p_sourceVoice->DestroyVoice();
		m_p_sourceVoice = nullptr;
	}
	void SetVolume(const float arg_volume) override {
		if (!m_p_sourceVoice)return;
		m_p_sourceVoice->SetVolume(arg_volume);
		m_volume = arg_volume;
	}
	float GetVolume() const override {
		return m_volume;
	}
	bool IsLoop() const override {
		return isLoop;
	}
	void ExitLoop() const override {
		if (!m_p_sourceVoice)return;
		m_p_sourceVoice->ExitLoop();
	}
	bool IsEnd() const override {
		if (!m_p_sourceVoice)return true;
		XAUDIO2_VOICE_STATE state;
		m_p_sourceVoice->GetState(&state);
		return !state.BuffersQueued;
	}
	
private:
	IXAudio2SourceVoice* m_p_sourceVoice = nullptr;
	bool isIsolate = false,isLoop=false;
	float m_volume;
	ButiEngine::Value_ptr<WaveDatas> m_vlp_data;
};

class SoundManager :public ISoundManager {
public:
	void Initialize();
	void Update()override;
	void ClearCheck();

	SoundObject_t PlaySE(Resource_Sound_t arg_sound, const float arg_volume, const bool arg_isLoop) override;
	
	SoundObject_t PlayBGM(Resource_Sound_t arg_sound, const  float arg_volume) override;
	
	void SetMasterVolume(const float arg_masterVolume) override;
	
	float GetMasterVolume()const override { return m_masterVolume; }
	
	void StopSE() override;
	
	void StopBGM() override;
	
	void RestartSE() override;
	
	void RestartBGM() override;
	
	void DestroySE() override;
	
	void DestroyBGM() override;
	
	void SetBGMVolume(const float volume) override;
	
	void Release() override;
	
	Resource_Sound_t GetCurrentPlayBGM()const override;
	
	float GetBGMVolume()const override;
private:
	Resource_Sound_t currentBGM;
	SoundObject_t m_bgm= nullptr;
	IXAudio2MasteringVoice* p_masterVoice = nullptr;
	Microsoft::WRL::ComPtr< IXAudio2> cmp_pXAudio2 = nullptr;
	std::vector<SoundObject_t> vec_seVoices;
	float m_masterVolume = 1.0f;
	ButiEngine::Value_ptr<ButiEngine::RelativeTimer> m_vlp_clearTimer;
};


void SoundManager::Initialize()
{
	CoInitialize(NULL);
	HRESULT hr;
	if (FAILED(hr = XAudio2Create(cmp_pXAudio2.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR))) {
		throw ButiEngine::ButiException(L"Failed", L"Create", L"XAudio2");
	}


	if (FAILED(hr = cmp_pXAudio2->CreateMasteringVoice(&p_masterVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL)))
	{
		throw ButiEngine::ButiException(L"Failed", L"Create", L"MasteringVoice");
	}
	m_vlp_clearTimer = ButiEngine::ObjectFactory::Create<ButiEngine::RelativeTimer>(600);
	m_vlp_clearTimer->Start();
}

void SoundManager::Update()
{
	if (m_vlp_clearTimer->Update()) {
		ClearCheck();
	}
}

void SoundManager::ClearCheck()
{

	auto itr = vec_seVoices.begin();
	while (itr != vec_seVoices.end()) {
		if ((*itr)->IsEnd()) {
			(*itr)->Destroy();
			itr = vec_seVoices.erase(itr);
		}
		else {
			itr++;
		}
	}

}

SoundObject_t SoundManager::PlaySE(Resource_Sound_t arg_sound, const float arg_volume, const bool arg_isLoop)
{
	auto waveData = arg_sound->GetData();
	waveData->m_buffer.Flags = XAUDIO2_END_OF_STREAM;
	auto sound = ButiEngine::make_value<SoundObject>(arg_sound,arg_volume,false, arg_isLoop);
	if (!sound->Initialize(cmp_pXAudio2)) {
		return nullptr;
	}
	vec_seVoices.push_back(sound);
	sound->Start();
	return sound;
}

SoundObject_t SoundManager::PlayBGM(Resource_Sound_t arg_sound, const float arg_volume)
{
	auto waveData = arg_sound->GetData();
	waveData->m_buffer.Flags = XAUDIO2_END_OF_STREAM;
	auto sound = ButiEngine::make_value<SoundObject>(arg_sound, arg_volume,  false,true);
	if (!sound->Initialize(cmp_pXAudio2)) {
		return nullptr;
	}

	currentBGM = arg_sound;
	if (m_bgm) {
		m_bgm->Stop();
		m_bgm->Destroy();
	}
	m_bgm = sound;
	m_bgm->Start();
	return m_bgm;
}

void SoundManager::SetMasterVolume(const float arg_masterVolume)
{
	m_masterVolume = arg_masterVolume;
	p_masterVoice->SetVolume(arg_masterVolume);
}

void SoundManager::Release()
{
	for (auto itr :vec_seVoices) {
		(itr)->Destroy();
	}
	DestroyBGM();
	p_masterVoice->DestroyVoice();
}

void SoundManager::StopSE()
{
	for (auto itr : vec_seVoices) {
		(itr)->Stop();
	}
}

void SoundManager::StopBGM()
{
	if (m_bgm) {
		m_bgm->Stop();
	}
}

void SoundManager::RestartSE()
{

	for (auto itr : vec_seVoices) {
		(itr)->Start();
	}
}

void SoundManager::RestartBGM()
{
	if (m_bgm) {
		m_bgm->Start();
	}
}

void SoundManager::DestroySE()
{

	for (auto itr : vec_seVoices) {
		(itr)->Stop();
		(itr)->Destroy();
	}
	vec_seVoices.clear();
}

void SoundManager::DestroyBGM()
{
	if (m_bgm) {
		m_bgm->Start();
		m_bgm->Destroy();
		m_bgm = nullptr;
		currentBGM = nullptr;
	}
}

void SoundManager::SetBGMVolume(const float volume)
{
	if (m_bgm) {
		m_bgm->SetVolume(volume * m_masterVolume);
	}
}

Resource_Sound_t SoundManager::GetCurrentPlayBGM() const
{
	return currentBGM;
}

float SoundManager::GetBGMVolume() const
{

	return !m_bgm?-1.0f: m_bgm->GetVolume();
}



}
ButiEngine::Value_ptr<ButiSound::ISoundManager> ButiSound::CreateSoundManager()
{
	auto output = ButiEngine::make_value<SoundManager>();
	output->Initialize();
	return output;
}