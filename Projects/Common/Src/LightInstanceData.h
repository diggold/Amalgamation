#pragma once
#include <AglVector3.h>
#include <AglMatrix.h>

// =======================================================================================
//                                      LightInstanceData
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	Data for an 
/// # InstanceVertex
/// Detailed description.....
/// Created on: 7-12-2012 
///---------------------------------------------------------------------------------------

struct TransformComponents
{
	AglVector3 scale;
	AglQuaternion rotation;
	AglVector3 translation;

	TransformComponents()
	{
		scale = AglVector3::one();
		rotation = AglQuaternion::identity();
		translation = AglVector3::zero();
	}

	AglMatrix toMatrix()
	{
		AglMatrix mat; 
		AglMatrix::componentsToMatrix( mat, scale, rotation, translation );
		return mat;
	}
};


struct LightTypes
{
	enum E_LightTypes
	{
		E_LightTypes_DIRECTIONAL,
		E_LightTypes_POINT,
		E_LightTypes_SPOT,
	};
};

struct LightInstanceData
{
	float worldTransform[16]; /* Used internally to set the correct matrix.
								 Will be overwritten every frame.*/

	float lightDir[3];
	float range;

	float attenuation[3];			// Used in the formula: x, x^2, x^3
	float spotLightConeSizeAsPow;		// Spot cone width. Power of 10.

	float color[3];
	float lightEnergy;

	int enabled;
	int type;				// Use E_LightTypes
	int shadowIdx;

	//float pad[1];			// Padding

	LightInstanceData()
	{
		enabled = true;
		shadowIdx = -1;
		type = LightTypes::E_LightTypes_DIRECTIONAL;
		range = 0.0f;
		spotLightConeSizeAsPow = 0.0f;
		
		//Identity
		for( int i=0; i<16; i++) {
			worldTransform[i] = 0.0f;
		}
		worldTransform[0] = worldTransform[5] = worldTransform[10] = worldTransform[15] = 1.0f;


		for( int i=0; i<3; i++) {
			lightDir[0] = 0.5773f; // 1/sqrt(3)
			attenuation[i] = 0.0f;
		}

		for( int i=0; i<3; i++) {
			//ambient[i] = 0.0f;
			color[i] = 0.0f;
			//specular[i] = 0.0f;
		}

	}

	///-----------------------------------------------------------------------------------
	/// Used internally to set the correct matrix. Will be overwritten every frame.
	/// \param p_transform
	/// \return void
	///-----------------------------------------------------------------------------------
	void setWorldTransform( AglMatrix p_transform )
	{
		p_transform = AglMatrix::transpose( p_transform );
		for( int i=0; i<16; i++ ) {
			worldTransform[i] = p_transform[i];
		}
	}

};