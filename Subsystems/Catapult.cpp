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
	m_state              = CATAPULT_STATE_NOT_READY;
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
	m_state              = CATAPULT_STATE_NOT_READY;
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
	m_state              = CATAPULT_STATE_NOT_READY;
	m_step->Start();
}

Catapult::catapult_state
Catapult::GetState()
{
	return m_state;
}

void
Catapult::Fire()
{
	switch(m_state) {
	case CATAPULT_STATE_READY:
#ifdef DEBUG_CATAPULT
			printf("Catapult: Firing")
#endif
		m_latch->Open();
		m_state = CATAPULT_STATE_FIRING;
	default:
		break;
	}
}

void
Catapult::PrepareFire()
{
	switch(m_state) {
	case CATAPULT_STATE_NOT_READY:
#ifdef DEBUG_CATAPULT
		printf("Catapult: Prepare fire");
#endif
		m_shift->Close();
		m_latch->Open();
		m_state = CATAPULT_STATE_PULLING_BACK;
		break;
	case CATAPULT_STATE_PULLING_BACK:
		m_winch->Set(-0.7);
		if (m_stop->Get() == 1) {
			m_state = CATAPULT_STATE_LATCHING;
		}
		break;
	case CATAPULT_STATE_LATCHING:
#ifdef DEBUG_CATAPULT
			printf("Catapult: Latching")
#endif
		m_winch->Set(0.0);
		m_encoderStart = m_step->Get();
		m_state = CATAPULT_STATE_BACKDRIVING;
		break;
	case CATAPULT_STATE_BACKDRIVING:
		m_winch->Set(0.5);
		if(abs(m_step->Get() - m_encoderStart) > CATAPULT_BACK_OFF_AMOUNT) {
			m_winch->Set(0.0);
			m_shift->Open();
			m_state = CATAPULT_STATE_READY;
		}
		break;
	default:
		break;
	}
}

void
Catapult::UnprepareFire()
{
	switch(m_state) {
	case CATAPULT_STATE_READY:
		m_encoderStart = m_step->Get();
		m_shift->Close();
		m_state = CATAPULT_STATE_ERELEASE;
		break;
	case CATAPULT_STATE_ERELEASE:
		// Drive forward to ensure the shifter catches
		m_winch->Set(0.3);
		if(abs(m_step->Get() - m_encoderStart) > CATAPULT_FORWARD_CATCH_AMOUNT) {
#ifdef DEBUG_CATAPULT
			printf("Catapult: Emergency Release")
#endif
			m_winch->Set(0.0);
			m_latch->Open();
			m_state = CATAPULT_STATE_READY;
		}
		break;
	default:
		break;
	}
}


