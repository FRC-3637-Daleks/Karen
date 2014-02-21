#ifndef _GAMEPAD_H_
#define _GAMEPAD_H_

#include "Joystick.h"
#include "Defines.h"

class GamePad: public Joystick
// front end for translating Joystick interface to make sense for the Logitech Gamepad
{
public:
	// Axis labels for the two joysticks on the gamepad
	/*
	 * TODO order these correctly
	 */
	typedef enum
	{
		LEFT_X=1, LEFT_Y, RIGHT_X, RIGHT_Y, PAD_X, PAD_Y
	} PadAxisType;
	
	
	// Constants for various named buttons such as the shoulder buttons
	/*
	 * TODO: order these correctly
	 */	
	typedef enum
	{
		B1=1, B2, B3, B4,
		TOP_LEFT_SHOULDER, TOP_RIGHT_SHOULDER, 
		BOTTOM_LEFT_SHOULDER, BOTTOM_RIGHT_SHOULDER, 
		SELECT, START,
		LEFT_JOY_BUTTON, RIGHT_JOY_BUTTON, 
		N_GAMEPAD_BUTTONS
	} PadButtonType;
	
	GamePad(UINT32 port): Joystick(port) {};
	
	float GetAxis(PadAxisType axis) {return Joystick::GetRawAxis(axis);};
	bool GetButton(PadButtonType button) {return Joystick::GetRawButton(button);};
	
private:
//	DISSALLOW_COPY_AND_ASSIGN(GamePad);
//  Won't compile with this, saying DISSALLOW_COPY_AND_ASSIGN requires a type... must it be declared?
	float GetX(JoystickHand hand = kRightHand) {return 0.0;};
	float GetY(JoystickHand hand = kRightHand) {return 0.0;};
	float GetZ() {return 0.0;};
	float GetTwist() {return 0.0;};
	float GetThrottle() {return 0.0;};
	bool GetTrigger(JoystickHand hand = kRightHand) {return false;};
	bool GetTop(JoystickHand hand = kRightHand) {return false;};
	bool GetBumper(JoystickHand hand = kRightHand) {return false;};
	float GetMagnitude() {return false;};
	float GetDirectionRadians() {return false;};
	float GetDirectionDegrees() {return false;};
};



#endif /*_GAMEPAD_H_*/
