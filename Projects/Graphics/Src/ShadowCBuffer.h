#pragma once
#include <AglMatrix.h>
#include <Globals.h>

// =======================================================================================
//                                      ShadowCBuffer
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	Brief
///        
/// # ShadowCBuffer
/// Detailed description.....
/// Created on: 31-1-2013 
///---------------------------------------------------------------------------------------

struct ShadowCBuffer
{
	float viewProj[16*MAXSHADOWS];

	void setViewProjection(const AglMatrix& p_viewProj){
		for (unsigned int i = 0; i < 16; i++){
			viewProj[i] = p_viewProj[i];
		}
	}
};