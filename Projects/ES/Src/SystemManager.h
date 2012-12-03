#pragma once

#include "EntitySystem.h"
#include "EntityWorld.h"
#include "Manager.h"
#include "SystemType.h"

class EntitySystem;
class EntityWorld;
class Manager;
class SystemType;

class SystemManager : public Manager
{
public:
	SystemManager( EntityWorld* p_world );
	~SystemManager();

	void initialize(){ initializeAll(); };
	EntitySystem* getSystem( SystemType::SystemTypeIdx p_systemIndex );

	/**
	 * Will add a system to this manager.
	 *  
	 * @param p_type Type of system.
	 * @param p_system The system to add.
	 * @param p_enabled Wether or not this system will be processed by World.process().
	 * Defaults to true.
	 * @return The added system.
	 */
	EntitySystem* setSystem( SystemType p_type, EntitySystem* p_system,
		bool p_enabled = true );

	/**
	 * Will add a system to this manager.
	 *  
	 * @param p_typeIdx Index for the type of system.
	 * @param p_system The system to add.
	 * @param p_enabled Wether or not this system will be processed by World.process().
	 * Defaults to true.
	 * @return The added system.
	 */
	EntitySystem* setSystem( SystemType::SystemTypeIdx p_typeIdx, EntitySystem* p_system,
		bool p_enabled = true );

	/**
	 * Remove the specified system from the manager.
	 * @param p_type Type of system to be deleted from manager.
	 */
	void deleteSystem( SystemType p_type );

	/**
	 * Remove the specified system from the manager.
	 * @param p_typeIdx index of type of system to be deleted from manager.
	 */
	void deleteSystem( SystemType::SystemTypeIdx p_typeIdx );
	
	/**
	 * EXPENSIVE! Use the above methods if possible!
	 * Remove the specified system from the manager.
	 * @param p_system to be deleted from manager.
	 */
	void deleteSystem( EntitySystem* p_system);

	void initializeAll();
	void updateSynchronous();

private:
	map<SystemType::SystemTypeIdx, EntitySystem*> m_systems;
};
