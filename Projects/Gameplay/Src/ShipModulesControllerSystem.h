#pragma once

#include <EntitySystem.h>
#include "AglVector3.h"
#include "ConnectionPointSet.h"

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
	ShipModulesControllerSystem();
	~ShipModulesControllerSystem();

	virtual void initialize();
	void processEntities(const vector<Entity*>& p_entities );

	void applyImpulse(int p_bodyID, AglVector3 p_impulse, AglVector3 p_angularImpulse);

	void addHighlightEvent(int p_slot, int p_id,int p_status=1);
	void addActivateEvent(int p_index);
	void addDeactivateEvent(int p_index);
private:
	void checkDrop(Entity* p_parent);
	void drop(Entity* p_parent, unsigned int p_slot);
	void changeHighlight(Entity* p_entity, int p_new, int p_status);
	void setActivation(Entity* p_entity, bool p_value);
	void setActivationChildren(Entity* p_entity, bool p_value);
	float calculateScore(Entity* p_entity);
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


};