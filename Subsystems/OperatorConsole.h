#ifndef _OPERATOR_CONSOLE_H_
#define _OPERATOR_CONSOLE_H_

#include <cmath>
#include "Defines.h"
#include "Gamepad.h"
#include "Pickup.h"
#include "Catapult.h"
#include "WPILib.h"

#define DEADZONE 0.05

class OperatorConsole
{
public:
	typedef enum {ARCADE_DRIVE, TANK_DRIVE} Drive_t;
private:
	Drive_t drive;
	Joystick *m_leftStick, *m_rightStick;
	GamePad *m_gamePad;
	bool allocated;
	bool oneDriver;
	
private:
	bool squared;
	float precisionFactor;
	bool engaged;
	bool manual;
	
public:
	OperatorConsole(const Drive_t driveConfig, Joystick *left, Joystick *right, GamePad *pad, const bool driver): 
		drive(driveConfig), m_leftStick(left), m_rightStick(right), m_gamePad(pad), allocated(false), oneDriver(driver),
		squared(true), precisionFactor(1.0), engaged(false), manual(false) {};
	
	OperatorConsole(const Drive_t driveConfig, Joystick &left, Joystick &right, GamePad &pad, const bool driver):
		drive(driveConfig), m_leftStick(&left), m_rightStick(&right), m_gamePad(&pad), allocated(false), oneDriver(driver),
		squared(true), precisionFactor(1.0), engaged(false), manual(false) {};
	
	OperatorConsole(const Drive_t driveConfig, const UINT8 left, const UINT8 right, const UINT8 pad, const bool driver):
		drive(driveConfig), m_leftStick(new Joystick(left)), m_rightStick(new Joystick(right)), m_gamePad(new GamePad(pad)), allocated(true), oneDriver(driver),
		squared(true), precisionFactor(1.0), engaged(false), manual(false) {};
	
public:
	const float GetX() const;
	const float GetY() const;
	const float GetTheta() const;
	const float GetLeft() const;
	const float GetRight() const;
	
	const bool CatapultFire() const;
	const bool CatapultPrepareFire() const;
	const bool CatapultEmergencyRelease() const;
	
	const float GetRoller() const;		// returns roller velocity input
	const bool RollerUp() const;
	const bool RollerDown() const;
	const bool RollerCenter() const;

public:
	void SetSquared(const bool b) {squared = b;};
	const bool GetSquared() const {return squared;};
	
	void SetPrecision(const float f) {precisionFactor = ceiling(f);};
	const float GetPrecision() {return precisionFactor;};
	
	void SetDrive(const Drive_t d) {drive = d;};
	const int GetDrive() const  {return drive;};
	
	const bool GetOverride();
	const bool ManualEngage() {return m_gamePad->GetButton(GamePad::B4);};
	const bool GetTestContinue() const {return m_gamePad->GetButton(GamePad::B3);};
	
private:
	static const float deadzone(const float f) {return fabs(f) < DEADZONE? 0.0:f;};
	static const float ceiling(const float f) {return f > 1.0? 1.0:(f<-1.0? -1.0:f);};
	
public:
	virtual ~OperatorConsole();
};

#endif
