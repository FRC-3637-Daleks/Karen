#ifndef _PICKUP_H_
#define _PICKUP_H_

#include "Defines.h"
#include "WPILib.h"
#include "Valve.h"


class Pickup
{
public:
	enum pickup_state {
		PICKUP_STATE_DOWN,
		PICKUP_STATE_MIDDLE,
		PICKUP_STATE_UP,
	};

private:
	Valve *m_direction;
	Solenoid *m_stop;
	Solenoid *m_move;
	Talon *m_roller;
	DigitalInput *m_middleLeftReed;
	DigitalInput *m_middleRightReed;
	Task *m_reedWatch;

	bool allocated;
	bool m_targetMiddle;
	pickup_state m_state;

public:
	Pickup(Valve * const direction, Solenoid* const stop, Solenoid * const move, Talon * const roller, 
			DigitalInput *midLeft, DigitalInput *midRight);

	Pickup(Valve& direction, Solenoid &stop, Solenoid &move, Talon& roller, 
			DigitalInput& midLeft, DigitalInput& midRight);

	Pickup(const UINT8 top, const UINT8 bottom, const UINT8 stop_a, const UINT8 stop_b, const UINT8 roller,
			const UINT8 midLeft, const UINT8 midRight);

	void Up();
	void Down();
	void Lock();
	void Unlock();
	bool CenterArms();

	void CheckArms(); // Checks the reed, and locks it when needed
	const pickup_state GetState() const {return m_state;};	// returns the current location of the piston

public:
	void SetRoller(const float vel) {if(m_roller) m_roller->Set(vel);};

public:
	~Pickup();
};

void WatchReeds(Pickup *armsObj);  // Main Thread function which watches the arm position

#endif /*_PICKUP_H_*/
