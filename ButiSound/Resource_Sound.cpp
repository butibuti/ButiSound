#include"stdafx.h"
#include"SoundHelper.h"
namespace ButiSound{
class Resource_Sound:public IResource_Sound {
public:
	Resource_Sound(ButiEngine::Value_ptr<WaveDatas> arg_data):m_vlp_waveData(arg_data){}
	ButiEngine::Value_ptr<WaveDatas> GetData() { return m_vlp_waveData; }
private:
	ButiEngine::Value_ptr<WaveDatas> m_vlp_waveData;
};
}
ButiEngine::Value_ptr<ButiSound::IResource_Sound> ButiSound::CreateSound(ButiEngine::Value_ptr<ButiEngine::IBinaryReader> arg_reader)
{
	auto output=ButiEngine::make_value<Resource_Sound>( SoundHelper::CreateFromBinary(arg_reader));
	return output;
}