#pragma once

#include <vector>
#include "InputHelper.h"

class Control;
struct ControlSet;

using namespace std;

// =======================================================================================
//                                      Cursor
// =======================================================================================

///---------------------------------------------------------------------------------------
/// \brief Class that represents a cursor in NDC-space. Can have several sets of controls
/// which controls it simultaneously.
///        
/// # Cursor
/// Detailed description.....
/// Created on: 4-1-2013 
/// \author Mattias Liljeson
///---------------------------------------------------------------------------------------

class Cursor
{
public:

	///-----------------------------------------------------------------------------------
	/// Constructor which inits all members to zero
	/// \return 
	///-----------------------------------------------------------------------------------
	Cursor();

	///-----------------------------------------------------------------------------------
	/// Empty deconstructor
	/// \return 
	///-----------------------------------------------------------------------------------
	~Cursor();

	///-----------------------------------------------------------------------------------
	/// Add a set of controls which will affect the cursor. Controls must be deleted 
	/// elsewhere. This class does not take ownership of pointers.
	/// \param p_xSensitivity Sensitivty multiplier for the x-axis
	/// \param p_ySensitivity Sensitivty multiplier for the x-axis
	/// \param p_left Control which indicates cursor movement.
	/// \param p_right Control which indicates cursor movement.
	/// \param p_down Control which indicates cursor movement.
	/// \param p_up Control which indicates cursor movement.
	/// \param p_primaryBtn Control which indicates status of the primary button
	/// \param p_secondaryBtn Control which indicates status of the secondary button
	/// \return void
	///-----------------------------------------------------------------------------------
	void addControlSet( double p_xSensitivity, double p_ySensitivity,
		Control* p_left, Control* p_right, Control* p_down, Control* p_up,
		Control* p_primaryBtn, Control* p_secondaryBtn );


	///-----------------------------------------------------------------------------------
	/// Add a set of controls which will affect the cursor. Controls must be deleted 
	/// elsewhere. This class does not take ownership of pointers.
	/// \param p_controlSet Struct containing all controls and variables for one set of 
	/// controls.
	/// \return void
	///-----------------------------------------------------------------------------------
	void addControlSet( ControlSet p_controlSet );

	///-----------------------------------------------------------------------------------
	/// Update size of screen, cursor needs to now this in order to properly
	/// handle sensitivity
	/// \param p_width
	/// \param p_height
	/// \return void
	///-----------------------------------------------------------------------------------
	void setScreenSize(int p_width, int p_height);

	///-----------------------------------------------------------------------------------
	/// Must be called each frame to update the cursor position and state
	/// \return void
	///-----------------------------------------------------------------------------------
	void update(float p_dt);

	///-----------------------------------------------------------------------------------
	/// Getter for the x position
	/// \return double The x position of the cursor position in NDC
	///-----------------------------------------------------------------------------------
	double getX();
	
	///-----------------------------------------------------------------------------------
	/// Getter for the y position
	/// \return double The y position of the cursor position in NDC
	///-----------------------------------------------------------------------------------
	double getY();


	int getCurrentScreenWidth() {return m_screenWidth;}
	int getCurrentScreenHeight() {return m_screenHeight;}

	void reset();
	
	void overridePosition(float p_ndcX, float p_ndcY);

	///-----------------------------------------------------------------------------------
	/// Get the primary buttons state
	/// \return InputHelper::KeyStates
	///-----------------------------------------------------------------------------------
	InputHelper::KeyStates getPrimaryState();

	///-----------------------------------------------------------------------------------
	/// Get the primary buttons status
	/// \return double  0.0 or 1.0
	///-----------------------------------------------------------------------------------
	double getPrimaryStatus();

	///-----------------------------------------------------------------------------------
	/// Get the primary buttons delta (difference between this frame and the last)
	/// \return double  -1.0, 0.0 or 1.0
	///------------------------------------------------------------------------------------
	double getPrimaryDelta();

	///-----------------------------------------------------------------------------------
	/// Get the secondary buttons state
	/// \return InputHelper::KeyStates
	///-----------------------------------------------------------------------------------
	InputHelper::KeyStates getSecondaryState();

	///-----------------------------------------------------------------------------------
	/// Get the secondary buttons status
	/// \return double  0.0 or 1.0
	///-----------------------------------------------------------------------------------
	double getSecondaryStatus();

	///-----------------------------------------------------------------------------------
	/// Get the secondary buttons delta (difference between this frame and the last)
	/// \return double  -1.0, 0.0 or 1.0
	///------------------------------------------------------------------------------------
	double getSecondaryDelta();

	bool isVisible() {return m_visible;}
	void hide() {m_visible=false;}
	void show() {m_visible=true;}

	int m_screenWidth;
	int m_screenHeight;
private:
	double m_x; // double used internally in input
	double m_y; // double used internally in input
	bool m_visible;

	vector<ControlSet> m_controlSets;
};

