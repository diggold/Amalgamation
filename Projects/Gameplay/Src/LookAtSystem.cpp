#include "LookAtSystem.h"

#include "CameraSystem.h"
#include "CameraInfo.h"
#include "GraphicsBackendSystem.h"
#include "InputBackendSystem.h"
#include "LookAtEntity.h"
#include "Transform.h"
#include "GameplayTags.h"

LookAtSystem::LookAtSystem() : 
EntitySystem( SystemType::LookAtSystem, 1,
			 ComponentType::ComponentTypeIdx::LookAtEntity,
			 ComponentType::ComponentTypeIdx::Transform)
{

}


LookAtSystem::~LookAtSystem()
{
}

void LookAtSystem::initialize()
{

}

void LookAtSystem::processEntities( const vector<Entity*>& p_entities )
{
	float dt = m_world->getDelta();
	for(unsigned int i=0; i<p_entities.size(); i++ )
	{

		Transform* transform = static_cast<Transform*>(
			p_entities[i]->getComponent( ComponentType::ComponentTypeIdx::Transform ) );

		// optional component for lookat
		LookAtEntity* lookAt= static_cast<LookAtEntity*>(
			p_entities[i]->getComponent( ComponentType::ComponentTypeIdx::LookAtEntity) );

		// optional lookAt tags
		LookAtFollowMode_TAG* lookAtFollow=NULL;
		LookAtOrbitMode_TAG* lookAtOrbit=NULL;

		Component* t = p_entities[i]->getComponent( ComponentType::ComponentTypeIdx::TAG_LookAtFollowMode );
		if (t!=NULL)
			lookAtFollow = static_cast<LookAtFollowMode_TAG*>(t);


		t = p_entities[i]->getComponent( ComponentType::ComponentTypeIdx::TAG_LookAtOrbitMode );
		if (t!=NULL)
			lookAtOrbit = static_cast<LookAtOrbitMode_TAG*>(t);


		// Retrieve initial info
		AglVector3 position = transform->getTranslation();
		AglQuaternion rotation = transform->getRotation();
		
		// Extract look-at entity and its transform
		Entity* targetEntity = m_world->getEntity(lookAt->getEntityId());
		Transform* targetTransform = static_cast<Transform*>(
			targetEntity->getComponent(ComponentType::ComponentTypeIdx::Transform));

		AglVector3 lookTarget = targetTransform->getTranslation();


		// Follow behaviour
		if (lookAtFollow)
		{		
			// Set up look-at vars for the view matrix
			// Create offset vector from look-at component in the space of the target
			AglVector3 offset = lookAt->getOffset();
			offset.transformNormal(targetTransform->getMatrix());
			// update transform

			// position = AglVector3::lerp(position,lookTarget+offset,
			//							abs(lookAt->getMoveSpd())*dt);
			// rotation = AglQuaternion::slerp(rotation,targetTransform->getRotation(),
			//							abs(lookAt->getRotationSpeed())*dt);
			position = lookTarget+offset;
			rotation = targetTransform->getRotation();
			rotation.normalize();

			// update
			transform->setTranslation( position );
			transform->setRotation( rotation );
		}

		// orbit behaviour
		if (lookAtOrbit)
		{
			
		}

		// just lookat behaviour
		if (!lookAtOrbit && !lookAtFollow)
		{
			AglVector3 dir = lookTarget-position;
			AglVector3::normalize(dir);
			transform->setForward(dir);
		}
		
	}
}
