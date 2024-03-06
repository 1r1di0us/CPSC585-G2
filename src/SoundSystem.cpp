#include "SoundSystem.h"

//all code stolen from https://codyclaborn.me/tutorials/making-a-basic-fmod-audio-engine-in-c/

Implementation::Implementation() {
    mpStudioSystem = NULL;
    SoundSystem::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
    SoundSystem::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

    mpSystem = NULL;
    mnNextChannelId = 0; //idk if this is right
    SoundSystem::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem)); //getLowLevelSystem does not exist so I just assume core == lowlevel
}

Implementation::~Implementation() {
    SoundSystem::ErrorCheck(mpStudioSystem->unloadAll());
    SoundSystem::ErrorCheck(mpStudioSystem->release());
}

void Implementation::Update() {
    std::vector<ChannelMap::iterator> pStoppedChannels;
    for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it)
    {
        bool bIsPlaying = false;
        it->second->isPlaying(&bIsPlaying);
        if (!bIsPlaying)
        {
            pStoppedChannels.push_back(it);
        }
    }
    for (auto& it : pStoppedChannels)
    {
        mChannels.erase(it);
    }
    SoundSystem::ErrorCheck(mpStudioSystem->update());
}

Implementation* sgpImplementation = nullptr;
SharedDataSystem* dataSys = nullptr;

void SoundSystem::Init(SharedDataSystem* sharedDataSys) {
    sgpImplementation = new Implementation;
    dataSys = sharedDataSys;
}

void SoundSystem::Update() {
    sgpImplementation->Update();
}

void SoundSystem::LoadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
{
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt != sgpImplementation->mSounds.end())
        return;

    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= b3d ? FMOD_3D : FMOD_2D;
    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    FMOD::Sound* pSound = nullptr;
    SoundSystem::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), FMOD_3D, 0, &pSound));
    if (pSound) {
        sgpImplementation->mSounds[strSoundName] = pSound;
    }

}

void SoundSystem::UnLoadSound(const std::string& strSoundName)
{
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
        return;

    SoundSystem::ErrorCheck(tFoundIt->second->release());
    sgpImplementation->mSounds.erase(tFoundIt);
}

int SoundSystem::PlaySound(const std::string& strSoundName, const FMOD_VECTOR& vPosition, float fVolumedB)
{
    int nChannelId = sgpImplementation->mnNextChannelId++;
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
    {
        LoadSound(strSoundName);
        tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        if (tFoundIt == sgpImplementation->mSounds.end())
        {
            return nChannelId;
        }
    }
    FMOD::Channel* pChannel = nullptr;
    SoundSystem::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
    if (pChannel)
    {
        FMOD_MODE currMode;
        tFoundIt->second->getMode(&currMode);
        if (currMode & FMOD_3D) {
            SoundSystem::ErrorCheck(pChannel->set3DAttributes(&vPosition, nullptr));
        }
        SoundSystem::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
        SoundSystem::ErrorCheck(pChannel->setPaused(false));
        sgpImplementation->mChannels[nChannelId] = pChannel;
    }
    return nChannelId;
}

void SoundSystem::SetChannel3dPosition(int nChannelId, const FMOD_VECTOR& vPosition)
{
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
        return;

    SoundSystem::ErrorCheck(tFoundIt->second->set3DAttributes(&vPosition, NULL));
}

void SoundSystem::SetChannelVolume(int nChannelId, float fVolumedB)
{
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
        return;

    SoundSystem::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
}

//event stuff we probably wont use

void SoundSystem::LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
    auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
    if (tFoundIt != sgpImplementation->mBanks.end())
        return;
    FMOD::Studio::Bank* pBank;
    SoundSystem::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
    if (pBank) {
        sgpImplementation->mBanks[strBankName] = pBank;
    }
}

void SoundSystem::LoadEvent(const std::string& strEventName) {
    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
    if (tFoundit != sgpImplementation->mEvents.end())
        return;
    FMOD::Studio::EventDescription* pEventDescription = NULL;
    SoundSystem::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
    if (pEventDescription) {
        FMOD::Studio::EventInstance* pEventInstance = NULL;
        SoundSystem::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
        if (pEventInstance) {
            sgpImplementation->mEvents[strEventName] = pEventInstance;
        }
    }
}

void SoundSystem::PlayEvent(const std::string& strEventName) {
    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
    if (tFoundit == sgpImplementation->mEvents.end()) {
        LoadEvent(strEventName);
        tFoundit = sgpImplementation->mEvents.find(strEventName);
        if (tFoundit == sgpImplementation->mEvents.end())
            return;
    }
    tFoundit->second->start();
}

void SoundSystem::StopEvent(const std::string& strEventName, bool bImmediate) {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;

    FMOD_STUDIO_STOP_MODE eMode;
    eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    SoundSystem::ErrorCheck(tFoundIt->second->stop(eMode));
}

bool SoundSystem::IsEventPlaying(const std::string& strEventName) const {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return false;

    FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
    if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
        return true;
    }
    return false;
}

//certain functions in here no longer exist.
//void SoundSystem::GetEventParameter(const std::string& strEventName, const std::string& strEventParameter, float* parameter) {
    //auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    //if (tFoundIt == sgpImplementation->mEvents.end())
    //    return;

    //FMOD::Studio::EventInstance* pParameter = NULL;
    //SoundSystem::ErrorCheck(tFoundIt->second->getParameter(strEventParameter.c_str(), &pParameter));
    //SoundSystem::ErrorCheck(pParameter->getValue(parameter));
//}

//void SoundSystem::SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue) {
//    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
//    if (tFoundIt == sgpImplementation->mEvents.end())
//        return;

    //FMOD::Studio::ParameterInstance* pParameter = NULL;
    //SoundSystem::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
    //SoundSystem::ErrorCheck(pParameter->setValue(fValue));
//}

//FMOD_VECTOR SoundSystem::VectorToFmod(const Vector3& vPosition) {
//    FMOD_VECTOR fVec;
//    fVec.x = vPosition.x;
//    fVec.y = vPosition.y;
//    fVec.z = vPosition.z;
//    return fVec;
//}

float  SoundSystem::dbToVolume(float dB)
{
    return powf(10.0f, 0.05f * dB);
}

float  SoundSystem::VolumeTodb(float volume)
{
    return 20.0f * log10f(volume);
}

int SoundSystem::ErrorCheck(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        std::cout << "FMOD ERROR " << result << std::endl;
        return 1;
    }
    // cout << "FMOD all good" << endl;
    return 0;
}

void SoundSystem::Shutdown() {
    delete sgpImplementation;
}

void SoundSystem::AddToSoundDict(std::string name, std::string location) {
    SoundDict.push_back( std::pair < std::string, std::string> ( name, location ) );
}

void SoundSystem::PlayAllSounds() {
    for (std::pair <std::string, PxVec3> soundPair : dataSys->SoundsToPlay) {
        for (std::pair <std::string, std::string> dictPair : SoundDict) {
            if (soundPair.first == dictPair.first) {
                FMOD_VECTOR location = FMOD_VECTOR{ soundPair.second.x/15, soundPair.second.y/15, soundPair.second.z/15 };
                PlaySound(dictPair.second, location, SfxVolume);
            }
        }
    }
    dataSys->SoundsToPlay.clear(); //remove all sounds since we played them
}