#pragma once

// =======================================================================================
//                                      LookAtEntity
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	Store reference id to look-at target as well as an offset for the observer.
///        
/// # LookAtEntity
/// Detailed description.....
/// Created on: 14-12-2012 
///---------------------------------------------------------------------------------------

#pragma once

#include "Component.h"
#include <AglVector3.h>
#include <GraphicsWrapper.h>
#include <RendererSceneInfo.h>

class LookAtEntity : public Component
{
public:
	LookAtEntity(int p_entityId,
		float p_moveSpeed=1.0f,float p_rotationSpeed=1.0f)
		: Component( ComponentType::LookAtEntity )
	{
		m_entityId = p_entityId;
		m_followPositionOffset = AglVector3::zero();
		m_orbitMovement = AglVector3::zero();
		m_moveSpd = p_moveSpeed;
		m_rotationSpd = p_rotationSpeed;
		m_orbitRotationSpd=1.0f;
		m_orbitDistance = 1.0f;
	}

	LookAtEntity(int p_entityId,
		const AglVector3& p_followPositionOffset,
		float p_moveSpeed=1.0f,float p_rotationSpeed=1.0f
		)
		: Component( ComponentType::LookAtEntity )
	{
		m_entityId = p_entityId;
		m_followPositionOffset = p_followPositionOffset;
		m_orbitMovement = AglVector3::zero();
		m_moveSpd = p_moveSpeed;
		m_rotationSpd = p_rotationSpeed;
		m_orbitRotationSpd=1.0f;
		m_orbitDistance = 1.0f;
	}

	LookAtEntity(int p_entityId,
				const AglVector3& p_followPositionOffset,
				float p_moveSpeed,float p_rotationSpeed,
				float p_orbitRotationSpd,float p_orbitDistance
				)
		: Component( ComponentType::LookAtEntity )
	{
		m_entityId = p_entityId;
		m_followPositionOffset = p_followPositionOffset;
		m_orbitMovement = AglVector3::zero();
		m_moveSpd = p_moveSpeed;
		m_rotationSpd = p_rotationSpeed;
		m_orbitDistance = p_orbitDistance;
		m_orbitRotationSpd=p_orbitRotationSpd;
	}

	~LookAtEntity() {}

	const AglVector3& getFollowPositionOffset() {return m_followPositionOffset;}
	const AglVector3& getOrbitMovement() {return m_orbitMovement;}
	float getOrbitDistance() {return m_orbitDistance;}
	int getEntityId() {return m_entityId;}
	float getRotationSpeed() {return m_rotationSpd;}
	float getOrbitRotationSpeed() {return m_orbitRotationSpd;}
	AglQuaternion& getOrbitOffset() {return m_orbitOffset;}
	float getMoveSpd() {return m_moveSpd;}

	void setFollowPositionOffset(const AglVector3& p_offset) {m_followPositionOffset=p_offset;}
	void setOrbitMovement(const AglVector3& p_movement) {m_orbitMovement=p_movement;}
	void setOrbitDistance(float p_orbitDistance) {m_orbitDistance = p_orbitDistance;}
	void setRotationSpeed(float p_val) {m_rotationSpd=p_val;}
	void setOrbitRotationSpeed(float p_val) {m_orbitRotationSpd=p_val;}
	void setOrbitOffset(AglQuaternion& p_val) {m_orbitOffset=p_val;}
	void setMoveSpeed(float p_val) {m_moveSpd=p_val;}	
private:
	int m_entityId;
	AglVector3 m_followPositionOffset; ///< offset position from target
	AglQuaternion m_orbitOffset;
	AglVector3 m_orbitMovement; ///< plane movement for orbit
	float m_rotationSpd; ///< Speed of rotation towards goal
	float m_orbitRotationSpd; ///< Speed of rotation towards goal in orbit
	float m_moveSpd; ///< Speed of movement towards goal  
	float m_orbitDistance; ///< distance from target when orbiting

};

