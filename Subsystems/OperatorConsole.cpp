#include "OperatorConsole.h"

const float OperatorConsole::GetX() const
{
	if(drive == TANK_DRIVE)
		return precisionFactor * 
				((m_rightStick->GetX() < 0? -1:1)*pow(deadzone(m_rightStick->GetX()), squared? 2:1) + 
				(m_leftStick->GetX() < 0? -1:1)*pow(deadzone(m_leftStick->GetX()), squared? 2:1))/2;
	else {
		float x = deadzone(m_rightStick->GetX());
		if (squared) {
			x *= fabs(x);
		}
		x *= precisionFactor;
		return x;
	}
}

const float OperatorConsole::GetY() const
{
	if(drive == TANK_DRIVE)
		return precisionFactor * 
				((m_rightStick->GetY() < 0? -1:1) * pow(deadzone(m_rightStick->GetY()), squared? 2:1) + 
				(m_leftStick->GetY() < 0? -1:1) * pow(deadzone(m_leftStick->GetY()), squared? 2:1))/2;
	else {
		float y = deadzone(m_rightStick->GetY());
		if (squared) {
			y *= fabs(y);
		}
		y *= precisionFactor;
		return y;
	}
}

const float OperatorConsole::GetTheta() const
{
	if(drive == TANK_DRIVE)
		return precisionFactor * 
				((m_rightStick->GetY() < 0? -1:1) * pow(deadzone(m_rightStick->GetY()), squared? 2:1) -
				(m_leftStick->GetY() < 0? -1:1) * pow(deadzone(m_leftStick->GetY()), squared? 2:1))/2;
	else
		return (m_leftStick->GetX() < 0 && squared? -1:1) * precisionFactor * (pow(deadzone(m_leftStick->GetX()), squared? 2:1));
}

const float OperatorConsole::GetRight() const
{
	return  (m_rightStick->GetY() < 0 && squared? -1:1) * precisionFactor * (pow(deadzone(m_rightStick->GetY()), squared? 2:1));
}

const float OperatorConsole::GetLeft() const
{
	return (m_leftStick->GetY() < 0 && squared? -1:1) * precisionFactor * (pow(deadzone(m_leftStick->GetY()), squared? 2:1));
}

/* Catapult control functions */
const bool OperatorConsole::CatapultFire() const
{
	if(oneDriver)
		return (m_rightStick->GetRawButton(1) && m_leftStick->GetRawButton(1));  // PLEASE NOTE: FIRING IS NOW DONE WITH THIS CONFIGURATION 
	else
		return m_gamePad->GetButton(GamePad::BOTTOM_LEFT_SHOULDER) && m_gamePad->GetButton(GamePad::BOTTOM_LEFT_SHOULDER);
}

const bool OperatorConsole::CatapultPrepareFire() const
{
	if(oneDriver)
		return m_leftStick->GetRawButton(5) || m_rightStick->GetRawButton(4);
	else
		return (m_gamePad->GetButton(GamePad::B1));
}

const bool OperatorConsole::CatapultEmergencyRelease() const
{
	if(oneDriver)
		return m_leftStick->GetRawButton(7) || m_rightStick->GetRawButton(10);
	else
		return (m_gamePad->GetButton(GamePad::B2));
}

/* Roller controls */
const float OperatorConsole::GetRoller() const
{
	if(oneDriver)
		if(m_leftStick->GetRawButton(2))
			return 1.0;
		else if(m_leftStick->GetRawButton(3))
			return -1.0;
		else
			return 0.0;
	else
		return m_gamePad->GetAxis(GamePad::PAD_Y);
}

const bool OperatorConsole::RollerUp() const
{
	if(oneDriver)
		return m_rightStick->GetRawButton(2);
	else
		return m_gamePad->GetButton(GamePad::TOP_RIGHT_SHOULDER);
}
const bool OperatorConsole::RollerDown() const
{
	if(oneDriver)
		return m_rightStick->GetRawButton(2);
	else
		return m_gamePad->GetButton(GamePad::BOTTOM_RIGHT_SHOULDER);
}
const bool OperatorConsole::RollerCenter() const
{
	return m_gamePad->GetButton(GamePad::B4);
}

const bool OperatorConsole::GetOverride()
{
	static bool pressed = false;
	if(m_gamePad->GetButton(GamePad::START) && !pressed)
	{
		pressed = true;
		manual = !manual;
		printf("Manual Override: %d", manual);
	}
	else if(!m_gamePad->GetButton(GamePad::START))
	{
		pressed = false;
	}
	
	return manual;
}



OperatorConsole::~OperatorConsole()
{
	if(allocated)
	{
		if(m_leftStick) delete m_leftStick;
		if(m_rightStick) delete m_rightStick;
		if(m_gamePad) delete m_gamePad;
	}
}
















