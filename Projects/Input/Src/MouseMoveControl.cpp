#include "MouseMoveControl.h"
#include "InputManager.h"
#include "MessageLoopFetcher.h"


MouseMoveControl::MouseMoveControl( InputHelper::MouseAxes p_axis)
	: Control( InputHelper::InputDeviceTypes_MOUSE_AXIS )
{
	m_axis = p_axis;

	//NOTE: Multiple fallthroughs!
	switch( p_axis )
	{
	case InputHelper::MouseAxes_X_POSITIVE:
	case InputHelper::MouseAxes_Y_POSITIVE:
	case InputHelper::MouseAxes_Z_POSITIVE:
		m_subAxis = InputHelper::SubAxes_POSITIVE;
		break;

	case InputHelper::MouseAxes_X_NEGATIVE:
	case InputHelper::MouseAxes_Y_NEGATIVE:
	case InputHelper::MouseAxes_Z_NEGATIVE:
		m_subAxis = InputHelper::SubAxes_NEGATIVE;
		break;
	}
}


MouseMoveControl::~MouseMoveControl()
{
}

void MouseMoveControl::update( float p_dt, InputManager* p_manager )
{
	IMouseKeyboardFetcher* fetcher = p_manager->getMouseKeyboardFetcher();
	double mouseSensitivity = fetcher->getMouseSensitivity();
	double travel = (double)fetcher->getMouseTravel( m_axis );// * mouseSensitivity) / p_dt;
	travel *= mouseSensitivity;
	travel /= p_dt;

	double newStatus = 0.0;
	if( (m_subAxis == InputHelper:: SubAxes_POSITIVE && travel > 0) ||
		(m_subAxis == InputHelper:: SubAxes_NEGATIVE && travel < 0) )
	{
		travel = abs( travel );
		newStatus = (double)travel / SHRT_MAX;
	}
	
	m_statusDelta = newStatus - m_status;
	m_status = newStatus;
	m_rawData = fetcher->getMouseTravel( m_axis );;
}

int MouseMoveControl::getControlEnum()
{
	return m_axis;
}
