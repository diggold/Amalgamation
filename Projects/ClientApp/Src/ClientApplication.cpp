#include "ClientApplication.h"


ClientApplication::ClientApplication( HINSTANCE p_hInstance )
{
	m_running = false;

	m_hInstance = p_hInstance;
	
	m_client = new TcpClient();

	m_world = new EntityWorld();
	initSystems();
	initEntities();
}

ClientApplication::~ClientApplication()
{
	delete m_world;
	delete m_client;
}

void ClientApplication::run()
{
	m_running = true;

	// simple timer
	__int64 cntsPerSec = 0;
	__int64 currTimeStamp = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	double secsPerCnt = 1.0f / (float)cntsPerSec;

	double dt = 0.0f;
	__int64 m_prevTimeStamp = 0;

	MSG msg = {0};
	while(WM_QUIT != msg.message)
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			currTimeStamp = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
			dt = (currTimeStamp - m_prevTimeStamp) * secsPerCnt;
			dt = 1/100.0;
			m_prevTimeStamp = currTimeStamp;

			m_world->setDelta(dt);
			m_world->process();
		}
	}

}

void ClientApplication::initSystems()
{
	InputSystem* inSys = new InputSystem();
	m_world->setSystem( SystemType::InputSystem, inSys, true);

	GraphicsBackendSystem* graphicsBackend = new GraphicsBackendSystem( m_hInstance );
	m_world->setSystem( SystemType::SystemTypeIdx::GraphicsBackendSystem, graphicsBackend , true );

	CameraSystem* camSys = new CameraSystem( graphicsBackend );
	m_world->setSystem( SystemType::SystemTypeIdx::CameraSystem, camSys , true );

	RenderPrepSystem* rpSys = new RenderPrepSystem( graphicsBackend );
	m_world->setSystem( SystemType::RenderPrepSystem, rpSys , true );

	NetworkConnectToServerSystem* connectSystem =
		new NetworkConnectToServerSystem( m_client );
	m_world->setSystem( SystemType::NetworkConnectoToServerSystem, connectSystem,
		false );

	m_world->initialize();
}

void ClientApplication::initEntities()
{
	Entity* e;
	Component* c;

	e = m_world->createEntity();
	c = new RenderInfo();
	e->addComponent( ComponentType::RenderInfo, c );
	c = new Transform();
	e->addComponent( ComponentType::Transform, c );
	m_world->addEntity(e);

	e = m_world->createEntity();
	c = new CameraInfo(800/(float)600);
	e->addComponent( ComponentType::CameraInfo, c );
	c = new Input();
	e->addComponent( ComponentType::Input, c );
	m_world->addEntity(e);
}
