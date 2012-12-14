#include "CameraSystem.h"

CameraSystem::CameraSystem( GraphicsBackendSystem* p_gfxBackend, 
						    InputBackendSystem* p_inputBackend ) : 
		      EntitySystem( SystemType::CameraSystem, 1,
							ComponentType::ComponentTypeIdx::CameraInfo )
{
	m_gfxBackend = p_gfxBackend;
	m_inputBackend = p_inputBackend;
}


CameraSystem::~CameraSystem()
{
}

void CameraSystem::initialize()
{
	m_ticker = 0.0f;
	m_mouseXPositive = m_inputBackend->getInputControl("Mouse X positive");
	m_mouseXNegative = m_inputBackend->getInputControl("Mouse X negative");
	m_mouseYPositive = m_inputBackend->getInputControl("Mouse Y positive");
	m_mouseYNegative = m_inputBackend->getInputControl("Mouse Y negative");
}

void CameraSystem::processEntities( const vector<Entity*>& p_entities )
{
	// Input controls
	double mouseX = m_mouseXPositive->getStatus() - m_mouseXNegative->getStatus();
	double mouseY = m_mouseYPositive->getStatus() - m_mouseYNegative->getStatus();


	for(unsigned int i=0; i<p_entities.size(); i++ )
	{
		m_ticker += m_world->getDelta();

		CameraInfo* camInfo = static_cast<CameraInfo*>(
			p_entities[i]->getComponent( ComponentType::ComponentTypeIdx::CameraInfo ) );

		Transform* transform = static_cast<Transform*>(
			p_entities[i]->getComponent( ComponentType::ComponentTypeIdx::Transform ) );

		// optional component for lookat
		LookAtEntity* lookAt = static_cast<LookAtEntity*>(
			p_entities[i]->getComponent( ComponentType::ComponentTypeIdx::LookAtEntity ) );

		// Handle Input for camera
		AglVector3 position = transform->getTranslation();
		// AglQuaternion rotation = transform->getRotation();
		AglVector3 lookTarget = position+transform->getMatrix().GetForward();
		AglVector3 up = transform->getMatrix().GetUp();
		double sensitivityMult = 1000.0;
		// position.x -= static_cast<float>(mouseX*sensitivityMult);
		// position.y -= static_cast<float>(mouseY*sensitivityMult);
		// transform->setTranslation( position );

		if (lookAt)
		{
			// Extract look-at entity and its transform
			Entity* targetEntity = m_world->getEntity(lookAt->getEntityId());
			Transform* targetTransform = static_cast<Transform*>(
				targetEntity->getComponent(ComponentType::ComponentTypeIdx::Transform));
			lookTarget = targetTransform->getTranslation();
			// Set up look-at vars for the view matrix
			// Create offset vector from look-at component in the space of the target
			AglVector3 offset = lookAt->getOffset();
			offset.transformNormal(targetTransform->getMatrix());
			// Transform camera up
			up.transformNormal(targetTransform->getMatrix());

			// update transform
			position = lookTarget+offset;
			// rotation = targetTransform->getRotation();
		}

		AglMatrix view = AglMatrix::createViewMatrix(position,
													 lookTarget,
													 up);

		// update of position
		transform->setTranslation( position );
		// transform->setRotation( rotation );
		
		AglMatrix viewProj = AglMatrix::identityMatrix();
		viewProj = view * camInfo->m_projMat;
		viewProj = AglMatrix::transpose( viewProj );
		
		RendererSceneInfo tempSceneInfo;
		for( int i=0; i<16; i++ )
		{
			tempSceneInfo.viewProjectionMatrix[i] = viewProj[i];
		}

		// sets up certain "global" scene data
		GraphicsWrapper* gfxWrapper = m_gfxBackend->getGfxWrapper();
		gfxWrapper->setSceneInfo(tempSceneInfo);
	}
}
