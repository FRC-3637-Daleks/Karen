#ifndef _DALEK_DRIVE_H_
#define _DALEK_DRIVE_H_

#include "WPILib.h"

float leftFrontAlg(float x, float y, float theta) {return x+y+theta;}
float rightFrontAlg(float x, float y, float theta) {return y-x-theta;}
float leftRearAlg(float x, float y, float theta) {return y-x+theta;}
float rightRearAlg(float x, float y, float theta) {return y+x-theta;}

class DalekDrive
{
public:  // static declares/defines
	enum {LEFT_FRONT=0, RIGHT_FRONT, LEFT_REAR, RIGHT_REAR, N_MOTORS};
	typedef enum {MECANUM_WHEELS, OMNI_WHEELS, TRACTION_WHEELS} Wheel_t;
	typedef enum {TANK_DRIVE, ARCADE_DRIVE} Drive_t;
	
	class Motor
	{
	private:
		typedef float MECANUM_FUNCTION(float, float, float);
		MECANUM_FUNCTION mecFuncs[N_MOTORS] = {leftFrontAlg, rightFrontAlg, leftRearAlg, rightRearAlg};
		CANJaguar *motor;
		bool flip;
		int *refCount;
		int location;
		
	public:
		Motor(CANJaguar const &m, const int loc, const bool f=false): motor(&m), location(loc), flip(f), refCount(NULL) {};
		Motor(CANJaguar * const m, const int loc, const bool f=false): motor(m), location(loc), flip(f), allocated(NULL) {};
		Motor(const UINT8 id, const int loc, const bool f=false): motor(new CANJaguar(id)), location(loc), flip(f), allocated(new refCount(1)) {};
		Motor(const Motor& m): motor(m.motor), location(m.location), flip(m.flip), refCount(m.refCount) {++*refcount;};
		
	public:
		void SetFlip(const bool b) {flip = b;};
		const bool GetFlip() const {return flip;};
		
	public:
		CANJaguar *operator-> () const {return motor;};
		void SetSpeed(const float speed) {motor->Set(speed*(flip? -1:1));};
		void SetMecSpeed(const float x, const float y, const float theta);
		
	public:
		~Motor() {if(refCount) if(!--*refCount && motor) delete motor;};  // put in .cpp
	};
	
private:  // data members
	Motor motors[N_MOTORS];
	Wheel_t wheels;
	
public:
	DalekDrive(Wheel_t wheelConfig, Motor const &leftFront, Motor const &rightFront, Motor const &leftRear, Motor const &rightRear);
	DalekDrive(Wheel_t wheelConfig, const UINT8 leftFront, const UINT8 rightFront, const UINT8 leftRear, const UINT8 rightRear);
	DalekDrive(Wheel_t wheelConfig, Motor m[N_MOTORS]);
	
	const Wheel_t GetWheels() const {return drive;};
	
public:
	void Drive(const float x, const float y, const float theta);
	void Drive(const float y, const float theta) {Drive(0.0, y, theta);};
};

#endif /*_DALEK_DRIVE_H_*/
