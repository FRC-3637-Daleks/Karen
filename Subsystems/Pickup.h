#ifndef _PICKUP_H_
#define _PICKUP_H_

#include "WPILib.h"

class Pickup
{
public:
	enum {PICKUP_DOWN, PICKUP_MIDDLE, PICKUP_UP};
	
/*private:
	Solenoid *m_top;
	Solenoid *m_bottom;
	Talon *m_roller;
	DigitalInput *m_middle;
	
	bool allocated;
	int targetPos;
	int location;
	
public:
	Pickup(Solenoid * const top, Solenoid * const bottom, Talon * const roller, DigitalInput *mid, const int start): 
		m_top(top), m_bottom(bottom), m_roller(roller), m_middle(mid), allocated(false), targetPos(start), location(start) {};
	Pickup(Solenoid& top, Solenoid& bottom, Talon& roller, DigitalInput& mid, const int start)
	{Pickup(&top, &bottom, &roller, &mid, start);};
	Pickup(const UINT8 top, const UINT8 bottom, const UINT8 roller, const UINT8 mid, const int start)
	{Pickup(new Solenoid(top), new Solenoid(bottom), new Talon(roller), new DigitalInput(mid), start); allocated = true;};
	
public:
	void Up();
	void Down();
	void Stop();
	
	const bool SetPos(const int pos); // returns true if it's at the spot
	const int GetTarget() const {return targetPos;};   // returns the current target position
	const int GetConfig() const;	// returns the current location of the piston
	
public:
	void SetRoller(const float vel) {if(m_roller) m_roller->Set(vel);};
	
public:
	~Pickup();
	*/
};






#endif /*_PICKUP_H_*/
