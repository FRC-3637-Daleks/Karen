#ifndef _OPERATOR_CONSOLE_H_
#define _OPERATOR_CONSOLE_H_

#include <cmath>
#include "Gamepad.h"
#include "WPILib.h"

#define DEADZONE 0.05

enum {PICKUP_DOWN, PICKUP_MIDDLE, PICKUP_UP};  // temporary until there is a Pickup header file

class OperatorConsole
{
public:
	typedef enum {ARCADE_DRIVE, TANK_DRIVE} Drive_t;
private:
	Drive_t drive;
	Joystick *m_leftStick, *m_rightStick;
	GamePad *m_gamePad;
	bool allocated;
	
private:
	bool squared;
	float precisionFactor;
	bool engaged;
	int rollerPos;
	
public:
	OperatorConsole(const Drive_t driveConfig, Joystick *left, Joystick *right, GamePad *pad): 
		drive(driveConfig), m_leftStick(left), m_rightStick(right), m_gamePad(pad), allocated(false), 
		squared(true), precisionFactor(1.0), engaged(false), rollerPos() {};
	OperatorConsole(const Drive_t driveConfig, Joystick &left, Joystick &right, GamePad &pad) 
		{OperatorConsole(driveConfig, &left, &right, &pad);};
	OperatorConsole(const Drive_t driveConfig, const UINT8 left, const UINT8 right, const UINT8 pad) 
		{OperatorConsole(driveConfig, new Joystick(left), new Joystick(right), new GamePad(pad));};
	
public:
	const float GetX() const;
	const float GetY() const;
	const float GetTheta() const;
	const float GetLeft() const;
	const float GetRight() const;
	
	const bool Disengage() const;	// returns true if the firing button conditions are true
	const bool Engage() const;		// returns true if the winching precondition are true
	
	const float GetRoller() const;		// returns roller velocity input
	const int GetRollerPosition();// returns roller
	
public:
	void SetSquared(const bool b) {squared = b;};
	const bool GetSquared() const {return squared;};
	
	void SetPrecision(const float f) {precisionFactor = ceiling(f);};
	const float GetPrecision() {return precisionFactor;};
	
	const int GetDrive() const  {return drive;};
	
private:
	static const float deadzone(const float f) {return fabs(f) < DEADZONE? 0.0:f;};
	static const float ceiling(const float f) {return f > 1.0? 1.0:(f<-1.0? -1.0:f);};
	
public:
	virtual ~OperatorConsole();
};





#endif
