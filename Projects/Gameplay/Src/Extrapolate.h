#pragma once

#include "Component.h"
#include <AglVector3.h>

// =======================================================================================
//	Extrapolate
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief A component that keeps track of last instance of network update regarding
/// an entity's velocity and WHEN that velocity were set.
///        
/// # Extrapolate
/// Used for extrapolating positions of moving entities. Should remove most jittering
/// problems caused by the asynchronous receiving of packets.
/// Created on: 15-1-2013 
///---------------------------------------------------------------------------------------

class Extrapolate: public Component
{
public:
	Extrapolate()
	{
		lastUpdateServerTimeStamp = 0;
		lastVelocityVector = AglVector3();
	}

public:
	float lastUpdateServerTimeStamp;
	AglVector3 lastVelocityVector;

};