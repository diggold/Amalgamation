#include "EntityManager.h"

EntityManager::EntityManager()
{
	m_nextAvailableId = 0;
	m_active = 0;
	m_added = 0;
	m_created = 0;
	m_deleted = 0;
}

EntityManager::~EntityManager()
{
	for( unsigned int i=0; i<m_entities.size(); i++ )
	{
		delete m_entities[i];
	}
}

Entity* EntityManager::createEntityInstance()
{
	int id;
	if( m_availableIds.empty() ) 
	{
		id = m_nextAvailableId++;
	}
	else
	{
		id = m_availableIds.top();
		m_availableIds.pop();
	}

	Entity* entity= new Entity(m_world, id);
	m_created++;
	return entity;
}

void EntityManager::added( Entity* p_entity )
{
		m_active++;
		m_added++;

		int idx = p_entity->getIndex();

		if( (int)m_entities.size() <= idx )
		{
			m_entities.resize( idx+1 );
			m_disabled.resize( idx+1);
		}
		m_entities[idx] =  p_entity;
		m_disabled[idx] = false;
}

void EntityManager::enabled( Entity* p_entity )
{
	m_disabled[p_entity->getIndex()] = false;
}

void EntityManager::disabled( Entity* p_entity )
{
	m_disabled[p_entity->getIndex()] = true;
}

void EntityManager::deleted( Entity* p_entity )
{
	m_disabled[p_entity->getIndex()] = false;
	//m_availableIds.push(p_entity->getIndex()); Done in postPerform to ensure everything
	//is recycled without errors.
	
	// can we delete here? Alex says: NO!!!! are there still references? 
	//delete m_entities[p_entity->getIndex()];
	// Deletion needs to be done in postPerform! Add the entity to that list instead.
	// Setting the entity in the list to null also needs to be done in postPerform
	//m_entities[p_entity->getIndex()] = NULL;
	m_deleteOnPost.push_back(p_entity);

	// NO!!!
	//delete p_entity;

	m_active--;
	m_deleted++;
}

void EntityManager::postPerform()
{
	// Delete entities here!
	for (unsigned int i = 0; i < m_deleteOnPost.size(); i++)
	{
		m_availableIds.push(m_deleteOnPost[i]->getIndex());
		m_entities[m_deleteOnPost[i]->getIndex()] = NULL;
		delete m_deleteOnPost[i];
	}
	m_deleteOnPost.clear();
}


bool EntityManager::isActive( int p_entityId ) const
{
	return m_entities[p_entityId] != NULL;
}

bool EntityManager::isEnabled( int p_entityId ) const
{
	return !m_disabled[p_entityId];
}

Entity* EntityManager::getEntity( int p_entityId ) const
{
	if(p_entityId < (int)m_entities.size())
		return m_entities[p_entityId];
	return NULL;
}

int EntityManager::getActiveEntityCount() const
{
	return m_active;
}

int EntityManager::getTotalCreated() const
{
	return m_created;
}

int EntityManager::getTotalAdded() const
{
	return m_added; 
}

int EntityManager::getTotalDeleted() const
{
	return m_deleted;
}

Entity* EntityManager::getFirstEntityByComponentType( ComponentType::ComponentTypeIdx p_componentType )
{
	for(unsigned int i=0; i<m_entities.size(); i++)
	{
		if(m_entities[i] && m_entities[i]->getComponent( p_componentType ) != NULL)
		{
			return m_entities[i];
		}
	}

	return NULL;
}

