#pragma once

#include "OnHitScoreEffectPacket.h"
#include <queue>
#include <Entity.h>
#include <EntitySystem.h>
#include "ModuleStatusEffectPacket.h"
#include "ServerStateSystem.h"

using namespace std;

class TcpServer;

// =======================================================================================
//                             ModuleVisualEffectBufferSystem
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	System that buffers effect packets for sendout and visualization to clients 
///        
/// # ModuleVisualEffectBufferSystem
/// Detailed description.....
/// Created on: 13-2-2013 
///---------------------------------------------------------------------------------------

class ModuleVisualEffectServerBufferSystem : public EntitySystem
{
public:
	ModuleVisualEffectServerBufferSystem(TcpServer* p_server, ServerStateSystem* p_states);
	virtual ~ModuleVisualEffectServerBufferSystem();

	virtual void initialize();
	virtual void process();

	void enqueueEffect(Entity* p_entity, OnHitScoreEffectPacket& p_packet);
	void enqueueEffect(int p_networkOwnerId, OnHitScoreEffectPacket& p_packet);
	void enqueueEffect(ModuleStatusEffectPacket& p_packet);
protected:
private:
	queue<pair<Entity*,OnHitScoreEffectPacket>> m_scoreFXqueue_entity;
	queue<pair<int,OnHitScoreEffectPacket>> m_scoreFXqueue_netowner;
	queue<ModuleStatusEffectPacket> m_statusFXqueue_netowner;
	TcpServer* m_server;
	ServerStateSystem* m_serverStates;
};