#include "SpeedBoostModuleControllerSystem.h"
#include "ShipModule.h"
#include <TcpServer.h>
#include "OnActivateSpeedBoostModule.h"
#include "OnDeActivateSpeedBoostModule.h"

SpeedBoostModuleControllerSystem::SpeedBoostModuleControllerSystem(TcpServer* p_server)
	: EntitySystem( SystemType::SpeedBoostModuleControllerSystem, 3,
	ComponentType::ShipModule, ComponentType::SpeedBoosterModule,
	ComponentType::Transform )
{
	m_server = p_server;
}

SpeedBoostModuleControllerSystem::~SpeedBoostModuleControllerSystem()
{
}

void SpeedBoostModuleControllerSystem::inserted( Entity* p_entity )
{
	ShipModule* module = static_cast<ShipModule*>(p_entity->getComponent(
		ComponentType::ShipModule));
	module->addOnActivate(new OnActivateSpeedBoostModule(p_entity, m_server));
	module->addOnDeActivate(new OnDeActivateSpeedBoostModule(p_entity, m_server));
}
