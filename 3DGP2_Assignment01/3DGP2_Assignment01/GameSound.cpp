#include "stdafx.h"
#include "GameSound.h"
GameSound::GameSound()
{
	result = FMOD::System_Create(&soundSystem);


	result = soundSystem->init(32, FMOD_INIT_NORMAL, extradriverdata); // 
	result = soundSystem->createSound("Sound/TankExplosion.wav", FMOD_DEFAULT, 0, &BackGroundSound);
	result = BackGroundSound->setMode(FMOD_LOOP_NORMAL); //LOOP_NORMAL ->���� ���� FMOD_LOOP_OFF->�ѹ�?

	result = soundSystem->playSound(BackGroundSound, 0, true, &BackGroundChannel); //true�� ����.
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