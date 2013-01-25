#include "LightsComponent.h"

#include <LightInstanceData.h>
#include <AglMatrix.h>
#include <algorithm>

Light::Light()
{
	//scale = AglVector3::one();
	//rotation = AglQuaternion( 0.0f, 0.0f, 0.0f, 0.0f );
	//translation = AglVector3::zero();
	offsetMat = AglMatrix::identityMatrix();
}


LightsComponent::LightsComponent()
{
	m_componentType = ComponentType::LightsComponent;
}

LightsComponent::~LightsComponent()
{
}

vector<Light>* LightsComponent::getLightsPtr()
{
	return &m_lights;
}

void LightsComponent::addLight( Light p_light  )
{
	m_lights.push_back( p_light );
}

void LightsComponent::init( vector<ComponentData> p_initData )
{
	for( unsigned int i=0; i<p_initData.size(); i++ )
	{
		// Find the light index and make sure there's room for it
		string idxAsStr = p_initData[i].dataName.substr( 0, 1 );
		int lightIdx = atoi( idxAsStr.c_str() );

		if( lightIdx >= m_lights.size() ) {
			m_lights.resize( lightIdx+1 );
		}

		// Read data about the particular light
		string lightInfoStr = p_initData[i].dataName.substr( 2 );
		
		AglVector3 scale = AglVector3::one();
		AglQuaternion rotation = AglQuaternion::identity();
		AglVector3 translation = AglVector3::zero();

		if( lightInfoStr == "m_scaleX"){
			p_initData[i].getData<float>( &scale.x );
		} else if( lightInfoStr == "m_scaleY"){
			p_initData[i].getData<float>( &scale.y );
		} else if( lightInfoStr == "m_scaleZ"){
			p_initData[i].getData<float>( &scale.z );
		} else if( lightInfoStr == "m_rotationUX"){
			p_initData[i].getData<float>( &rotation.u.x );
		} else if( lightInfoStr == "m_rotationUY"){
			p_initData[i].getData<float>( &rotation.u.y );
		} else if( lightInfoStr == "m_rotationUZ"){ 
			p_initData[i].getData<float>( &rotation.u.z );
		} else if( lightInfoStr == "m_rotationV"){
			p_initData[i].getData<float>( &rotation.v );
		} else if( lightInfoStr == "m_translationX"){
			p_initData[i].getData<float>( &translation.x );
		} else if( lightInfoStr == "m_translationY"){
			p_initData[i].getData<float>( &translation.y );
		} else if( lightInfoStr == "m_translationZ"){ 
			p_initData[i].getData<float>( &translation.z );
		} else if( lightInfoStr == "range"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.range );
		} else if( lightInfoStr == "lightDirX"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.lightDir[0] );
		} else if( lightInfoStr == "lightDirY"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.lightDir[1] );
		} else if( lightInfoStr == "lightDirZ"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.lightDir[2] );
		} else if( lightInfoStr == "attenuationX"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.attenuation[0] );
		} else if( lightInfoStr == "attenuationY"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.attenuation[1] );
		} else if( lightInfoStr == "attenuationZ"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.attenuation[2] );
		} else if( lightInfoStr == "spotPower"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.spotPower );
		} else if( lightInfoStr == "ambientR"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.ambient[0] );
		} else if( lightInfoStr == "ambientG"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.ambient[1] );
		} else if( lightInfoStr == "ambientB"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.ambient[2] );
		} else if( lightInfoStr == "diffuseR"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.diffuse[0] );
		} else if( lightInfoStr == "diffuseG"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.diffuse[1] );
		} else if( lightInfoStr == "diffuseB"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.diffuse[2] );
		} else if( lightInfoStr == "specularR"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.specular[0] );
		} else if( lightInfoStr == "specularG"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.specular[1] );
		} else if( lightInfoStr == "specularB"){ 
			p_initData[i].getData<float>( &m_lights[lightIdx].instanceData.specular[2] );
		} else if( lightInfoStr == "enabled"){ 
			p_initData[i].getData<int>( &m_lights[lightIdx].instanceData.enabled );
		} else if( lightInfoStr == "typeAsNum"){ 
			p_initData[i].getData<int>( &m_lights[lightIdx].instanceData.type );
		} else if( lightInfoStr == "typeAsStr" || lightInfoStr == "typeAsString" ) {
			// special case when type is given as a string and not int
			string lightType;
			p_initData[i].getDataAsString(&lightType);
			std::transform( lightType.begin(), lightType.end(), lightType.begin(), ::tolower );

			if( lightType == "directional" || 
				lightType == "directionallight" || 
				lightType == "directional_light") 
			{ 
				m_lights[lightIdx].instanceData.type = LightTypes::E_LightTypes_DIRECTIONAL;
			} 
			else if( lightType == "point" || 
				lightType == "pointlight" || 
				lightType == "point_light") 
			{ 
				m_lights[lightIdx].instanceData.type = LightTypes::E_LightTypes_POINT;
			} 
			else if( lightType == "spot" || 
				lightType == "spotlight" || 
				lightType == "spot_light")
			{ 
				m_lights[lightIdx].instanceData.type = LightTypes::E_LightTypes_SPOT;
			}
		}
	}
}