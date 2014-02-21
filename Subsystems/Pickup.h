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
	
	bool allocated;
	int targetPos;
	int location;
	
public:
	Pickup(Valve * const direction, Valve * const stop, Talon * const roller, 
			DigitalInput *midLeft, DigitalInput *midRight, const int start): 
		m_direction(direction), m_stop(stop), m_roller(roller), m_middleLeft(midLeft), m_middleRight(midRight), allocated(false), targetPos(start), location(start) {};
	
	Pickup(Valve& direction, Valve& stop, Talon& roller, 
			DigitalInput& midLeft, DigitalInput& midRight, const int start):
		m_direction(&direction), m_stop(&stop), m_roller(&roller), m_middleLeft(&midLeft), m_middleRight(&midRight), allocated(false), targetPos(start), location(start) {};
	
	Pickup(const UINT8 top, const UINT8 bottom, const UINT8 stop_a, const UINT8 stop_b, const UINT8 roller,
			const UINT8 midLeft, const UINT8 midRight, const int start):
		m_direction(new Valve(top, bottom)), m_stop(new Valve(stop_a, stop_b)), m_roller(new Talon(roller)), 
		m_middleLeft(new DigitalInput(midLeft)), m_middleRight(new DigitalInput(midRight)), allocated(true), targetPos(start), location(start) {};
	
public:
	void Up();
	void Down();
	void Stop();
	
	const bool SetPos(const int pos); // returns true if it's at the spot
	const int GetTarget() const {return targetPos;};   // returns the current target position
	const int GetLocation() const {return location;};	// returns the current location of the piston
	
public:
	void SetRoller(const float vel) {if(m_roller) m_roller->Set(vel);};
	
public:
	~Pickup();
};






#endif /*_PICKUP_H_*/
