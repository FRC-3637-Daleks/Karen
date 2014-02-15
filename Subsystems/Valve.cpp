#include "WPILib.h"
#include "Valve.h"

Valve::Valve(uint32_t aChannel, uint32_t bChannel, bool open)
{
	m_a = new Solenoid(aChannel);
	m_b = new Solenoid(bChannel);
	m_needFree = true;
	if(open) {
		m_a->Set(true);
		m_b->Set(false);
	}
	else {
		m_a->Set(false);
		m_b->Set(true);
	}
	return;
}

Valve::Valve(uint8_t aModuleNumber, uint32_t aChannel, 
		uint8_t bModuleNumber, uint32_t bChannel, bool open)
{
	m_a = new Solenoid(aModuleNumber, aChannel);
	m_b = new Solenoid(bModuleNumber, bChannel);
	m_needFree = true;
	if(open) {
		m_a->Set(true);
		m_b->Set(false);
	}
	else {
		m_a->Set(false);
		m_b->Set(true);
	}
	return;
}

Valve::Valve(Solenoid *aSource, Solenoid *bSource, bool open)
{
	m_a = aSource;
	m_b = bSource;
	m_needFree = false;
	if(open) {
		m_a->Set(true);
		m_b->Set(false);
	}
	else {
		m_a->Set(false);
		m_b->Set(true);
	}
	return;
}

Valve::Valve(Solenoid &aSource, Solenoid &bSource, bool open)
{
	m_a = &aSource;
	m_b = &bSource;
	m_needFree = false;
	if(open) {
		m_a->Set(true);
		m_b->Set(false);
	}
	else {
		m_a->Set(false);
		m_b->Set(true);
	}
	return;
}

bool
Valve::isValid()
{
	bool aState, bState;
	
	aState = m_a->Get();
	bState = m_b->Get();
	if((aState && bState) || !(aState || bState))
		return false;
	return true;
}

bool
Valve::isOpen()
{
	if(isValid()) {
		if(m_a->Get())
			return true;
	}
	return false;
}

bool
Valve::isClosed()
{
	if(isValid()) {
		if(m_b->Get())
			return true;
	}
	return false;
}

void
Valve::Open()
{
	if(isOpen())
		return;
	// open one side, and close the other
	m_a->Set(true);
	m_b->Set(false);
	return;
}

void
Valve::Close()
{
	if(isClosed())
		return;
	// close one side, and open the other
	m_a->Set(false);
	m_b->Set(true);
	return;
}
