#ifndef __CATAPULT_H__
#define __CATAPULT_H__
#include "Defines.h"
#include "WPILib.h"
#include "Valve.h"

class Talon;
class Encoder;
class Solenoid;
class DigitalInput;
class Valve;

class Catapult {
public:
	Catapult(Talon *winch, Encoder *step, Solenoid *shiftLow,
			Solenoid *shiftHigh, Solenoid *latchPinEngaged,
			Solenoid *latchPinDisengaged, DigitalInput *stop);
	Catapult(Talon &winch, Encoder &step, Solenoid &shiftLow,
			Solenoid &shiftHigh, Solenoid &latchPinEngaged,
			Solenoid &latchPinDisengaged, DigitalInput &stop);
	Catapult(UINT32 winch, UINT32 stepA, UINT32 stepB, UINT32 shiftLow,
			UINT32 shiftHight, UINT32 latchPinEngaged,
			UINT32 latchPinDisengaged, UINT32 stop);
	
	enum catapult_state {
		CATAPULT_STATE_NOT_READY, // Catapult in up position, shifter closed, default
		CATAPULT_STATE_PULLING_BACK, // Steps to fire
		CATAPULT_STATE_LATCHING,
		CATAPULT_STATE_BACKDRIVING,
		CATAPULT_STATE_READY, // Catapult in down position, shifter open
		CATAPULT_STATE_FIRING,
		CATAPULT_STATE_ERELEASE // Emergency release
	};

	catapult_state GetState();
	void Fire();
	void PrepareFire();
	void UnprepareFire();
	void CheckPosition();

private:
	catapult_state m_state;

	Talon *m_winch;
	Encoder *m_step;
	Valve *m_shift;
	Valve *m_latch;

	DigitalInput *m_stop;

	bool isLatched();
	UINT32 m_backOffAmt;
	int m_encoderStart;
};
#endif
