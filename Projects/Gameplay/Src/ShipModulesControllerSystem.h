#pragma once

#include <EntitySystem.h>
#include "AglVector3.h"
#include "ConnectionPointSet.h"

class Transform;
class TcpServer;
class ModuleVisualEffectServerBufferSystem;

// =======================================================================================
//                                      ShipModulesControllerSystem
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	System for handling communication with the physics library
///        
/// # ShipModulesControllerSystem
/// Detailed description.....
/// Created on: 11-12-2012 
///---------------------------------------------------------------------------------------

class ShipModulesControllerSystem: public EntitySystem
{
public:
	ShipModulesControllerSystem(TcpServer* p_server,
		ModuleVisualEffectServerBufferSystem* p_effectBuffer,
		PhysicsSystem* p_physicsSystem);
	~ShipModulesControllerSystem();

	virtual void initialize();
	void processEntities(const vector<Entity*>& p_entities );

	void applyImpulse(int p_bodyID, AglVector3 p_impulse, AglVector3 p_angularImpulse);

	void addHighlightEvent(int p_slot, int p_id,int p_status=1);
	void addActivateEvent(int p_index);
	void addDeactivateEvent(int p_index);
	void setEditMode(bool p_editMode);
	vector<Entity*> getModulesBySlot(Entity* p_shipEntity, int p_slotId);
	void getConnectedChildren(Entity* p_root, vector<Entity*>* p_list);

private:
	void checkDrop_ApplyScoreAndDamage(Entity* p_parent, pair<float,int>& p_outMassBoosters);
	void drop(Entity* p_parent, unsigned int p_slot);
	void changeHighlight(Entity* p_entity, int p_new, int p_status);
	void setActivation(Entity* p_entity, bool p_value);
	void setActivationChildren(Entity* p_entity, bool p_value);
	// float calculateScore(Entity* p_entity);

	// effect visualization for client
	void setScoreEffect(int p_networkOwner, Transform* p_moduleTransform, int p_score);
	void enableModuleUnusuedEffect(int p_moduleNetworkOwner);
	void disableModuleUnusuedEffect(int p_moduleNetworkOwner);
	void updateModuleHealthEffect(int p_moduleNetworkOwner, float p_healthPercent);
	void updateModuleValueEffect(int p_moduleNetworkOwner, float p_valuePercent);
	void sendMassBoostersTotal(int p_moduleNetworkOwner, float p_massValue, int p_boosters);
private:
	struct HighlightEvent
	{		
		int id;
		int slot;
		int status;
	};

	vector<HighlightEvent> m_toHighlight;
	vector<int> m_toActivate;
	vector<int> m_toDeactivate;

	// system dependencies
	TcpServer* m_server;
	ModuleVisualEffectServerBufferSystem* m_effectbuffer;
	PhysicsSystem* m_physicsSystem;

	bool m_editMode;

};