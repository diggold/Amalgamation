#pragma once

#include <EntitySystem.h>
#include "ShipInputProcessingSystem.h"
#include "TcpClient.h"

class Control;
class InputBackendSystem;


// =======================================================================================
//                              ShipSystemsInputHelper
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	Input help structs and methods for the ship, collected in one place
///        
/// # ShipSystemsInputHelper
/// Detailed description.....
/// Created on: 9-1-2013 
///---------------------------------------------------------------------------------------

class ShipInputProcessingSystem : public EntitySystem
{
public:
	ShipInputProcessingSystem(InputBackendSystem* p_inputBackend, TcpClient* p_client);
	virtual ~ShipInputProcessingSystem() {}

	virtual void initialize();
	virtual void process();

	struct RawInputForces
	{
		double	hPositive,hNegative,
			vPositive,vNegative,
			shPositive,shNegative,
			svPositive,svNegative,
			rRight, rLeft,
			thrust;
		bool stateSwitchTrig;

		float getHorizontalInput()
		{
			return float(hPositive - hNegative);
		}
		float getVerticalInput()
		{
			return (float)(vPositive - vNegative);
		}
		float getRollInput()
		{
			return (float)(rLeft - rRight);
		}
		float getThrust()
		{
			return (float)(thrust);
		}
		float getStrafeHorizontalInput()
		{
			return (float)(shPositive - shNegative);
		}
		float getStrafeVerticalInput()
		{
			return (float)(svPositive - svNegative);
		}
		bool getStateModeSwitch()
		{
			return stateSwitchTrig;
		}
	};

	struct ResultingInputForces
	{
		// Store raw float data
		float horizontalInput,
			verticalInput, 
			rollInput,
			thrustInput,
			strafeHorizontalInput,
			strafeVerticalInput;
		bool stateSwitchInput;

		ResultingInputForces() {}

		ResultingInputForces(RawInputForces p_rawInput)
		{
			horizontalInput = p_rawInput.getHorizontalInput();
			verticalInput	= p_rawInput.getVerticalInput();
			rollInput	= p_rawInput.getRollInput();
			thrustInput = p_rawInput.getThrust();
			strafeHorizontalInput	= p_rawInput.getStrafeHorizontalInput();
			strafeVerticalInput		= p_rawInput.getStrafeVerticalInput();
			stateSwitchInput = p_rawInput.getStateModeSwitch();
		}
	};

	ResultingInputForces& getProcessedInput();

private:
	//Added by Anton 15/1 - 13
	void sendModuleSlotHighlight(int p_slot);
	void sendSlotActivation();
	void sendSlotDeactivation();

private:

	float* getControllerEpsilonPointer();
	RawInputForces readAllInput();
	void updateAntTweakBar(const ResultingInputForces& p_input);

	ResultingInputForces m_processedInput;

	Control* m_gamepadHorizontalPositive;
	Control* m_gamepadHorizontalNegative;
	Control* m_gamepadVerticalPositive;
	Control* m_gamepadVerticalNegative;

	Control* m_mouseHorizontalPositive;
	Control* m_mouseHorizontalNegative;
	Control* m_mouseVerticalPositive;
	Control* m_mouseVerticalNegative;

	Control* m_gamepadRollRight;
	Control* m_gamepadRollLeft;
	Control* m_gamepadThrust;
	Control* m_gamepadStrafeHorizontalPositive;
	Control* m_gamepadStrafeHorizontalNegative;
	Control* m_gamepadStrafeVerticalNegative;
	Control* m_gamepadStrafeVerticalPositive;

	Control* m_gamepadEditModeTrig;

	Control* m_keyboardRollRight;
	Control* m_keyboardRollLeft;
	Control* m_keyboardThrust;
	Control* m_keyboarStrafeHorizontalPos;
	Control* m_keyboarStrafeHorizontalNeg;
	Control* m_keyboardStrafeVerticalPos;
	Control* m_keyboardStrafeVerticalNeg;

	Control* m_keyboardEditModeTrig;

	// The values of analogue sticks as a vector3 (used in anttweakbar).

	// Threshold value for the Gamepad's analogue stick error.
	float m_controllerEpsilon;

	float m_angleInputMultiplier;

	// Correction vectors for the left and right thumb sticks.
	//double m_leftStickCorrection[2];
	//double m_rightStickCorrection[2];

	//Added by Anton 15/1 - 13
	Control* m_keyboardModuleSlots[4];
	Control* m_mouseModuleActivation;

	void initGamePad();
	void initMouse();
	void initKeyboard();

	bool m_editSwitchTrigReleased;
	InputBackendSystem* m_inputBackend;
	TcpClient* m_client;
};