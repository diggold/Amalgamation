#include "libRocketBackendSystem.h"

LibRocketBackendSystem::LibRocketBackendSystem( GraphicsBackendSystem* p_graphicsBackend
											   , InputBackendSystem* p_inputBackend )
	: EntitySystem( SystemType::LibRocketBackendSystem )
{
	m_graphicsBackend = p_graphicsBackend;
	m_inputBackend = p_inputBackend;
}


LibRocketBackendSystem::~LibRocketBackendSystem()
{
	for( unsigned int i=0; i<m_documents.size(); i++ )
	{
		m_documents[i]->GetContext()->UnloadDocument(m_documents[i]);
	}

	m_rocketContext->RemoveReference(); //release context

	delete m_renderInterface;
	delete m_systemInterface;
}

void LibRocketBackendSystem::initialize()
{
	m_renderInterface = new LibRocketRenderInterface( m_graphicsBackend->getGfxWrapper() );
	m_systemInterface = new LibRocketSystemInterface( m_world );

	Rocket::Core::SetSystemInterface(m_systemInterface);
	Rocket::Core::SetRenderInterface(m_renderInterface);
	Rocket::Core::Initialise();

	int wndWidth = m_graphicsBackend->getGfxWrapper()->getWindowWidth();
	int wndHeight = m_graphicsBackend->getGfxWrapper()->getWindowdHeight();

	m_rocketContextName = "default_name"; // Change the name if using multiple contexts!

	m_rocketContext = Rocket::Core::CreateContext(
		Rocket::Core::String( m_rocketContextName.c_str() ),
		Rocket::Core::Vector2i( wndWidth, wndHeight) );

	// Load fonts and documents
	// TODO: Should be done by assemblage

	vector<string> fonts;
	fonts.push_back( "Delicious-Roman.otf" );
	fonts.push_back( "Delicious-Bold.otf" );
	fonts.push_back( "Delicious-Italic.otf" );
	fonts.push_back( "Delicious-Roman.otf" );
	for( int i=0; i<fonts.size(); i++ )
	{
		string tmp = ROCKET_FONT_PATH + fonts[i]; 
		loadFontFace( tmp.c_str() );
	}

	string tmp;
	tmp = ROCKET_HUD_PATH + "demo.rml";
	loadDocument( tmp.c_str() );

	//tmp = ROCKET_HUD_PATH + "main.rml";
	//loadDocument( tmp.c_str() );

	//tmp = ROCKET_HUD_PATH + "index.rml";
	//loadDocument( tmp.c_str() );

	//tmp = ROCKET_HUD_PATH + "window.rml";
	//loadDocument( tmp.c_str() );

	//Hard-coded controls
	m_cursor.leftBtn	= m_inputBackend->getControlByEnum( InputHelper::MOUSE_BTN::M_LBTN );
	m_cursor.xNegative	= m_inputBackend->getControlByEnum( InputHelper::MOUSE_AXIS::X_NEGATIVE );
	m_cursor.xPositive	= m_inputBackend->getControlByEnum( InputHelper::MOUSE_AXIS::X_POSITIVE );
	m_cursor.yNegative	= m_inputBackend->getControlByEnum( InputHelper::MOUSE_AXIS::Y_NEGATIVE );
	m_cursor.yPositive	= m_inputBackend->getControlByEnum( InputHelper::MOUSE_AXIS::Y_POSITIVE );

	AntTweakBarWrapper* dbgGui = AntTweakBarWrapper::getInstance();
	dbgGui->addReadOnlyVariable( "Cursor X", TwType::TW_TYPE_DOUBLE, &m_cursor.x, "" );
	dbgGui->addReadOnlyVariable( "Cursor Y", TwType::TW_TYPE_DOUBLE, &m_cursor.y, "" );

	m_cursor.init();

	// HACK: tmp crazy lulz0r cursor.
	string cursorPath = ROCKET_CURSOR_PATH + "cursor.rml";
	if( m_rocketContext->LoadMouseCursor(cursorPath.c_str()) == NULL )
	{
		int breakHere = 0;
	}
}

bool LibRocketBackendSystem::loadFontFace( const char* p_fontPath )
{
	return Rocket::Core::FontDatabase::LoadFontFace( Rocket::Core::String(p_fontPath) );
}

int LibRocketBackendSystem::loadDocument( const char* p_filePath )
{
	int id = -1;
	Rocket::Core::ElementDocument* tmpDoc = NULL;
	tmpDoc = m_rocketContext->LoadDocument( Rocket::Core::String(p_filePath) );
	
	if( tmpDoc != NULL )
	{
		id = m_documents.size();
		m_documents.push_back( tmpDoc );
		tmpDoc->Show(); //HACK! Remove this when doing this from the outside
		tmpDoc->RemoveReference();
	}

	return id;
}

void LibRocketBackendSystem::process()
{
	m_cursor.update();
	m_rocketContext->ProcessMouseMove(m_cursor.x, m_cursor.y, 0);

	if( m_cursor.leftBtn->getRawData() == InputHelper::KEY_STATE::KEY_PRESSED )
	{
		m_rocketContext->ProcessMouseButtonDown( 0, 0 );
	}
	else if( m_cursor.leftBtn->getRawData() == InputHelper::KEY_STATE::KEY_RELEASED )
	{
		m_rocketContext->ProcessMouseButtonUp( 0, 0 );
	}

	//Rocket::Core::Element* test = m_documents[0]->GetElementById("btn");
	//if (test != NULL)
	//	test->SetProperty("width", "800px");

	m_rocketContext->Update();
}

void LibRocketBackendSystem::render()
{
	m_rocketContext->Render();
}
