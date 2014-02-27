#include "Pickup.h"

Pickup::Pickup(Valve * const direction, Solenoid * const stop, Solenoid * const move, Talon * const roller, 
			DigitalInput *midLeft, DigitalInput *midRight, const int start): 
		m_direction(direction), m_stop(stop), m_move(move), m_roller(roller), 
		m_middleLeft(midLeft), m_middleRight(midRight), allocated(false), targetPos(start), location(start) 
{
	m_reedWatch = new Task("reedMonitor", (FUNCPTR)WatchReeds);
	m_reedWatch->Start((uint32_t)this);
}

Pickup::Pickup(Valve& direction, Solenoid& stop, Solenoid& move, Talon& roller, DigitalInput& midLeft, DigitalInput& midRight, const int start):
		m_direction(&direction), m_stop(&stop), m_move(&move), m_roller(&roller), 
		m_middleLeft(&midLeft), m_middleRight(&midRight), allocated(false), targetPos(start), location(start)
{
	m_reedWatch = new Task("reedMonitor", (FUNCPTR)WatchReeds);
	m_reedWatch->Start((uint32_t)this);
}

Pickup::Pickup(const UINT8 top, const UINT8 bottom, const UINT8 stop_a, const UINT8 stop_b, const UINT8 roller,
			const UINT8 midLeft, const UINT8 midRight, const int start):
		m_direction(new Valve(top, bottom)), m_stop(new Solenoid(stop_a)), m_move(new Solenoid(stop_b)), m_roller(new Talon(roller)), 
		m_middleLeft(new DigitalInput(midLeft)), m_middleRight(new DigitalInput(midRight)), allocated(true), 
		targetPos(start), location(start)
{
	m_reedWatch = new Task("reedMonitor", (FUNCPTR)WatchReeds);
	m_reedWatch->Start((uint32_t)this);
}
	

void Pickup::Up()
{
	/*
	if(!m_top && !m_bottom)
		return;
	m_top->Set(false);
	m_bottom->Set(true);
	*/
	m_stop->Set(false);
	m_move->Set(true);
	m_direction->Close();
}

void Pickup::Down()
{
	/*
	if(!m_top && !m_bottom)
		return;
	m_top->Set(true);
	m_bottom->Set(false);
	*/
	m_stop->Set(false);
	m_move->Set(true);
	m_direction->Open();
}

void Pickup::Stop()
{
	/*
	if(!m_top && !m_bottom)
		return;
	m_top->Set(true);
	m_bottom->Set(true);
	*/
	//if(m_stop->isOpen())
	m_stop->Set(true);
	m_move->Set(false);
	
}

const bool Pickup::SetPos(const int pos)
{
	targetPos = pos > PICKUP_UP? PICKUP_UP:(pos < PICKUP_DOWN? PICKUP_DOWN:pos);
	
	switch(targetPos)
	{
	case PICKUP_DOWN:
		Down();
		break;
	case PICKUP_UP:
		Up();
		break;
	case PICKUP_MIDDLE:
		if(location == PICKUP_MIDDLE)
		{
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


void Pickup::CheckArms()
{	
	if(!m_middleLeft || !m_middleRight)
		return;
	
	switch(targetPos)
	{
	case PICKUP_DOWN:
		if(!m_middleLeft->Get() || !m_middleRight->Get())
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
		if(!m_middleLeft->Get() || !m_middleRight->Get())
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
		if(!m_middleLeft->Get() || !m_middleRight->Get())
		{
			location = PICKUP_MIDDLE;
			Stop();
		}
		break;
	}
	
	return;
}


Pickup::~Pickup()
{
	if(m_reedWatch)
	{
		m_reedWatch->Stop();
		delete m_reedWatch;
	}
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
	if(m_move)
		delete m_move;
	if(m_direction)
		delete m_direction;
}

void WatchReeds(Pickup *armsObj)
{
	while(true)
	{
		armsObj->CheckArms();
	}
}
