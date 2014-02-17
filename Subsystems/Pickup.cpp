#include "Pickup.h"

void Pickup::Up()
{
	/*
	if(!m_top && !m_bottom)
		return;
	m_top->Set(false);
	m_bottom->Set(true);
	*/
	m_stop->Open();
	m_direction->Open();
}

void Pickup::Down()
{
	/*
	if(!m_top && !m_bottom)
		return;
	m_top->Set(true);
	m_bottom->Set(false);
	*/
	m_stop->Open();
	m_direction->Close();
}

void Pickup::Stop()
{
	/*
	if(!m_top && !m_bottom)
		return;
	m_top->Set(true);
	m_bottom->Set(true);
	*/
	m_stop->Close();
	
}

const bool Pickup::SetPos(const int pos)
{
	targetPos = pos > PICKUP_UP? PICKUP_UP:(pos < PICKUP_DOWN? PICKUP_DOWN:pos);
	
	if(!m_middleLeft && !m_middleRight)
		return targetPos == location;
	
	switch(targetPos)
	{
	case PICKUP_DOWN:
		Down();
		if(m_middleLeft->Get() || m_middleRight->Get())
		{
			location = PICKUP_MIDDLE;
		}
		else if(location == PICKUP_MIDDLE)
		{
			location = PICKUP_DOWN;
		}
		else
		{
			//location = PICKUP_UP;
		}
		break;
	case PICKUP_UP:
		Up();
		if(m_middleLeft->Get() || m_middleRight->Get())
		{
			location = PICKUP_MIDDLE;
		}
		else if(location == PICKUP_MIDDLE)
		{
			location = PICKUP_UP;
		}
		else
		{
			//location = PICKUP_DOWN;
		}
		break;
	case PICKUP_MIDDLE:
		if(m_middleLeft->Get() || m_middleRight->Get())
		{
			location = PICKUP_MIDDLE;
			Stop();
		}
		else if(location == PICKUP_UP)
		{
			Down();
		}
		else if(location == PICKUP_DOWN)
		{
			Up();
		}
		break;
	}
	
	return location == targetPos;
}

Pickup::~Pickup()
{
	if(!allocated)
		return;
	
	if(m_roller)
		delete m_roller;
	
	if(m_middleLeft)
		delete m_middleLeft;
	
	if(m_middleRight)
		delete m_middleRight;
	
	if(m_stop)
		delete m_stop;
	
	if(m_direction)
		delete m_direction;		
}
