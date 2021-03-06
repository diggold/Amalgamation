#pragma once
#include <TcpServer.h>
#include <Entity.h>
#include "Transform.h"
#include "ModuleEvent.h"
#include "SpawnSoundEffectPacket.h"
#include "RemoveSoundEffectPacket.h"
// =======================================================================================
// SpeedBoostModuleActivation
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief Brief...
///        
/// # SpeedBoostModuleActivation
/// Detailed description...
/// Created on: 4-2-2013 
///---------------------------------------------------------------------------------------
class SpeedBoostModuleActivation: public ModuleEvent
{
public:
	SpeedBoostModuleActivation(Entity* p_shipEntity, TcpServer* p_server)
	{
		m_shipEntity = p_shipEntity;
		m_server = p_server;
	}
	
	void activate()
	{
		Transform* transform = static_cast<Transform*>(m_shipEntity->getComponent(
			ComponentType::Transform));
		if(transform)
		{
			SpawnSoundEffectPacket data;
			data.attachedToNetsyncEntity = m_shipEntity->getIndex();
			data.positional = true;
			data.position = transform->getTranslation();
			data.soundIdentifier = SpawnSoundEffectPacket::SpeedBoostActive;
			m_server->broadcastPacket(data.pack());
		}
	}

	void deactivate()
	{
		Transform* transform = static_cast<Transform*>(m_shipEntity->getComponent(
			ComponentType::Transform));
		if(transform)
		{
			RemoveSoundEffectPacket data;
			data.attachedNetsyncIdentity = m_shipEntity->getIndex();
			m_server->broadcastPacket(data.pack());
		}
	}

private:
	Entity* m_shipEntity;
	TcpServer* m_server;
};