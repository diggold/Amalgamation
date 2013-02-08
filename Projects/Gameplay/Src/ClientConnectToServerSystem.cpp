#include "ClientConnectToServerSystem.h"
#include "Control.h"
#include <TcpClient.h>
#include <Rocket/Core/Event.h>
#include "LibRocketEventManagerSystem.h"

ClientConnectToServerSystem* ClientConnectToServerSystem::m_selfPointer = NULL;

ClientConnectToServerSystem::ClientConnectToServerSystem( TcpClient* p_tcpClient )
	: EntitySystem( SystemType::ClientConnectoToServerSystem ), EventHandler("join")
{
	m_tcpClient = p_tcpClient;
	
	m_isLookingForConnection = false;
	m_connectionAddress.octets1 = 127;
	m_connectionAddress.octets2 = 0;
	m_connectionAddress.octets3 = 0;
	m_connectionAddress.octets4 = 1;
	m_connectionAddress.port =	1337;

	m_selfPointer = this;
}

ClientConnectToServerSystem::~ClientConnectToServerSystem()
{
}

void ClientConnectToServerSystem::processEntities( const vector<Entity*>& p_entities )
{
	if( m_tcpClient->hasActiveConnection() )
	{
		m_isLookingForConnection = false;
		m_world->getSystem(SystemType::ClientPacketHandlerSystem)->setEnabled(true);
		setEnabled(false);

		auto eventManagerSys = static_cast<LibRocketEventManagerSystem*>(
			m_world->getSystem(SystemType::LibRocketEventManagerSystem));

		// Clears and hides all currently visible documents.
		eventManagerSys->clearDocumentStack();
	}
}

void ClientConnectToServerSystem::initialize()
{

	TwStructMember ipMembers[] = {
		{ "Ip-part1", TW_TYPE_INT32, offsetof(NetworkAdress,octets1),"min=0 max=255"},
		{ "Ip-part2", TW_TYPE_INT32, offsetof(NetworkAdress,octets2),"min=0 max=255"},
		{ "Ip-part3", TW_TYPE_INT32, offsetof(NetworkAdress,octets3),"min=0 max=255"},
		{ "Ip-part4", TW_TYPE_INT32, offsetof(NetworkAdress,octets4),"min=0 max=255"},
		{ "Port",	  TW_TYPE_INT32, offsetof(NetworkAdress,port),"min=0"	},
	};

	TwType myStruct;

	myStruct = TwDefineStruct("IP-address", ipMembers,5,sizeof(NetworkAdress),NULL,NULL);
	AntTweakBarWrapper::getInstance()->addWriteVariable(AntTweakBarWrapper::NETWORK,
		"Connection address",myStruct,&m_connectionAddress,"");

	TwAddButton(AntTweakBarWrapper::getInstance()->getAntBar(AntTweakBarWrapper::NETWORK),
		"Connect to address", callbackConnectToNetworkAddress, NULL, "key=F4");
}

void TW_CALL ClientConnectToServerSystem::callbackConnectToNetworkAddress( void* p_clientData )
{
	if( m_selfPointer->getEnabled())
		m_selfPointer->connectToNetworkAddress();
}

void ClientConnectToServerSystem::connectToNetworkAddress()
{
	string address = m_connectionAddress.getIpAddress();
	address = m_connectionAddress.getPortAddress();

	m_tcpClient->connectToServerAsync( m_connectionAddress.getIpAddress(), 
		m_connectionAddress.getPortAddress() );
	m_isLookingForConnection = true;
}

void ClientConnectToServerSystem::connectToNetworkAddress(
														const std::string& p_serverAddress, 
														const std::string& p_serverPort)
{
	m_tcpClient->connectToServerAsync( p_serverAddress, p_serverPort);
	m_isLookingForConnection = true;
}

void ClientConnectToServerSystem::processEvent( Rocket::Core::Event& p_event, const Rocket::Core::String& p_value )
{
	// Sent from the 'onsubmit' of the play screen, we set the network ip and port here,
	// and enable the system.
	if (p_value == "join_server")
	{
		// "server_host" is the name attribute specified in the input element in the rml file.
		// "localhost" simply is provided as a default value, if the host isn't set. This could be left as "" as well.
		string server_host = p_event.GetParameter<Rocket::Core::String>("server_host", "localhost").CString();
		string server_port = p_event.GetParameter<Rocket::Core::String>("server_port", "1337").CString();

		if( !m_tcpClient->hasActiveConnection() &&  !m_isLookingForConnection)
			connectToNetworkAddress(server_host, server_port);
	}
}