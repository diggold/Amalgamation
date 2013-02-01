#include "ShipModule.h"

ComponentRegister<ShipModule> ShipModule::s_reg("ShipModule");

ShipModule::ShipModule()
{
	m_parentEntity = -1;
	m_active = false;
	m_value = 100;
	m_health = 100.0f;
	m_addedDamage = 0;
	m_componentType = ComponentType::ShipModule;
}

ShipModule::~ShipModule()
{
	for(unsigned int i=0; i<m_activateEvents.size(); i++)
	{
		delete m_activateEvents[i];
	}
	for(unsigned int i=0; i<m_deActivateEvents.size(); i++)
	{
		delete m_deActivateEvents[i];
	}
}

void ShipModule::init( vector<ComponentData> p_initData )
{
	for( unsigned int i=0; i<p_initData.size(); i++ )
	{
		if( p_initData[i].dataName == "value" )
			p_initData[i].getData<float>(&m_value);
		else if( p_initData[i].dataName == "health" )
			p_initData[i].getData<float>(&m_health);
	}
}

void ShipModule::addDamageThisTick( float p_amount )
{
	m_addedDamage += p_amount;
}

void ShipModule::applyDamage()
{
	m_health -= m_addedDamage;
	m_addedDamage = 0;
}

void ShipModule::resetDamage()
{
	m_addedDamage = 0;
}

bool ShipModule::damageTaken() const
{
	return m_addedDamage > 0;
}

const bool& ShipModule::getActive() const
{
	return m_active;
}

void ShipModule::activate()
{
	m_active = true;
	for(unsigned int i=0; i<m_activateEvents.size(); i++)
	{
		m_activateEvents[i]->happen();
	}
}

void ShipModule::deActivate()
{
	m_active = false;
	for(unsigned int i=0; i<m_deActivateEvents.size(); i++)
	{
		m_deActivateEvents[i]->happen();
	}
}

void ShipModule::addOnActivate( ModuleEvent* p_event )
{
	m_activateEvents.push_back( p_event );
}

void ShipModule::addOnDeActivate( ModuleEvent* p_event )
{
	m_deActivateEvents.push_back( p_event );
}
