#include "WPILib.h"
#include "Catapult.h"

Catapult::Catapult(Talon *winch, Encoder *step,
	Solenoid *shiftLow, Solenoid *shiftHigh, 
	Solenoid *latchPinEngaged, Solenoid *latchPinDisengaged,
	DigitalInput *stop)
{
	m_winch              = winch;
	m_step               = step;
	m_shift              = new Valve(shiftLow, shiftHigh, false);
	m_latch              = new Valve(latchPinEngaged, latchPinDisengaged, true);
	m_stop               = stop;
	m_latchEngage       = false;
	m_stopActive         = false;
	m_needFree           = false;
	m_backOffAmt         = 100;
	override             = false;
	m_step->Start();
}

Catapult::Catapult(Talon &winch, Encoder &step,
			Solenoid &shiftLow, Solenoid &shiftHigh,
			Solenoid &latchPinEngaged, Solenoid &latchPinDisengaged,
			DigitalInput &stop)
{
	m_winch              = &winch;
	m_step               = &step;
	m_shift              = new Valve(shiftLow, shiftHigh, false);
	m_latch              = new Valve(latchPinEngaged, latchPinDisengaged, true);
	m_stop               = &stop;
	m_latchEngage       = false;
	m_stopActive         = false;
	m_needFree           = false;
	m_backOffAmt         = 100;
	override             = false;
	m_step->Start();
}

Catapult::Catapult(UINT32 winch, UINT32 stepA, UINT32 stepB, 
			UINT32 shiftLow, UINT32 shiftHigh,
			UINT32 latchPinEngaged, UINT32 latchPinDisengaged,
			UINT32 stop)
{
	m_winch              = new Talon(winch);
	m_step               = new Encoder(stepA, stepB, false);
	m_shift              = new Valve(shiftLow, shiftHigh, false);
	m_latch              = new Valve(latchPinEngaged, latchPinDisengaged, true);
	m_stop               = new DigitalInput(stop);
	m_latchEngage        = false;
	m_stopActive         = false;
	m_needFree           = true;
	m_backOffAmt         = 100;
	override             = false;
	m_step->Start();
}

bool
Catapult::isLatched()
{
	return m_latch->isClosed();
}

bool
Catapult::isAtStop()
{
	return (m_stop->Get() == 1 || override);
}

bool
Catapult::lockedAndloaded()
{
	if(isAtStop() &&  
			// requires it be at stop and latched for a certain amount of time
	   isLatched() &&
	   m_shift->isOpen())
	{
		m_winch->Set(0.0);
		return true;
	}
	return false;
}

void
Catapult::Fire()
{
	if(lockedAndloaded())
		m_latch->Open();
	return;
}

void
Catapult::prepareFire()
{
	static UINT32 cnt=0;
	
	if(!isAtStop() && !isLatched()) {
		m_shift->Close();
		m_winch->Set(-0.7);
	}
	else if(!isLatched()) {
		m_winch->Set(0.0);
		m_latch->Close();
	}
	else if(!lockedAndloaded()){
		if(cnt == 0)
			cnt = m_step->Get();
#ifdef DEBUG_CATAPULT
		printf("cnt: %d\ncurrent encoder: %d\nabsolute value of the difference of these: %d", cnt, m_step->Get(), abs(m_step->Get() - cnt));
#endif
		m_winch->Set(0.5);
		if(abs(m_step->Get() - cnt) > m_backOffAmt) {
#ifdef DEBUG_CATAPULT
			printf("Just for kicks\n");
#endif
			m_winch->Set(0.0);
			m_shift->Open();
			cnt = 0;
		}
	}
	/*else
	{
		m_winch->Set(0.0);
	}*/
}

void
Catapult::unprepareFire()
{
	static unsigned int cnt = 0;
	if(m_shift->isOpen() || m_latch->isOpen())
	{
		cnt = 0;
		m_shift->Close();
	}
	if(cnt > m_backOffAmt)
	{
		m_latch->Open();
	}
	else
	{
		cnt++;
	}
}


