#include "SystemManager.h"

SystemManager::SystemManager( EntityWorld* p_world )
{
	m_world = p_world;
}

SystemManager::~SystemManager()
{
	map<SystemType::SystemTypeIdx, EntitySystem*>::iterator it;
	for( it=m_systems.begin(); it != m_systems.end(); it++ )
	{
		delete it->second;
		it->second = NULL;
	}
}

EntitySystem* SystemManager::getSystem( SystemType::SystemTypeIdx p_systemIndex )
{
	return m_systems[p_systemIndex];
}

EntitySystem* SystemManager::setSystem( SystemType p_type, EntitySystem* p_system,
									   bool p_enabled )
{
	p_system->setEnabled( p_enabled );
	m_systems[(SystemType::SystemTypeIdx)p_type.getIndex()] = p_system;
	return p_system;
}

EntitySystem* SystemManager::setSystem( SystemType::SystemTypeIdx p_typeIdx,
									   EntitySystem* p_system, bool p_enabled )
{
	//find and remove any previous versions of this system
	vector<EntitySystem*>::iterator it = m_systemList.begin();
	for( ; it!=m_systemList.end(); it++ )
	{
		if((*it)->getSystemTypeIdx() == p_typeIdx )
			it = m_systemList.erase(it);
	}
	m_systemList.push_back( p_system );
	
	p_system->setEnabled( p_enabled );
	m_systems[p_typeIdx] = p_system;

	return p_system;
}

void SystemManager::deleteSystem( SystemType p_type )
{
	deleteSystem( (SystemType::SystemTypeIdx)p_type.getIndex() );
}

void SystemManager::deleteSystem( SystemType::SystemTypeIdx p_typeIdx )
{
	//find and remove the system
	vector<EntitySystem*>::iterator it = m_systemList.begin();
	for( ; it!=m_systemList.end(); it++ )
	{
		if((*it)->getSystemTypeIdx() == p_typeIdx )
			it = m_systemList.erase(it);
	}

	delete m_systems[p_typeIdx];
	m_systems[p_typeIdx] = NULL;
}

void SystemManager::deleteSystem( EntitySystem* p_system)
{
	int idx = p_system->getSystemTypeIdx();
	//find and remove the system
	for( vector<EntitySystem*>::iterator it = m_systemList.begin();
		it!=m_systemList.end();
		it++ )
	{
		if((*it)->getSystemTypeIdx() == idx )
			m_systemList.erase(it);
	}

	//NOTE: break in for-loop 
	map<SystemType::SystemTypeIdx, EntitySystem*>::iterator it;
	for( it=m_systems.begin(); it != m_systems.end(); it++ )
	{
		if( it->second == p_system)
		{
			delete p_system;
			it->second = NULL;
			break;
		}
	}
}

void SystemManager::initializeAll()
{
	// Should be done on the list. For dependency injection.
	//map<SystemType::SystemTypeIdx, EntitySystem*>::iterator it;
	//for( it=m_systems.begin(); it != m_systems.end(); it++ )
	//	it->second->initialize();

	for( unsigned int i=0; i<m_systemList.size(); i++ )
	{
		m_systemList[i]->initialize();
	}
}

void SystemManager::updateSynchronous()
{
	//map<SystemType::SystemTypeIdx, EntitySystem*>::iterator it;
	//for( it=m_systems.begin(); it != m_systems.end(); it++ )
	//	it->second->process();

	for( unsigned int i=0; i<m_systemList.size(); i++ )
	{
		EntitySystem* system = m_systemList[i];
		if( system->getEnabled() )
			system->process();
	}
}

void SystemManager::notifySystems( IPerformer* p_performer, Entity* p_entity )
{
	for( unsigned int i = 0; i<m_systemList.size(); i++ ) 
	{
		p_performer->perform(m_systemList[i], p_entity);
	}
}
