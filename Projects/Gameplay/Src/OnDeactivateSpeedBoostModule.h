#pragma once
#include "ModuleEvent.h"
#include <TcpServer.h>
#include "Transform.h"
#include <Entity.h>
#include "RemoveSoundEffectPacket.h"
// =======================================================================================
// OnDeActivateSpeedBoostModule
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief Brief...
///        
/// # OnDeActivateSpeedBoostModule
/// Detailed description...
/// Created on: 4-2-2013 
///---------------------------------------------------------------------------------------
class OnDeActivateSpeedBoostModule: public ModuleEvent
{
public:
	OnDeActivateSpeedBoostModule(Entity* p_shipEntity, TcpServer* p_server)
	{
		m_shipEntity = p_shipEntity;
		m_server = p_server;
	}
	
	void happen() // NOTE: (Johan) try "virtual void happen() final" later. :)
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