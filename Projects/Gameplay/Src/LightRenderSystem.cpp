#include "LightRenderSystem.h"

#include "GraphicsBackendSystem.h"
#include <BufferFactory.h>
#include <GraphicsWrapper.h>
#include <InstanceData.h>
#include <Mesh.h>
#include <LightInstanceData.h>


LightRenderSystem::LightRenderSystem( GraphicsBackendSystem* p_gfxBackend )
	: EntitySystem( SystemType::LightRenderSystem )
{
	m_gfxBackend = p_gfxBackend;

	m_box = NULL;
}


LightRenderSystem::~LightRenderSystem()
{
}

void LightRenderSystem::initialize()
{
	GraphicsWrapper* gfxWrapper = m_gfxBackend->getGfxWrapper();

	BufferFactory* bufferFactory = new BufferFactory( gfxWrapper->getDevice(),
		gfxWrapper->getDeviceContext() );

	m_box = bufferFactory->createLightBoxMesh();
	delete bufferFactory;
}

void LightRenderSystem::processEntities( const vector<Entity*>& p_entities )
{
	GraphicsWrapper* gfxWrapper = m_gfxBackend->getGfxWrapper();
	
	gfxWrapper->beginLightPass();			  // finalize, draw to back buffer
	//gfxWrapper->renderLights( NULL, NULL );

	static float range = 10.0f;
	AntTweakBarWrapper::getInstance()->addWriteVariable(AntTweakBarWrapper::GRAPHICS, "lightCubeWidth", TwType::TW_TYPE_FLOAT, &range,"");

	AglMatrix mat = AglMatrix::identityMatrix();
	mat[0] = mat[5] = mat[10] =  range / 2.0f; // The cube is 2.0f wide, therefore 2 and not 1
	
	LightInstanceData instData;
	instData.range = range; // Should be synced with wolrdTransform
	for( int i=0; i<16; i++ ){
		instData.worldTransform[i] = mat[i];
	}

	instData.lightDir[0] = 1.0f;
	instData.lightDir[1] = 0.0f;
	instData.lightDir[2] = 0.0f;

	instData.attenuation[0] = 1.1f;
	instData.attenuation[1] = 0.01f;
	instData.attenuation[2] = 0.0f;
	instData.spotPower = 100.0f;

	instData.ambient[0] = 0.0f;
	instData.ambient[1] = 0.0f;
	instData.ambient[2] = 0.0f;
	instData.ambient[3] = 1.0f;

	instData.diffuse[0] = 0.0f;
	instData.diffuse[1] = 0.5f;
	instData.diffuse[2] = 0.0f;
	instData.diffuse[3] = 1.0f;

	instData.specular[0] = 0.5f;
	instData.specular[1] = 0.1f;
	instData.specular[2] = 0.0f;
	instData.specular[3] = 1.0f;

	instData.enabled = true;
	instData.type = LightTypes::E_LightTypes_POINT;

	vector<LightInstanceData> instDatas;
	for( int x=0; x<5; x++ )
	{
		instData.worldTransform[3] = x * (range+1.0f) - 25.0f;
		instData.diffuse[1] += 0.1f;
		for( int y=0; y<5; y++ )
		{
			instData.worldTransform[7] = y * (range+1.0f) - 25.0f;
			instData.diffuse[2] += 0.1f;
			for( int z=0; z<5; z++ )
			{
				instData.worldTransform[11] = z * (range+1.0f) - 25.0f;
				instData.diffuse[3] += 0.1f;
				instDatas.push_back( instData );
			}
		}
	}

	gfxWrapper->renderLights( m_box, &instDatas );
	 
	gfxWrapper->endLightPass();
}