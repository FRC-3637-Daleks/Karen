#include "OperatorConsole.h"

const float OperatorConsole::GetX() const
{
	if(drive == TANK_DRIVE)
		return precisionFactor * (pow(deadzone(m_rightStick->GetX()), squared? 2:1) + pow(deadzone(m_leftStick->GetX()), squared? 2:1))/2;
	else
		return precisionFactor * (pow(deadzone(m_rightStick->GetX()), squared? 2:1));
}

const float OperatorConsole::GetY() const
{
	if(drive == TANK_DRIVE)
		return precisionFactor * (pow(deadzone(m_rightStick->GetY()), squared? 2:1) + pow(deadzone(m_leftStick->GetY()), squared? 2:1))/2;
	else
		return precisionFactor * (pow(deadzone(m_rightStick->GetY()), squared? 2:1));
}

const float OperatorConsole::GetTheta() const
{
	if(drive == TANK_DRIVE)
		return precisionFactor * (pow(deadzone(m_rightStick->GetY()), squared? 2:1) - pow(deadzone(m_leftStick->GetY()), squared? 2:1))/2;
	else
		return precisionFactor * (pow(deadzone(m_leftStick->GetX()), squared? 2:1));
}

const float OperatorConsole::GetLeft() const
{
	return  precisionFactor * (pow(deadzone(m_rightStick->GetY()), squared? 2:1));
}

const float OperatorConsole::GetRight() const
{
	return precisionFactor * (pow(deadzone(m_leftStick->GetY()), squared? 2:1));
}

const bool OperatorConsole::Disengage() const
{
	return (m_rightStick->GetRawButton(1) || m_leftStick->GetRawButton(1)) &&	// At least one trigger and
			(m_rightStick->GetRawButton(2) || m_leftStick->GetRawButton(2));	// one top button need to be pressed
}

const bool OperatorConsole::Engage() const
{
	return (m_gamePad->GetButton(GamePad::B1));
}

const float OperatorConsole::GetRoller() const
{
	return m_gamePad->GetAxis(GamePad::PAD_Y);
}

const int OperatorConsole::GetRollerPosition()
{
	static bool Shoulder1 = false, Shoulder2 = false;
	if(m_gamePad->GetButton(GamePad::BOTTOM_RIGHT_SHOULDER))
	{
		Shoulder1 = true;
	}
	else if(Shoulder1)
	{
		Shoulder1 = false;
		Shoulder2 = false;
		return --rollerPos < Pickup::PICKUP_DOWN? rollerPos=0:rollerPos;
	}
	else if(m_gamePad->GetButton(GamePad::TOP_RIGHT_SHOULDER))
	{
		Shoulder2 = true;
	}
	else if(Shoulder2)
	{
		Shoulder2 = false;
		Shoulder1 = false;
		return ++rollerPos > Pickup::PICKUP_UP? rollerPos=0:rollerPos;
	}
	
	return rollerPos; 
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















