#include "PhysicsBody.h"

ComponentRegister<PhysicsBody> PhysicsBody::s_reg("PhysicsBody");

PhysicsBody::PhysicsBody()
{
	m_type = ComponentType::ComponentTypeIdx::PhysicsBody;

	m_id = -1;
	m_parentId = -1;
	m_parentChanged = false;
}

void PhysicsBody::init( vector<ComponentData> p_initData )
{
	// Nothing needed here right now
}

void PhysicsBody::setParentId( int p_parentId )
{
	// if parent is changed after system processing, we have to store the
	// old parent for the next process run.
	// All coming calls to this function before the next run should not update
	// the old id. Only the original old id is the one which will have been
	// attached before the next processing and which should be tested for detaching.
	// All intermediate attachments will just never occur! :)
	if (!m_parentChanged)
	{
		m_oldParentId = m_parentId;
		m_parentChanged = true;
	}

	m_parentId=p_parentId;
}

void PhysicsBody::unspecifyParent()
{
	setParentId(-1);
}
