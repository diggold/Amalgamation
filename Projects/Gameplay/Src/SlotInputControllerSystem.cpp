#include "SlotInputControllerSystem.h"
#include <Control.h>
#include "InputBackendSystem.h"
#include "HighlightSlotPacket.h"
#include "SimpleEventPacket.h"
#include <TcpClient.h>
#include "AudioBackendSystem.h"
#include <Globals.h>

SlotInputControllerSystem::SlotInputControllerSystem(InputBackendSystem* p_inputBackend,
										 TcpClient* p_client)
	: EntitySystem( SystemType::SlotInputController )
{
	m_inputBackend = p_inputBackend;
	m_client = p_client;
}

SlotInputControllerSystem::~SlotInputControllerSystem()
{
}

void SlotInputControllerSystem::process()
{
	for (unsigned int i = 0; i < 4; i++)
	{
		if (m_keyboardModuleSlots[i]->getDelta() > 0 ||
			m_gamepadModuleSlots[i]->getDelta() > 0)
		{
			//Highlight slot
			sendModuleSlotHighlight(i);
			AudioBackendSystem* audioBackend = static_cast<AudioBackendSystem*>(
				m_world->getSystem(SystemType::AudioBackendSystem));
			audioBackend->playSoundEffect(TESTSOUNDEFFECTPATH,
				"WARFARE M-16 RELOAD RELOAD FULL CLIP MAGAZINE 01.wav");
		}
	}
	
	if (m_mouseModuleActivation->getDelta() > 0 ||
		m_gamepadModuleActivation->getDelta()>0)
	{
		sendSlotActivation();
	}
	else if (m_mouseModuleActivation->getDelta() < 0 ||
			m_gamepadModuleActivation->getDelta() < 0)
	{
		sendSlotDeactivation();
	}
}

void SlotInputControllerSystem::initialize()
{
	initMouse();
	initKeyboard();
	initGamepad();
}

void SlotInputControllerSystem::sendModuleSlotHighlight(int p_slot)
{
	HighlightSlotPacket packet;
	packet.id = p_slot;

	m_client->sendPacket( packet.pack() );
}

void SlotInputControllerSystem::sendSlotActivation()
{
	SimpleEventPacket packet;
	packet.type = SimpleEventType::ACTIVATE_MODULE;

	m_client->sendPacket( packet.pack() );
}

void SlotInputControllerSystem::sendSlotDeactivation()
{
	SimpleEventPacket packet;
	packet.type = SimpleEventType::DEACTIVATE_MODULE;

	m_client->sendPacket( packet.pack() );
}

void SlotInputControllerSystem::initKeyboard()
{
	m_keyboardModuleSlots[0] = m_inputBackend->getControlByEnum(
		InputHelper::KeyboardKeys_1);
	m_keyboardModuleSlots[1] = m_inputBackend->getControlByEnum(
		InputHelper::KeyboardKeys_2);
	m_keyboardModuleSlots[2] = m_inputBackend->getControlByEnum(
		InputHelper::KeyboardKeys_3);
	m_keyboardModuleSlots[3] = m_inputBackend->getControlByEnum(
		InputHelper::KeyboardKeys_4);
}


void SlotInputControllerSystem::initGamepad()
{
	m_gamepadModuleSlots[0] = m_inputBackend->getControlByEnum(
		InputHelper::Xbox360Digitals_BTN_A);
	m_gamepadModuleSlots[1] = m_inputBackend->getControlByEnum(
		InputHelper::Xbox360Digitals_BTN_Y);
	m_gamepadModuleSlots[2] = m_inputBackend->getControlByEnum(
		InputHelper::Xbox360Digitals_BTN_X);
	m_gamepadModuleSlots[3] = m_inputBackend->getControlByEnum(
		InputHelper::Xbox360Digitals_BTN_B);

	m_gamepadModuleActivation = m_inputBackend->getControlByEnum(
		InputHelper::Xbox360Analogs_TRIGGER_L);
}


void SlotInputControllerSystem::initMouse()
{
	m_mouseModuleActivation = m_inputBackend->getControlByEnum(
		InputHelper::MouseButtons_0);
}