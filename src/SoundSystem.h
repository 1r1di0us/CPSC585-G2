#pragma once
#ifndef _SOUND_SYSTEM_H_ //if sound system has not been defined before, do all the includes
#define _SOUND_SYSTEM_H_

#include "fmod_studio.hpp"
#include "fmod.hpp"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include "SharedDataSystem.h"

#endif

struct Implementation {
    Implementation();
    ~Implementation();

    void Update();

    FMOD::Studio::System* mpStudioSystem;
    FMOD::System* mpSystem;

    int mnNextChannelId;

    typedef std::map<std::string, FMOD::Sound*> SoundMap;
    typedef std::map<int, FMOD::Channel*> ChannelMap;
    typedef std::map<std::string, FMOD::Studio::EventInstance*> EventMap;
    typedef std::map<std::string, FMOD::Studio::Bank*> BankMap;

    BankMap mBanks;
    EventMap mEvents;
    SoundMap mSounds;
    ChannelMap mChannels;
};

class SoundSystem {
public:

    std::vector <std::pair <std::string, std::string> > SoundDict;
    float GameVolume = -25.0;

    static void Init(SharedDataSystem* sharedDataSys);
    static void Update();
    static void Shutdown();
    static int ErrorCheck(FMOD_RESULT result);

    void LoadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
    void LoadEvent(const std::string& strEventName);
    void LoadSound(const std::string& strSoundName, bool b3d = true, bool bLooping = false, bool bStream = false);
    void UnLoadSound(const std::string& strSoundName);
    //void Set3dListenerAndOrientation(const Vector3& vPos = Vector3{ 0, 0, 0 }, float fVolumedB = 0.0f); not implemented
    int PlaySound(const std::string& strSoundName, const FMOD_VECTOR& vPos = FMOD_VECTOR{ 0, 0, 0 }, float fVolumedB = 0.0f);
    void PlayEvent(const std::string& strEventName);
    //void StopChannel(int nChannelId); not implemented
    void StopEvent(const std::string& strEventName, bool bImmediate = false);
    //void GetEventParameter(const std::string& strEventName, const std::string& strEventParameter, float* parameter); doesn't work
    //void SetEventParameter(const std::string& strEventName, const std::string& strParameterName, float fValue); doesn't work
    //void StopAllChannels(); not implemented
    void SetChannel3dPosition(int nChannelId, const FMOD_VECTOR& vPosition);
    void SetChannelVolume(int nChannelId, float fVolumedB);
    //bool IsPlaying(int nChannelId) const; not implemented
    bool IsEventPlaying(const std::string& strEventName) const;
    float dbToVolume(float db);
    float VolumeTodb(float volume);
    //FMOD_VECTOR VectorToFmod(const FMOD_VECTOR& vPosition);

    void AddToSoundDict(std::string name, std::string location);
    void PlayAllSounds();
};