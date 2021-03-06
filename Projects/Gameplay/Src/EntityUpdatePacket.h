#pragma once

#include "EntityType.h"
#include <AglVector3.h>
#include <AglQuaternion.h>

// =======================================================================================
//                                      EntityUpdatePacket
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	Handles the packing and unpacking of entity updates.
///        
/// # EntityUpdatePacket
/// Before: 73 bytes. (With AngularVelocity and Velocity)
/// Now: 49 bytes.
/// Created on: 14-1-2013 
///---------------------------------------------------------------------------------------

struct EntityUpdatePacket
{
public:
	EntityUpdatePacket();

public:
	/************************************************************************/
	/* EntityType Might not be needed as the networkIdentity can fetch the	*/
	/* type?																*/
	/************************************************************************/
	char entityType; 
	int networkIdentity;
	AglVector3		translation;
	AglVector3		scale;
	AglQuaternion	rotation;
	float timestamp;
};