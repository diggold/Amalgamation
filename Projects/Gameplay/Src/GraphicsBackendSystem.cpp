#include "GraphicsBackendSystem.h"
#include <TextureParser.h>
#include <DebugUtil.h>
#include <EntitySystem.h>
#include <GraphicsWrapper.h>
#include <Window.h>
#include <SystemType.h>
#include <ComponentType.h>
#include "CameraInfo.h"
#include <AglMesh.h>
#include <RendererSceneInfo.h>
#include "Transform.h"
#include "ShipHiglightSystem.h"
#include "SettingsSystem.h"

Window* GraphicsBackendSystem::m_window = NULL;

GraphicsBackendSystem* GraphicsBackendSystem::m_selfPointer = NULL;

GraphicsBackendSystem::GraphicsBackendSystem( 
	HINSTANCE p_hInstance, GameSettingsInfo& p_settings) 
	: EntitySystem( SystemType::GraphicsBackendSystem )										 
{
	m_hInstance = p_hInstance;
	applySettings( p_settings );

	m_newWidth = m_scrWidth;
	m_newHeight = m_scrHeight;

	/************************************************************************/
	/* ONLY NEEDED OF THE ANTTWEAKBAR CALLBACK								*/
	/************************************************************************/
	m_selfPointer = this;	
	m_wireframe = false;
}


GraphicsBackendSystem::~GraphicsBackendSystem(void)
{
	//delete m_window;
	delete m_graphicsWrapper;
	AntTweakBarWrapper::destroy();
}

void GraphicsBackendSystem::changeResolution( int p_scrWidth, int p_scrHeight )
{
	m_scrWidth = p_scrWidth;
	m_scrHeight = p_scrHeight;

	// update info of aspect ratio to world
	m_world->setAspectRatio(getAspectRatio());

	// Resize the actual window.
	m_window->changeWindowRes( p_scrWidth, p_scrHeight );

	// Resize the back buffer.
	m_graphicsWrapper->changeBackbufferRes( p_scrWidth, p_scrHeight );	

	// recalc projection matrix
	Entity* mainCamera =
		m_world->getEntityManager()->getFirstEntityByComponentType(
		ComponentType::TAG_MainCamera );
	if( mainCamera != NULL )
	{
		CameraInfo* cameraInfo = static_cast<CameraInfo*>( mainCamera->getComponent(
			ComponentType::CameraInfo ) );
		cameraInfo->createPerspectiveMatrix(getAspectRatio());
	}

	// Inform AntTweakBar
	AntTweakBarWrapper::getInstance()->setWindowSize( p_scrWidth, p_scrHeight );
}

void GraphicsBackendSystem::initialize()
{
	// update info of aspect ratio to world
	m_world->setAspectRatio(getAspectRatio());

	TextureParser::init();

	auto settings = static_cast<SettingsSystem*>(m_world->getSystem(SystemType::SettingsSystem));

	if (!m_window)
		m_window = new Window( m_hInstance, m_scrWidth, m_scrHeight, 1);
	m_graphicsWrapper = new GraphicsWrapper( 
		m_window->getWindowRef(), 
		m_scrWidth, 
		m_scrHeight, 
		m_windowed,
		m_enableHdr,
		m_enableEffects,
		settings->getSettings().enableVSYNC);

	AntTweakBarWrapper::getInstance( m_graphicsWrapper->getDevice());

	TwAddButton(AntTweakBarWrapper::getInstance()->getAntBar(AntTweakBarWrapper::OVERALL),
		"Toggle_Windowed/FullScreen",
		toggleFullScreen, (void*)NULL, "");

	m_graphicsWrapper->hookUpAntTweakBar();

	// Anttweakbar resolution
	AntTweakBarWrapper::getInstance()->addWriteVariable( AntTweakBarWrapper::OVERALL, 
		"Win width", TwType::TW_TYPE_INT32, &m_newWidth, "min=800 max=4096" );
	
	AntTweakBarWrapper::getInstance()->addWriteVariable( AntTweakBarWrapper::OVERALL,
		"Win height", TwType::TW_TYPE_INT32, &m_newHeight, "min=480 max=4096" );

	TwAddButton(AntTweakBarWrapper::getInstance()->getAntBar(AntTweakBarWrapper::OVERALL),
		"Apply resolution",
		applyNewResolution, (void*)NULL, "");

	TwAddButton(AntTweakBarWrapper::getInstance()->getAntBar(AntTweakBarWrapper::OVERALL),
		"Toggle wireframe",
		toggleWireframe, (void*)NULL, "");


	//Some handling of ship highlighting
	ShipHighlightSystem* shs = static_cast<ShipHighlightSystem*>(m_world->getSystem(SystemType::ShipHighlightSystem));

	TwAddVarRW(AntTweakBarWrapper::getInstance()->getAntBar(AntTweakBarWrapper::OVERALL), "Highlight Min", 
		TW_TYPE_FLOAT, shs->getMinDistance(), "min=0.0 max=1000.0");
	TwAddVarRW(AntTweakBarWrapper::getInstance()->getAntBar(AntTweakBarWrapper::OVERALL), "Highlight Max", 
		TW_TYPE_FLOAT, shs->getMaxDistance(), "min=0.0 max=1000.0");
}

void GraphicsBackendSystem::process()
{
}

// vector<Entity*> GraphicsBackendSystem::buildEntitiesFromMeshFile( const string& p_meshName, 
// 																 const string* p_path/*=NULL*/ )
// {
// 	return vector<Entity*>();
// 	// return m_graphicsWrapper->createMeshFromRaw(p_meshName,p_path,p_outConnectionPoints);
// }

// int GraphicsBackendSystem::loadSingleMesh(const string& p_meshName, 
// 						   const string* p_path/*=NULL*/ )
// {
// 	return m_graphicsWrapper->createModelFromFile(p_meshName,p_path)->meshId;
// }

int GraphicsBackendSystem::getMeshId( const string& p_meshName )
{
	return m_graphicsWrapper->getMeshId(p_meshName);
}

GraphicsWrapper* GraphicsBackendSystem::getGfxWrapper()
{
	return m_graphicsWrapper;
}

HWND GraphicsBackendSystem::getWindowRef()
{
	return m_window->getWindowRef();
}

void TW_CALL GraphicsBackendSystem::toggleFullScreen(void* p_clientData)
{
	m_selfPointer->m_windowed = !m_selfPointer->m_windowed;
	m_selfPointer->m_graphicsWrapper->changeToWindowed(m_selfPointer->m_windowed);
}

void TW_CALL GraphicsBackendSystem::toggleWireframe(void* p_clientData)
{
	m_selfPointer->m_wireframe = !m_selfPointer->m_wireframe;
	m_selfPointer->m_graphicsWrapper->setWireframeMode(m_selfPointer->m_wireframe);
}

void TW_CALL GraphicsBackendSystem::applyNewResolution( void* p_clientData )
{
	m_selfPointer->changeResolution(m_selfPointer->m_newWidth,
									m_selfPointer->m_newHeight);
}

float GraphicsBackendSystem::getAspectRatio()
{
	return (float)m_scrWidth / m_scrHeight;
}

void GraphicsBackendSystem::renderParticleSystem( ParticleSystemAndTexture* p_system,
												 const InstanceData& p_worldTransform )
{
	m_graphicsWrapper->renderParticleSystem( p_system, p_worldTransform );
}

AglVector2 GraphicsBackendSystem::getWindowSize(){
	return AglVector2((float)m_scrWidth, (float)m_scrHeight);
}

void GraphicsBackendSystem::applySettings( GameSettingsInfo& p_settings )
{
	m_scrWidth		= p_settings.screenWidth;
	m_scrHeight		= p_settings.screenHeight;
	m_windowed		= p_settings.windowed;
	m_enableHdr		= p_settings.enableHdr;
	m_enableEffects = p_settings.enableEffects;
	m_vsync			= p_settings.enableVSYNC;
	m_tesselation	= p_settings.enableTesselation;
}
