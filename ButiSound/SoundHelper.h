#ifndef SOUNDHELPER_H
#define SOUNDHELPER_H
#pragma once
#include"ButiUtil/ButiUtil/Util.h"
#include"ButiUtil/ButiUtil/BinaryObject.h"
#include"ButiMemorySystem/ButiMemorySystem/ButiPtr.h"
namespace ButiSound {
class WaveDatas;
namespace SoundHelper
{
ButiEngine::Value_ptr<WaveDatas> CreateFromBinary(ButiEngine::Value_ptr<ButiEngine::IBinaryReader> arg_reader);
}

}

#endif // !SOUNDHELPER_H


