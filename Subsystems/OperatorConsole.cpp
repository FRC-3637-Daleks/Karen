#include "OperatorConsole.h"

const float OperatorConsole::GetX() const
{
	if(drive == TANK_DRIVE)
		return precisionFactor * 
				((m_rightStick->GetX() < 0? -1:1)*pow(deadzone(m_rightStick->GetX()), squared? 2:1) + 
				(m_leftStick->GetX() < 0? -1:1)*pow(deadzone(m_leftStick->GetX()), squared? 2:1))/2;
	else
		return (m_rightStick->GetX() < 0? -1:1) * precisionFactor * (pow(deadzone(m_rightStick->GetX()), squared? 2:1));
}

const float OperatorConsole::GetY() const
{
	if(drive == TANK_DRIVE)
		return precisionFactor * 
				((m_rightStick->GetY() < 0? -1:1) * pow(deadzone(m_rightStick->GetY()), squared? 2:1) + 
				(m_leftStick->GetY() < 0? -1:1) * pow(deadzone(m_leftStick->GetY()), squared? 2:1))/2;
	else
		return (m_rightStick->GetY() < 0? -1:1) * precisionFactor * (pow(deadzone(m_rightStick->GetY()), squared? 2:1));
}

const float OperatorConsole::GetTheta() const
{
	if(drive == TANK_DRIVE)
		return precisionFactor * 
				((m_rightStick->GetY() < 0? -1:1) * pow(deadzone(m_rightStick->GetY()), squared? 2:1) -
				(m_leftStick->GetY() < 0? -1:1) * pow(deadzone(m_leftStick->GetY()), squared? 2:1))/2;
	else
		return (m_leftStick->GetX() < 0? -1:1) * precisionFactor * (pow(deadzone(m_leftStick->GetX()), squared? 2:1));
}

const float OperatorConsole::GetRight() const
{
	return  (m_rightStick->GetY() < 0? -1:1) * precisionFactor * (pow(deadzone(m_rightStick->GetY()), squared? 2:1));
}

const float OperatorConsole::GetLeft() const
{
	return (m_leftStick->GetY() < 0? -1:1) * precisionFactor * (pow(deadzone(m_leftStick->GetY()), squared? 2:1));
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
		return ++rollerPos > Pickup::PICKUP_UP? rollerPos=Pickup::PICKUP_UP:rollerPos;
	}
	
	return rollerPos; 
}

const int OperatorConsole::GetRollerDirection()
{
	return m_gamePad->GetButton(GamePad::BOTTOM_RIGHT_SHOULDER)? -1:(m_gamePad->GetButton(GamePad::TOP_RIGHT_SHOULDER)? 1:0) ;
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
















