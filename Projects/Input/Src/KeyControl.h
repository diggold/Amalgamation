#pragma once

#include "Control.h"
#include "InputHelper.h"
#include "InputManager.h"
#include "MessageLoopFetcher.h"

class InputHelper;
class InputManager;
class MessageLoopFetcher;

// =======================================================================================
//                                      KeyControl
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief Implementation of Control used for keyboard keys
///        
/// Status will be set to 1.0 when the key is down and 0.0 when up. The delta will 
/// therefore be 1.0 on the frames where the key is pressed and -1.0 on the frames where 
/// the key is released. The raw data will be one of InputHelper::KEY_STATE.
/// Created on: 4-12-2012 
///\author Mattias Liljeson
///---------------------------------------------------------------------------------------

class KeyControl : public Control
{
public:
	KeyControl( InputHelper::KEYBOARD_KEY p_key );
	virtual ~KeyControl(void);

	virtual void update( InputManager* p_manager );

private:
	int m_key;
};
