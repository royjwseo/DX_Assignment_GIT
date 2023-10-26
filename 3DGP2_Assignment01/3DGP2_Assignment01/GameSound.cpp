#include "stdafx.h"
#include "GameSound.h"
GameSound::GameSound()
{
	result = FMOD::System_Create(&soundSystem);


	result = soundSystem->init(32, FMOD_INIT_NORMAL, extradriverdata); // 
	result = soundSystem->createSound("Sound/TankExplosion.wav", FMOD_DEFAULT, 0, &BackGroundSound);
	result = BackGroundSound->setMode(FMOD_LOOP_NORMAL); //LOOP_NORMAL ->무한 루프 FMOD_LOOP_OFF->한번?

	result = soundSystem->playSound(BackGroundSound, 0, true, &BackGroundChannel); //true가 정지.
	BackGroundChannel->setVolume(0.5f);

}

GameSound::~GameSound()
{

	result = BackGroundSound->release();
	//Common_Close();
}

void GameSound::PauseOpeningSound()
{
	BackGroundChannel->setPaused(true);
}

void GameSound::PlayOpeningSound()
{
	BackGroundChannel->setPaused(false);

}