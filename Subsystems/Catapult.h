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

class Catapult
{
public:
	Catapult(Talon *winch, Encoder *step,
			Solenoid *shiftLow, Solenoid *shiftHigh, 
			Solenoid *latchPinEngaged, Solenoid *latchPinDisengaged,
			DigitalInput *stop);
	Catapult(Talon &winch, Encoder &step,
			Solenoid &shiftLow, Solenoid &shiftHigh,
			Solenoid &latchPinEngaged, Solenoid &latchPinDisengaged,
			DigitalInput &stop);
	Catapult(UINT32 winch, UINT32 stepA, UINT32 stepB, 
			UINT32 shiftLow, UINT32 shiftHight,
			UINT32 latchPinEngaged, UINT32 latchPinDisengaged,
			UINT32 stop);
	
	bool isLatched();
	bool isAtStop();
	bool lockedAndloaded();
	void Fire();
	void prepareFire();
	void unprepareFire();
	void setOverride(const bool o) {override = o;};
	
private:
	Talon *m_winch;
	Encoder *m_step;
	Valve *m_shift;
	Valve *m_latch;

	DigitalInput *m_stop;
	
	bool m_latchEngage;
	bool m_stopActive;
	bool m_needFree;
	UINT32 m_backOffAmt;
	bool override;
};
#endif
