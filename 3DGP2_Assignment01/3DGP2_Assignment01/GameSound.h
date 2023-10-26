#pragma once
#include "inc/inc/fmod.hpp"
#include "inc/inc/fmod_errors.h"
// fmod dll
#pragma comment(lib, "lib/lib/64/fmod_vc.lib")
#pragma comment(lib, "lib/lib/64/fmodL_vc.lib")


class GameSound
{
public:
	FMOD::System* soundSystem; 

	FMOD::Sound*  BackGroundSound;
	FMOD::Channel* BackGroundChannel; 

	FMOD_RESULT  result;
	void* extradriverdata = 0;

public:
	GameSound();
	~GameSound();
public:
	float SoundElapsedTime = 0.f;
	bool Sound_ON = false;
	void PauseOpeningSound();
	void PlayOpeningSound();
};