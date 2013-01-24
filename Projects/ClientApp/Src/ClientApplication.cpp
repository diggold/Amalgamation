#include "ClientApplication.h"
#include <windows.h>

#ifdef COMBINE_CLIENT_AND_SERVER
	#include "ServerApplication.h"
#endif

#include <EntityWorld.h>
#include <Input.h>
#include <ComponentAssemblageAllocator.h>

// Components
#include <AudioInfo.h>
#include <AudioListener.h>
#include <BodyInitData.h>
#include <ConnectionPointSet.h>
#include <GameplayTags.h>
#include <HudElement.h>
#include <MinigunModule.h>
#include <PhysicsBody.h>
#include <PlayerCameraController.h>
#include <RenderInfo.h>
#include <ShipEditController.h>
#include <ShipFlyController.h>
#include <ShipModule.h>
#include <SpeedBoosterModule.h>
#include <MinigunModule.h>
#include <ShieldModule.h>
#include <MineLayerModule.h>
#include <RocketLauncherModule.h>
#include <Connector1to2Module.h>
#include <Transform.h>
#include <PositionalSoundSource.h>
#include <DebugMove.h>
#include <EntityParent.h>
#include <LoadMesh.h>

// Systems
#include <AudioBackendSystem.h>
#include <AudioController.h>
#include <AudioListenerSystem.h>
#include <CameraInfo.h>
#include <CameraSystem.h>
#include <ClientPacketHandlerSystem.h>
#include <DisplayPlayerScoreSystem.h>
#include <EntityFactory.h>
#include <GraphicsBackendSystem.h>
#include <HudSystem.h>
#include <InputBackendSystem.h>
#include <LibRocketBackendSystem.h>
#include <LookAtEntity.h>
#include <LookAtSystem.h>
#include <MainCamera.h>
#include <MinigunModuleControllerSystem.h>
#include <ClientConnectToServerSystem.h>
#include <PhysicsSystem.h>
#include <ClientPickingSystem.h>
#include <PlayerCameraControllerSystem.h>
#include <ProcessingMessagesSystem.h>
#include <MeshRenderSystem.h>
#include <PhysicsSystem.h>
#include <ShipEditControllerSystem.h>
#include <ShipFlyControllerSystem.h>
#include <ShipInputProcessingSystem.h>
#include <DisplayPlayerScoreSystem.h>
#include <HudSystem.h>
#include <ShieldModuleControllerSystem.h>
#include <MineLayerModuleControllerSystem.h>
#include <MineControllerSystem.h>
#include <RocketLauncherModuleControllerSystem.h>
#include <ShipModulesControllerSystem.h>
#include <TimerSystem.h>
#include <LevelGenSystem.h>
#include <ExtrapolationSystem.h>
#include <PositionalSoundSystem.h>
#include <NetSyncedPlayerScoreTrackerSystem.h>
#include <GraphicsRendererSystem.h>
#include <DebugMovementSystem.h>
#include <LightRenderSystem.h>
#include <AntTweakBarSystem.h>
#include <ParticleRenderSystem.h>
#include <TransformParentHandlerSystem.h>
#include <LoadMeshSystem.h>

// Helpers
#include <ConnectionPointCollection.h>
#include <vector>

using namespace std;

// MISC
#include <AntTweakBarSystem.h>
#include <AntTweakBarWrapper.h>
#include <ParticleRenderSystem.h>
#include <LightsComponent.h>
#include <LightInstanceData.h>


ClientApplication::ClientApplication( HINSTANCE p_hInstance )
{
	try{
		m_running = false;
		m_hInstance = p_hInstance;
		m_client = new TcpClient();
		m_world = new EntityWorld();

#ifdef COMBINE_CLIENT_AND_SERVER
		m_serverApp = new Srv::ServerApplication();
#endif
		// Systems first!
		initSystems();

		// Test entities later!
		initEntities();
		initSounds();

	}
	catch(exception& e)
	{
		DEBUGWARNING((e.what()));
	}
}

ClientApplication::~ClientApplication()
{

#ifdef COMBINE_CLIENT_AND_SERVER
	ProcessMessage* newMessage = new ProcessMessage(MessageType::TERMINATE,NULL);
	m_serverApp->putMessage( newMessage );
	m_serverApp->stop();
	delete m_serverApp;
#endif
	delete m_world;
	delete m_client;
}

void ClientApplication::run()
{
#ifdef COMBINE_CLIENT_AND_SERVER
	m_serverApp->start();
#endif
	m_running = true;

	// simple timer
	__int64 countsPerSec = 0;
	__int64 currTimeStamp = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	double secsPerCount = 1.0f / (float)countsPerSec;

	double dt = 0.0f;
	__int64 m_prevTimeStamp = 0;

	QueryPerformanceCounter((LARGE_INTEGER*)&m_prevTimeStamp);
	QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);

	MSG msg = {0};
	while(m_running)
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
			dt = (currTimeStamp - m_prevTimeStamp) * secsPerCount;

			m_prevTimeStamp = currTimeStamp;

			m_world->setDelta((float)dt);
			m_world->process();

			if(m_world->shouldShutDown())
				m_running = false;
			
		}
	}
}

void ClientApplication::initSystems()
{
	//----------------------------------------------------------------------------------
	// Systems must be added in the order they are meant to be executed. The order the
	// systems are added here is the order the systems will be processed
	//----------------------------------------------------------------------------------	
	
	/************************************************************************/
	/* Entity creation														*/
	/************************************************************************/
	EntityFactory* factory = new EntityFactory();
	m_world->setSystem( factory, true);

	/************************************************************************/
	/* TimerSystem used by other systems should be first.					*/
	/************************************************************************/
	m_world->setSystem(SystemType::TimerSystem, new TimerSystem(), true);

	/************************************************************************/
	/* Graphics																*/
	/************************************************************************/
	GraphicsBackendSystem* graphicsBackend = new GraphicsBackendSystem( m_hInstance ,
		1280,720,true);

	m_world->setSystem( graphicsBackend, true );

	/************************************************************************/
	/* Mesh loading															*/
	/************************************************************************/
	// Note! Must set *after* EntityFactory and GraphicsBackend, and *before* Physics
	m_world->setSystem(SystemType::LoadMeshSystem, new LoadMeshSystem(graphicsBackend), 
						true); 

	/************************************************************************/
	/* Physics																*/
	/************************************************************************/
	PhysicsSystem* physics = new PhysicsSystem();
	m_world->setSystem(SystemType::PhysicsSystem, physics, true);

	/************************************************************************/
	/* General controlling													*/
	/************************************************************************/
	LookAtSystem* lookAtSystem = new LookAtSystem();
	m_world->setSystem(SystemType::LookAtSystem, lookAtSystem, true);
	
	/************************************************************************/
	/* Input																*/
	/************************************************************************/
	InputBackendSystem* inputBackend = new InputBackendSystem( m_hInstance, 
		graphicsBackend );
	m_world->setSystem( inputBackend, true);
	
	/************************************************************************/
	/* GUI																	*/
	/************************************************************************/
	LibRocketBackendSystem* rocketBackend = new LibRocketBackendSystem( graphicsBackend,
		inputBackend );
	m_world->setSystem( rocketBackend, true );

	HudSystem* hud = new HudSystem( rocketBackend );
	m_world->setSystem( hud, true );

	/************************************************************************/
	/* Player    															*/
	/************************************************************************/
	// Input system for ships
	ShipInputProcessingSystem* shipInputProc = new ShipInputProcessingSystem(inputBackend,
		m_client);
	m_world->setSystem( shipInputProc, true);

	// Controller systems for the ship
	ShipFlyControllerSystem* shipFlyController = new ShipFlyControllerSystem(shipInputProc, physics,
		m_client );
	m_world->setSystem( shipFlyController, true);

	ShipEditControllerSystem* shipEditController = new ShipEditControllerSystem(shipInputProc, physics/*,
		m_client*/ );
	m_world->setSystem( shipEditController, true);

	/************************************************************************/
	/* Camera																*/
	/************************************************************************/

	// Controller logic for camera
	PlayerCameraControllerSystem* cameraControl = new PlayerCameraControllerSystem( shipInputProc );
	m_world->setSystem( cameraControl , true );
	// Camera system sets its viewport info to the graphics backend for render
	CameraSystem* camera = new CameraSystem( graphicsBackend );
	m_world->setSystem( camera , true );

	/************************************************************************/
	/* Renderer																*/
	/************************************************************************/
	MeshRenderSystem* renderer = new MeshRenderSystem( graphicsBackend );
	m_world->setSystem( renderer , true );

	ParticleRenderSystem* particleRender = new ParticleRenderSystem( graphicsBackend );
	m_world->setSystem( particleRender, true );

	LightRenderSystem* lightRender = new LightRenderSystem( graphicsBackend );
	m_world->setSystem( lightRender, true );
	
	AntTweakBarSystem* antTweakBar = new AntTweakBarSystem( graphicsBackend, inputBackend );
	m_world->setSystem( antTweakBar, true );

	/************************************************************************/
	/* Hierarchy															*/
	/************************************************************************/
	EntityParentHandlerSystem* entityParentHandler = new EntityParentHandlerSystem();
	m_world->setSystem( entityParentHandler, true );

	/************************************************************************/
	/* Network																*/
	/************************************************************************/
	ProcessingMessagesSystem* msgProcSystem = new ProcessingMessagesSystem( m_client );
	m_world->setSystem( msgProcSystem , true );

	ClientConnectToServerSystem* connect =
		new ClientConnectToServerSystem( m_client);
	m_world->setSystem( connect, true );

	ClientPacketHandlerSystem* communicatorSystem =
		new ClientPacketHandlerSystem( m_client );
	m_world->setSystem( communicatorSystem, false );
	m_world->setSystem( new NetSyncedPlayerScoreTrackerSystem(), true );
	m_world->setSystem( new ExtrapolationSystem(m_client), true );

	/************************************************************************/
	/* Audio																*/
	/************************************************************************/
#ifdef ENABLE_SOUND
	AudioBackendSystem* audioBackend = new AudioBackendSystem();
	m_world->setSystem( SystemType::AudioBackendSystem, audioBackend, true);

	AudioController* audioController = new AudioController(audioBackend);
	m_world->setSystem( SystemType::AudioControllerSystem, audioController, true);

	AudioListenerSystem* audioListener = new AudioListenerSystem(audioBackend);
	m_world->setSystem( SystemType::AudioListenerSystem, audioListener, true);

	m_world->setSystem( SystemType::PositionalSoundSystem, new PositionalSoundSystem(),
		true );
#endif // ENABLE_SOUND

	/************************************************************************/
	/* Gameplay																*/
	/************************************************************************/
	m_world->setSystem( new DisplayPlayerScoreSystem(), true );
	m_world->setSystem( new ClientPickingSystem(m_client), true );

	/************************************************************************/
	/* Graphics representer													*/
	/************************************************************************/
	GraphicsRendererSystem* graphicsRender = new GraphicsRendererSystem(graphicsBackend,
		renderer, rocketBackend, particleRender, antTweakBar, lightRender);
	m_world->setSystem( graphicsRender, true );

	/************************************************************************/
	/* Debugging															*/
	/************************************************************************/
	m_world->setSystem( new DebugMovementSystem(), true );

	m_world->initialize();

	// Run component assemblage allocator
	ComponentAssemblageAllocator* allocator = new ComponentAssemblageAllocator();
	delete allocator;
}

void ClientApplication::initEntities()
{
	Entity* entity = NULL;
	Component* component = NULL;

	// Read from assemblage
	AssemblageHelper::E_FileStatus status = AssemblageHelper::FileStatus_OK;
	EntityFactory* factory = static_cast<EntityFactory*>
		( m_world->getSystem( SystemType::EntityFactory ) );

	// Score HUD
	status = factory->readAssemblageFile( "Assemblages/ScoreHudElement.asd" );
	entity = factory->entityFromRecipe( "ScoreHudElement" );									 
	m_world->addEntity( entity );

	// Read monkey!
	status = factory->readAssemblageFile( "Assemblages/SpecialMonkey.asd" );
	entity = factory->entityFromRecipe( "SpecialMonkey" );									 
	m_world->addEntity( entity );



	EntitySystem* tempSys = NULL;

	// Load cube model used as graphic representation for all "graphical" entities.
	tempSys = m_world->getSystem(SystemType::GraphicsBackendSystem);
	GraphicsBackendSystem* graphicsBackend = static_cast<GraphicsBackendSystem*>(tempSys);
	int cubeMeshId = graphicsBackend->loadSingleMeshFromFile( "P_cube" );
	int shipMeshId = graphicsBackend->loadSingleMeshFromFile( "Ship.agl", &MODELPATH );
	int sphereMeshId = graphicsBackend->loadSingleMeshFromFile( "P_sphere" );

	LevelPieceFileMapping modelLevelFileMapping;	
	for (int i = 0; i < modelLevelFileMapping.getModelFileCount() - 1; i++)
	{
		string modelName = modelLevelFileMapping.getModelFileName(i);
		graphicsBackend->loadSingleMeshFromFile( modelName,
				&TESTMODELPATH);
	}

	// Ambient light
	float scale = 1000.0f;
	Light ambientLight;
	AglMatrix::componentsToMatrix(
		ambientLight.offset,
		AglVector3( scale, scale, scale ),
		AglQuaternion::constructFromAxisAndAngle( AglVector3(-1,0,0), 3.14/2.0 ),
		AglVector3(3,3,3)
		);
	ambientLight.instanceData.range = scale;
	ambientLight.instanceData.attenuation[0] = 1.0f;
	ambientLight.instanceData.ambient[0] = 0.2;
	ambientLight.instanceData.ambient[1] = 0.2;
	ambientLight.instanceData.ambient[2] = 0.2f;

	LightsComponent* ambientLightComp = new LightsComponent();
	ambientLightComp->addLight( ambientLight );
	
	entity = m_world->createEntity();
	component = new Transform( 5.0f, 10.0f, 19.0f);
	entity->addComponent( ComponentType::Transform, component );
	entity->addComponent( ComponentType::LightsComponent, ambientLightComp );

	m_world->addEntity(entity);

	/************************************************************************/
	/* HARD CODED LIGHTS													*/
	/************************************************************************/

	LightsComponent* lightGridComp = new LightsComponent();
	LightInstanceData lightGridInstData;
	float range = 10.0f;
	lightGridInstData.range = range;
	lightGridInstData.worldTransform[0] = range;
	lightGridInstData.worldTransform[5] = range;
	lightGridInstData.worldTransform[10] = range;
	lightGridInstData.attenuation[0] = 0.0f;
	lightGridInstData.attenuation[1] = 0.0f;
	lightGridInstData.attenuation[2] = 0.7f;
	lightGridInstData.spotPower = 25.0f;
	lightGridInstData.specular[3] = 1.0f;
	lightGridInstData.type = LightTypes::E_LightTypes_POINT;
	lightGridInstData.ambient[2] = 0.0f;

	float intensitity = 0.2f;
	for( int x=0; x<5; x++ )
	{
		for( int y=0; y<5; y++ )
		{
			for( int z=0; z<5; z++ )
			{
				lightGridInstData.diffuse[0] = intensitity * x;
				lightGridInstData.diffuse[1] = intensitity * y;
				lightGridInstData.diffuse[2] = intensitity * z;

				Light light;
				light.instanceData = lightGridInstData;
				AglMatrix::componentsToMatrix( 
					light.offset,
					AglVector3( range, range, range ),
					AglQuaternion::identity(),
					AglVector3( -x*(range+1.0f), -y*(range+1.0f), -z*(range+1.0f) )
					);

				lightGridComp->addLight( light );
			}
		}
	}
	entity = m_world->createEntity();
	entity->addComponent( ComponentType::LightsComponent, lightGridComp );
	entity->addComponent( ComponentType::Transform, new Transform( range/2.0f, range/2.0f, range/2.0f ) );
	m_world->addEntity( entity );

	// Test sound source
	entity = m_world->createEntity();
	entity->addComponent(ComponentType::Transform, new Transform(0, 0, 0));
	entity->addComponent(ComponentType::RenderInfo, new RenderInfo(sphereMeshId));
	entity->addComponent(ComponentType::PositionalSoundSource, new PositionalSoundSource(
		TESTSOUNDEFFECTPATH,
		"Spaceship_Engine_Idle_-_Spaceship_Onboard_Cruise_Rumble_Drone_Subtle_Slow_Swells.wav"));
	entity->addComponent(ComponentType::DebugMove, new DebugMove(AglVector3(
		0, 1.0f, 0)));
	m_world->addEntity(entity);

	//InitModulesTestByAnton();

	/*
	//Create a camera
	float aspectRatio = 
		static_cast<GraphicsBackendSystem*>(m_world->getSystem(
		SystemType::GraphicsBackendSystem ))->getAspectRatio();

	entity = m_world->createEntity();
	component = new CameraInfo( aspectRatio );
	entity->addComponent( ComponentType::CameraInfo, component );
	component = new MainCamera();
	entity->addComponent( ComponentType::MainCamera, component );
	//component = new Input();
	//entity->addComponent( ComponentType::Input, component );
	component = new Transform( -5.0f, 0.0f, -5.0f );
	entity->addComponent( ComponentType::Transform, component );
	component = new LookAtEntity(shipId, 
								 AglVector3(0,3,-10),
								 AglQuaternion::identity(),
								 10.0f,
								 10.0f,
								 4.0f);
	entity->addComponent( ComponentType::LookAtEntity, component );
	// default tag is follow
	entity->addTag(ComponentType::TAG_LookAtFollowMode, new LookAtFollowMode_TAG() );
	entity->addComponent(ComponentType::PlayerCameraController, new PlayerCameraController() );
	component = new AudioListener();
	entity->addComponent(ComponentType::AudioListener, component);
	
	m_world->addEntity(entity);
	*/
}

void ClientApplication::initSounds()
{

	EntitySystem*	tempSys			= NULL;
	Entity*			entity			= NULL;
	Component*		component		= NULL;
	int				soundIdx		= -1;
	string			fullFilePath;
	string			file;

	tempSys = m_world->getSystem(SystemType::AudioBackendSystem);
	AudioBackendSystem* audioBackend = static_cast<AudioBackendSystem*>(tempSys);

	/************************************************************************/
	/* Load positional sound												*/
	/************************************************************************/
//	file = "spaceship_laser.wav";
//	BasicSoundCreationInfo basicSoundInfo = BasicSoundCreationInfo(file.c_str(),
//		TESTSOUNDEFFECTPATH.c_str(),true);
//	PositionalSoundCreationInfo positionalSoundInfo = PositionalSoundCreationInfo(
//		AglVector3( 0, 0, 0 ));
//	soundIdx = audioBackend->createPositionalSound(&basicSoundInfo,&positionalSoundInfo);
//	entity = m_world->createEntity();
//	component = new Transform( 0, 0, 0 );
//	entity->addComponent( ComponentType::Transform, component );
//	component = new AudioInfo(soundIdx,true);
//	entity->addComponent(ComponentType::AudioInfo, component);
//	m_world->addEntity(entity);
//	audioBackend->changeAudioInstruction(soundIdx, SoundEnums::Instructions::PLAY);

	/************************************************************************/
	/* Load positional sound												*/
	/************************************************************************/
//	for(int i=0; i<1; i++)
//	{
//		file = "MusicMono.wav";
//		BasicSoundCreationInfo basicSoundInfo = BasicSoundCreationInfo(file.c_str(), TESTMUSICPATH.c_str(),true);
//		PositionalSoundCreationInfo positionalSoundInfo = PositionalSoundCreationInfo( AglVector3(3.0f,3.0f,(float)i*2.0f) );
//		soundIdx = audioBackend->createPositionalSound(&basicSoundInfo,&positionalSoundInfo);
//		entity = m_world->createEntity();
//		component = new Transform( 3.0f, 3.0f, (float)i*2.0f );
//		entity->addComponent( ComponentType::Transform, component );
//		component = new AudioInfo(soundIdx,true);
//		entity->addComponent(ComponentType::AudioInfo, component);
//		m_world->addEntity(entity);
//		audioBackend->changeAudioInstruction(soundIdx, SoundEnums::Instructions::PLAY);
//	}
	

//	/************************************************************************/
//	/* Load ambient sound													*/
//	/************************************************************************/
//	file = "Techno_1.wav";
//	basicSoundInfo = BasicSoundCreationInfo(file.c_str(),TESTMUSICPATH.c_str(), true);
//	soundIdx = audioBackend->createAmbientSound( &basicSoundInfo );
//	entity = m_world->createEntity();
//	component = new AudioInfo(soundIdx,false);
//	entity->addComponent(ComponentType::AudioInfo,component);
//	m_world->addEntity(entity);
//	
//	/************************************************************************/
//	/* Load ambient sound													*/
//	/************************************************************************/
//	file = "Spaceship_Weapon_-_Fighter Blaster or Laser-Shot-Mid.wav";
//	basicSoundInfo = BasicSoundCreationInfo(file.c_str(),TESTSOUNDEFFECTPATH.c_str());
//	soundIdx = audioBackend->createAmbientSound( &basicSoundInfo );
//	entity = m_world->createEntity();
//	component = new AudioInfo(soundIdx,false);
//	entity->addComponent(ComponentType::AudioInfo,component);
//	m_world->addEntity(entity);
//
//	/************************************************************************/
//	/* Load ambient sound													*/
//	/************************************************************************/
//	file = "Spaceship_Engine_Idle_-_Spacecraft_hovering.wav";
//	basicSoundInfo = BasicSoundCreationInfo(file.c_str(),TESTSOUNDEFFECTPATH.c_str(),true);
//	soundIdx = audioBackend->createAmbientSound( &basicSoundInfo );
//	entity = m_world->createEntity();
//	component = new AudioInfo(soundIdx,false);
//	entity->addComponent(ComponentType::AudioInfo, component);
//	m_world->addEntity(entity);
//	audioBackend->changeAudioInstruction(soundIdx,SoundEnums::Instructions::PLAY);
}

void ClientApplication::InitModulesTestByAnton()
{
	Entity* entity;
	Component* component;

	EntitySystem* tempSys = NULL;

	// Load cube model used as graphic representation for all "graphical" entities.
	tempSys = m_world->getSystem(SystemType::GraphicsBackendSystem);
	GraphicsBackendSystem* graphicsBackend = static_cast<GraphicsBackendSystem*>(tempSys);
	int cubeMeshId = graphicsBackend->loadSingleMeshFromFile( "P_cube" );
//	int shipMeshId = graphicsBackend->loadSingleMeshFromFile( "Ship.agl", &MODELPATH );
//	int walkerMeshId = graphicsBackend->createMesh( "MeshWalker.agl", &TESTMODELPATH );

	// Create a box that the spaceship can pickup
	entity = m_world->createEntity();
	component = new RenderInfo( cubeMeshId );
	entity->addComponent( ComponentType::RenderInfo, component );
	component = new Transform(10, 0, 0);
	entity->addComponent( ComponentType::Transform, component );

	entity->addComponent( ComponentType::PhysicsBody, 
		new PhysicsBody() );

	entity->addComponent( ComponentType::BodyInitData, 
		new BodyInitData(AglVector3(10, 0, 0),
		AglQuaternion::identity(),
		AglVector3(1, 1, 1), AglVector3(0, 0, 0), 
		AglVector3(0, 0, 0), 0, 
		BodyInitData::DYNAMIC, 
		BodyInitData::SINGLE, false));

	entity->addComponent(ComponentType::ShipModule, new ShipModule());
	entity->addComponent(ComponentType::MinigunModule, new MinigunModule(AglVector3(0, 0, 0), AglVector3(0, 0, 1)));
	m_world->addEntity(entity);

	entity = m_world->createEntity();
	component = new RenderInfo( cubeMeshId );
	entity->addComponent( ComponentType::RenderInfo, component );
	component = new Transform(20, 0, 0);
	entity->addComponent( ComponentType::Transform, component );

	entity->addComponent( ComponentType::PhysicsBody, 
		new PhysicsBody() );

	entity->addComponent( ComponentType::BodyInitData, 
		new BodyInitData(AglVector3(20, 0, 0),
		AglQuaternion::identity(),
		AglVector3(1, 1, 1), AglVector3(0, 0, 0), 
		AglVector3(0, 0, 0), 0, 
		BodyInitData::DYNAMIC, 
		BodyInitData::SINGLE, false));

	entity->addComponent(ComponentType::ShipModule, new ShipModule());
	entity->addComponent(ComponentType::ShieldModule, new ShieldModule());
	m_world->addEntity(entity);

	entity = m_world->createEntity();
	component = new RenderInfo( cubeMeshId );
	entity->addComponent( ComponentType::RenderInfo, component );
	component = new Transform(30, 0, 0);
	entity->addComponent( ComponentType::Transform, component );

	entity->addComponent( ComponentType::PhysicsBody, 
		new PhysicsBody() );

	entity->addComponent( ComponentType::BodyInitData, 
		new BodyInitData(AglVector3(30, 0, 0),
		AglQuaternion::identity(),
		AglVector3(1, 1, 1), AglVector3(0, 0, 0), 
		AglVector3(0, 0, 0), 0, 
		BodyInitData::DYNAMIC, 
		BodyInitData::SINGLE, false));

	entity->addComponent(ComponentType::ShipModule, new ShipModule());
	entity->addComponent(ComponentType::RocketLauncherModule, new RocketLauncherModule(AglVector3(0, 0, 0), AglVector3(0, 0, 1)));

	m_world->addEntity(entity);

	entity = m_world->createEntity();
	component = new RenderInfo( cubeMeshId );
	entity->addComponent( ComponentType::RenderInfo, component );
	component = new Transform(40, 0, 0);
	entity->addComponent( ComponentType::Transform, component );

	entity->addComponent( ComponentType::PhysicsBody, 
		new PhysicsBody() );

	entity->addComponent( ComponentType::BodyInitData, 
		new BodyInitData(AglVector3(40, 0, 0),
		AglQuaternion::identity(),
		AglVector3(1, 1, 1), AglVector3(0, 0, 0), 
		AglVector3(0, 0, 0), 0, 
		BodyInitData::DYNAMIC, 
		BodyInitData::SINGLE, false));

	entity->addComponent(ComponentType::MineLayerModule, new MineLayerModule());
	entity->addComponent(ComponentType::ShipModule, new ShipModule());

	m_world->addEntity(entity);

	entity = m_world->createEntity();
	component = new RenderInfo( cubeMeshId );
	entity->addComponent( ComponentType::RenderInfo, component );
	component = new Transform(50, 0, 0);
	entity->addComponent( ComponentType::Transform, component );

	entity->addComponent( ComponentType::PhysicsBody, 
		new PhysicsBody() );

	entity->addComponent( ComponentType::BodyInitData, 
		new BodyInitData(AglVector3(50, 0, 0),
		AglQuaternion::identity(),
		AglVector3(1, 1, 1), AglVector3(0, 0, 0), 
		AglVector3(0, 0, 0), 0, 
		BodyInitData::DYNAMIC, 
		BodyInitData::SINGLE, false));

	entity->addComponent(ComponentType::ShipModule, new ShipModule());
	entity->addComponent(ComponentType::SpeedBoosterModule, new SpeedBoosterModule());

	m_world->addEntity(entity);




	entity = m_world->createEntity();
	component = new RenderInfo( cubeMeshId );
	entity->addComponent( ComponentType::RenderInfo, component );
	component = new Transform(50, 0, -10);
	entity->addComponent( ComponentType::Transform, component );

	entity->addComponent( ComponentType::PhysicsBody, 
		new PhysicsBody() );

	entity->addComponent( ComponentType::BodyInitData, 
		new BodyInitData(AglVector3(50, 0, -10),
		AglQuaternion::identity(),
		AglVector3(1, 1, 1), AglVector3(0, 0, 0), 
		AglVector3(0, 0, 0), 0, 
		BodyInitData::DYNAMIC, 
		BodyInitData::SINGLE, true, true));

	m_world->addEntity(entity);

	entity = m_world->createEntity();
	component = new RenderInfo( cubeMeshId );
	entity->addComponent( ComponentType::RenderInfo, component );
	component = new Transform(40, 0, -10);
	entity->addComponent( ComponentType::Transform, component );

	entity->addComponent( ComponentType::PhysicsBody, 
		new PhysicsBody() );

	entity->addComponent( ComponentType::BodyInitData, 
		new BodyInitData(AglVector3(40, 0, -10),
		AglQuaternion::identity(),
		AglVector3(1, 1, 1), AglVector3(0, 0, 0), 
		AglVector3(0, 0, 0), 0, 
		BodyInitData::DYNAMIC, 
		BodyInitData::SINGLE, false));

	entity->addComponent(ComponentType::ShipModule, new ShipModule());


	ConnectionPointSet* cpset = new ConnectionPointSet();
	AglMatrix target1 = AglMatrix::createTranslationMatrix(AglVector3(1, 2, 0));
	AglMatrix target2 = AglMatrix::createTranslationMatrix(AglVector3(-1, 2, 0));
	cpset->m_connectionPoints.push_back(ConnectionPoint(target1));
	cpset->m_connectionPoints.push_back(ConnectionPoint(target2));
	entity->addComponent(ComponentType::ConnectionPointSet, cpset);

	m_world->addEntity(entity);


	entity = m_world->createEntity();
	component = new RenderInfo( cubeMeshId );
	entity->addComponent( ComponentType::RenderInfo, component );
	component = new Transform(30, 0, -10);
	entity->addComponent( ComponentType::Transform, component );

	entity->addComponent( ComponentType::PhysicsBody, 
		new PhysicsBody() );

	entity->addComponent( ComponentType::BodyInitData, 
		new BodyInitData(AglVector3(30, 0, -10),
		AglQuaternion::identity(),
		AglVector3(1, 1, 1), AglVector3(0, 0, 0), 
		AglVector3(0, 0, 0), 0, 
		BodyInitData::DYNAMIC, 
		BodyInitData::SINGLE, false));

	entity->addComponent(ComponentType::ShipModule, new ShipModule());

	cpset = new ConnectionPointSet();
	cpset->m_connectionPoints.push_back(ConnectionPoint(target1));
	cpset->m_connectionPoints.push_back(ConnectionPoint(target2));
	entity->addComponent(ComponentType::ConnectionPointSet, cpset);
	m_world->addEntity(entity); 

	//Ray entity
	/*entity = m_world->createEntity();
	component = new RenderInfo( cubeMeshId );
	entity->addComponent( ComponentType::RenderInfo, component );


	Transform* t = new Transform(AglVector3(0, 0, 0), AglQuaternion::rotateToFrom(AglVector3(0, 0, 1), AglVector3(0, 1, 0)), AglVector3(0.1f, 0.1f, 10));
	entity->addComponent( ComponentType::Transform, t);
	m_world->addEntity(entity);*/
}
