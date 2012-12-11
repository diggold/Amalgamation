#include "SoundWrapper.h"


SoundWrapper::SoundWrapper()
{
	m_soundDevice	= NULL;
	m_masterVoice	= NULL;
	m_masterVolume	= 0;

	CoInitializeEx( NULL, COINIT_MULTITHREADED );

	initSoundEngine();

	m_soundFactory = new SoundFactory(m_soundDevice);
}

SoundWrapper::~SoundWrapper()
{
	m_masterVoice->DestroyVoice();
	m_soundDevice->StopEngine();
	m_soundDevice->Release();
	
	delete m_soundFactory;
}

void SoundWrapper::initSoundEngine()
{
	
	m_soundDevice = AudioEngineCreator::createAudioEngine();
	m_masterVoice = AudioEngineCreator::createMasterVoice(m_soundDevice);
	AudioEngineCreator::createXAudio3(m_soundDevice, m_x3DAudioInstance);
}

void SoundWrapper::updateListener(const SoundSceneInfo& p_sceneInfo)
{
	X3DAUDIO_VECTOR front = {
		p_sceneInfo.listenerOrientFront[0],
		p_sceneInfo.listenerOrientFront[1],
		p_sceneInfo.listenerOrientFront[2],
	};

	X3DAUDIO_VECTOR top = {
		p_sceneInfo.listenerOrientFront[0],
		p_sceneInfo.listenerOrientFront[1],
		p_sceneInfo.listenerOrientFront[2],
	};
	
	X3DAUDIO_VECTOR velocity =  {
		p_sceneInfo.listenerVelocity[0],
		p_sceneInfo.listenerVelocity[1],
		p_sceneInfo.listenerVelocity[2],
	};

	X3DAUDIO_VECTOR pos = {
		p_sceneInfo.listenerPos[0],
		p_sceneInfo.listenerPos[1],
		p_sceneInfo.listenerPos[2],
	};

	m_listener.OrientFront	= front;
	m_listener.OrientTop	= top;
	m_listener.Position		= pos;
	m_listener.Velocity		= velocity;
	m_listener.pCone		= NULL;
}

Sound* SoundWrapper::createNewNonPositionalSound( const char* p_filePath )
{
	return m_soundFactory->createNonPositionalSound(p_filePath);
}

PositionalSound* SoundWrapper::createNewPositionalSound( const char* p_filePath )
{
	return m_soundFactory->createPositionalSound(p_filePath);
}