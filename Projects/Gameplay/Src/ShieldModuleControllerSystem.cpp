#include "ShieldModuleControllerSystem.h"
#include "Transform.h"
#include <AglMatrix.h>
#include <AglQuaternion.h>
#include <Control.h>
#include "PhysicsBody.h"
#include "BodyInitData.h"
#include "PhysicsSystem.h"
#include "ShipModule.h"
#include "EntityCreationPacket.h"
#include "NetworkSynced.h"
#include "ShipModulesTrackerSystem.h"
#include <Globals.h>
#include "SpawnSoundEffectPacket.h"
#include "ShieldModuleActivation.h"
#include "AnimationUpdatePacket.h"

ShieldModuleControllerSystem::ShieldModuleControllerSystem(TcpServer* p_server)
	: EntitySystem(SystemType::ShieldModuleControllerSystem, 2,
	ComponentType::ShieldModule, ComponentType::ShipModule, ComponentType::Transform)
{
	m_server = p_server;
}


ShieldModuleControllerSystem::~ShieldModuleControllerSystem()
{
}

void ShieldModuleControllerSystem::initialize()
{
}

void ShieldModuleControllerSystem::processEntities(const vector<Entity*>& p_entities)
{
	float dt = m_world->getDelta();

	for (unsigned int i = 0; i < p_entities.size(); i++)
	{
		ShipModule* module = static_cast<ShipModule*>(
			m_world->getComponentManager()->getComponent(p_entities[i],
			ComponentType::getTypeFor(ComponentType::ShipModule)));
		if (module->m_parentEntity >= 0)
		{
			ShieldModule* shieldModule = static_cast<ShieldModule*>(
				m_world->getComponentManager()->getComponent(p_entities[i],
				ComponentType::getTypeFor(ComponentType::ShieldModule)));
			
			handleShieldEntity(shieldModule, m_world->getEntity(module->m_parentEntity), module->getActive(), p_entities[i]);
		}
	}
}
void ShieldModuleControllerSystem::handleShieldEntity(ShieldModule* p_module, Entity* p_parentEntity, bool p_active, Entity* p_e)
{
	Transform* parentTransform = static_cast<Transform*>(
		m_world->getComponentManager()->getComponent(p_parentEntity,
		ComponentType::getTypeFor(ComponentType::Transform))); 

	if (p_module->m_shieldEntity >= 0)
	{
		//Update shield
		Entity* shield = m_world->getEntity(p_module->m_shieldEntity);
		
		Transform* transform = static_cast<Transform*>(
			m_world->getComponentManager()->getComponent(shield,
			ComponentType::getTypeFor(ComponentType::Transform))); 
		
		if (p_active)
		{
			//Handle animation
			if (p_module->m_shieldAge == 0)
				activateShield(p_e);

			p_module->m_shieldAge++;


			transform->setTranslation(parentTransform->getTranslation());
			transform->setRotation(parentTransform->getRotation());
			transform->setScale(AglVector3(6, 6, 6));
			
			// Get all "child" modules
			vector<ShipModule*> childModules = static_cast<ShipModulesTrackerSystem*>(
				m_world->getSystem(SystemType::ShipModulesTrackerSystem))->
				getModulesFromParent(p_parentEntity->getIndex());
			bool damageTaken = false;
			for(unsigned int moduleIndex=0; moduleIndex<childModules.size(); moduleIndex++)
			{
				if(childModules[moduleIndex])
				{
					if(childModules[moduleIndex]->damageTaken())
					{
						damageTaken = true;
					}
					childModules[moduleIndex]->resetDamage();
				}
			}
			if(damageTaken)
			{
				SpawnSoundEffectPacket soundEffectPacket;
				soundEffectPacket.soundIdentifier = (int)SpawnSoundEffectPacket::ShieldDeflect;
				soundEffectPacket.positional = false;
				soundEffectPacket.attachedToNetsyncEntity = -1;
				m_server->broadcastPacket(soundEffectPacket.pack());
			}
		}
		else
		{
			if (p_module->m_shieldAge != 0)
				deactivateShield(p_e);
			p_module->m_shieldAge = 0;
			transform->setScale(AglVector3(0, 0, 0));
		}
	}
	else
	{
		// HACKHACKHACK: (Just to search for l8ter).
		//Create Shield
		Entity* entity = m_world->createEntity();

		Transform* t = new Transform(parentTransform->getTranslation(), AglQuaternion::identity(), AglVector3(2, 2, 2));
		entity->addComponent( ComponentType::Transform, t);
		// NOTE: (Johan) Read the note a few rows below.
//		entity->addComponent(ComponentType::NetworkSynced, 
//			new NetworkSynced( entity->getIndex(), -1, EntityType::Shield));
		m_world->addEntity(entity);
		p_module->m_shieldEntity = entity->getIndex();
		
		// NOTE: (Johan) This was unhandled by the client side --resulting in the world
		// kind of disappearing.
//		EntityCreationPacket data;
//		data.entityType		= static_cast<char>(EntityType::Shield);
//		data.meshInfo		= 1; //Sphere
//		data.owner			= -1;
//		data.networkIdentity = entity->getIndex();
//		data.translation	= t->getTranslation();
//		data.rotation		= t->getRotation();
//		data.scale			= t->getScale();
//
//		m_server->broadcastPacket(data.pack());
	}
}

void ShieldModuleControllerSystem::inserted( Entity* p_entity )
{
	ShipModule* module = static_cast<ShipModule*>(p_entity->getComponent(
		ComponentType::ShipModule));
	module->addActivationEvent(new ShieldModuleActivation(p_entity, m_server));
}

void ShieldModuleControllerSystem::removed( Entity* p_entity )
{
	// HACK: NOTE: HACK: NOTE: REMOVE THE EVENTS!
	// NOTE: Or does the module destroy its own events? Well, maybe it does!
	// NOTE: Yeah, they do. Don't bother the guy writing the first line (A).
}

void ShieldModuleControllerSystem::activateShield(Entity* p_shield)
{
	AnimationUpdatePacket packet;
	packet.networkIdentity = p_shield->getIndex();
	packet.shouldPlay = true;
	packet.playSpeed = 15.0f;
	packet.take = "Start";
	m_server->broadcastPacket( packet.pack() );

	AnimationUpdatePacket packetIdle;
	packetIdle.networkIdentity = p_shield->getIndex();
	packetIdle.shouldPlay = true;
	packetIdle.playSpeed = 15.0f;
	packetIdle.take = "Active";
	packetIdle.shouldQueue = true;
	m_server->broadcastPacket( packetIdle.pack() );
}
void ShieldModuleControllerSystem::deactivateShield(Entity* p_shield)
{
	AnimationUpdatePacket packet;
	packet.networkIdentity = p_shield->getIndex();
	packet.shouldPlay = true;
	packet.playSpeed = 15.0f;
	packet.take = "Stop";
	m_server->broadcastPacket( packet.pack() );

	AnimationUpdatePacket idlepacket;
	idlepacket.networkIdentity = p_shield->getIndex();
	idlepacket.shouldPlay = true;
	idlepacket.playSpeed = 15.0f;
	idlepacket.take = "Default";
	idlepacket.shouldQueue = true;
	m_server->broadcastPacket( idlepacket.pack() );
}