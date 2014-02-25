#ifndef _PICKUP_H_
#define _PICKUP_H_

#include "Defines.h"
#include "WPILib.h"
#include "Valve.h"


class Pickup
{
public:
	enum {PICKUP_DOWN, PICKUP_MIDDLE, PICKUP_UP};
	
private:
	Valve *m_direction;
	Valve *m_stop;
	Talon *m_roller;
	DigitalInput *m_middleLeft;
	DigitalInput *m_middleRight;
	Task *m_reedWatch;
	
	bool allocated;
	int targetPos;
	int location;
	
public:
	Pickup(Valve * const direction, Valve * const stop, Talon * const roller, 
			DigitalInput *midLeft, DigitalInput *midRight, const int start);
	
	Pickup(Valve& direction, Valve& stop, Talon& roller, 
			DigitalInput& midLeft, DigitalInput& midRight, const int start);
	
	Pickup(const UINT8 top, const UINT8 bottom, const UINT8 stop_a, const UINT8 stop_b, const UINT8 roller,
			const UINT8 midLeft, const UINT8 midRight, const int start);
	
private:
	void Up();
	void Down();
	
public:
	void Stop();
	
	const bool SetPos(const int pos); // returns true if it's at the spot
	void CheckArms(); // runs code  previously in set pos which is tracking the piston position
	const int GetTarget() const {return targetPos;};   // returns the current target position
	const int GetLocation() const {return location;};	// returns the current location of the piston
	
public:
	void SetRoller(const float vel) {if(m_roller) m_roller->Set(vel);};
	
public:
	~Pickup();
};

void WatchReeds(Pickup *armsObj);  // Main Thread function which watches the arm position


#endif /*_PICKUP_H_*/
