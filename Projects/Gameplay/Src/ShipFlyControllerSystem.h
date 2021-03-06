#pragma once

#include <EntitySystem.h>
#include <AglVector3.h>

class Control;
class TcpClient;
class PhysicsSystem;
class ShipInputProcessingSystem;
class SlotInputControllerSystem;
class NetworkSynced;
class ShipFlyController;
class Transform;

// =======================================================================================
//                                ShipControllerSystem
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	System for controlling a ship
///        
/// # ShipControllerSystem
/// Detailed description.....
/// Created on: 13-12-2012 
///---------------------------------------------------------------------------------------

class ShipFlyControllerSystem : public EntitySystem
{
public:
	ShipFlyControllerSystem(ShipInputProcessingSystem* p_shipInput,
						 PhysicsSystem* p_physicsSystem,
						 TcpClient* p_client,
						 SlotInputControllerSystem* p_slotInput );
	~ShipFlyControllerSystem();

	void initialize();
	void processEntities( const vector<Entity*>& p_entities );
private:
	void	sendThrustPacketToServer(NetworkSynced* p_syncedInfo, AglVector3& p_thrust,
		AglVector3& p_angularVec);
	void handleIngame( const vector<Entity*>& p_entities );
	void updateThrustComponent(const AglVector3& p_thrust, const AglVector3& p_angular);
private:
	PhysicsSystem* m_physics;
	TcpClient* m_client;
	SlotInputControllerSystem* m_slotInput;
	ShipInputProcessingSystem* m_shipInput;
};