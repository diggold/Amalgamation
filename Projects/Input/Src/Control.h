#pragma once

class InputManager;

// =======================================================================================
//                                      Control
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief	General class for all types of input (Keyboard, Mouse, GamePad). 
///        
/// The term pressness notates how hard stick or trigger has been pushed and how much a 
/// mouse has been moved. This term is used throughout the input documentation.
///
/// Both analog (mouse, thumb sticks) and digital inputs (keys, buttons) use this class.
///
/// The Control class contains three types of data. Status, status delta and raw data. 
/// Status tells us the current status of the control (button/key/stick etc) at the moment
/// of the last update() call. The status delta is the difference between the current 
/// status and the status at the previous update() call. The raw data is the raw input 
/// that the control uses to calculate status and delta. This can be a state (down, up,
/// pressed, released) or pressness of a stick, mouse or trigger.
///
/// A digital control will always cause the status to be 0 or 1. In an analog control 
/// the status spans between 0 and 1.0 depending on the amount of "pressness". The status 
/// delta is the difference of the status between the latest update and the one before
/// that. The delta can therefore also be negative.
/// 
/// The delta can be used to see if a key has been pressed or released the last frame (for 
/// example). The delta will be 1.0 or -1.0 in those cases. Some examples:   
///
/// (current status - old status) = delta gives:    
/// (down - up) = (1.0 - 0.0) = 1.0 = pressed.    
/// (up - down) = (0.0 - 1.0) = -1.0 = released.    
/// (down - down) = (1.0 - 1.0) = 0.0 = no change.    
/// (up - up) = (0.0 - 0.0) = 0.0 = no change.    
///
/// Handles XBOX360 controllers and mouse and keyboard through the xInput API and the
/// message loop. When using the static message dispatch function in MessageLoopFetcher
/// must be called from the message loop. Otherwise no input from the keyboard and mouse
/// will be detected.
/// Created on: 4-12-2012 
///\author Mattias Liljeson
///---------------------------------------------------------------------------------------

class Control
{
public:
	Control();
	virtual ~Control();

	///@brief Override and update "pressness" member  variables.
	virtual void update(InputManager* p_manager) = 0;

	///@brief
	///@return The current status of the control. Between 0.0 and 1.0
	virtual double getStatus();

	///@brief
	///@return The delta of status from last update and the update before. 
	virtual double getDelta();

	///\brief get raw data from input device. In the devices native format
	///\return the raw data
	virtual int getRawData();

protected:
	// Current amount of "Pressness" on the control 
	double m_status;

	// "Pressness" delta between latest update and the update before.
	double m_statusDelta;

	// Latest raw data fetched from input device.
	int m_rawData;
};
