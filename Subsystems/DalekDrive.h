#ifndef _DALEK_DRIVE_H_
#define _DALEK_DRIVE_H_

#include "WPILib.h"

// functions to be stored in array for quick reference
float leftFrontAlg(float x, float y, float theta) {return x+y+theta;}
float rightFrontAlg(float x, float y, float theta) {return y-x-theta;}
float leftRearAlg(float x, float y, float theta) {return y-x+theta;}
float rightRearAlg(float x, float y, float theta) {return y+x-theta;}

// class to replace RobotDrive
class DalekDrive
{
public:  // static declares/defines
	enum {LEFT_FRONT=0, RIGHT_FRONT, LEFT_REAR, RIGHT_REAR, N_MOTORS}; // Wheel location defines
	typedef enum {MECANUM_WHEELS, OMNI_WHEELS, TRACTION_WHEELS} Wheel_t; 
	typedef enum {TANK_DRIVE, ARCADE_DRIVE} Drive_t;
	
	// Generic motor class which facilitates giving all wheels the same orders in different configurations
	class Motor
	{
	private:
		typedef float (*MECANUM_FUNCTION)(float, float, float); // function pointer type for converting x y and theta to an individual wheel speed
		static MECANUM_FUNCTION mecFuncs[N_MOTORS];
		CANJaguar *motor;
		bool flip;  // keeps track whether this motor is flipped
		int *refCount; // used 
		int location;
		
	public:
		Motor(CANJaguar &m, const int loc, const bool f=false): motor(&m), location(loc), flip(f), refCount(NULL) {};
		Motor(CANJaguar * const m, const int loc, const bool f=false): motor(m), location(loc), flip(f), refCount(NULL) {};
		Motor(const UINT8 id, const int loc, const bool f=false): motor(new CANJaguar(id)), location(loc), flip(f), refCount(new int(1)) {};
		Motor(const Motor& m): motor(m.motor), location(m.location), flip(m.flip), refCount(m.refCount) {refCount? ++*refCount:NULL;};
		Motor(): motor(NULL), location(N_MOTORS), flip(false), refCount(NULL) {}; // default constructor which sets motor to invalid state
		Motor& operator= (const Motor m);  // uses copy and swap method
		
	public:
		void SetFlip(const bool b) {flip = b;};
		const bool GetFlip() const {return flip;};
		
	public:
		CANJaguar *operator-> () const {return motor;};
		void SetSpeed(const float speed) {if(motor) motor->Set(speed*(flip? -1:1));};
		void SetMecSpeed(const float x, const float y, const float theta) {SetSpeed(mecFuncs[location](x, y, theta));};
		
	public:
		~Motor();
	};
	
private:  // data members
	Motor motors[N_MOTORS];  //motor array
	Wheel_t wheels; // Defines whether it every supplies the Motors and X val
	
public:
	DalekDrive(const Wheel_t wheelConfig, Motor const &leftFront, Motor const &rightFront, Motor const &leftRear, Motor const &rightRear);
	DalekDrive(const Wheel_t wheelConfig, const UINT8 leftFront, const UINT8 rightFront, const UINT8 leftRear, const UINT8 rightRear);
	DalekDrive(const Wheel_t wheelConfig, const Motor m[N_MOTORS]);
	
	const Wheel_t GetWheels() const {return wheels;};
	
public:
	void Drive(const float x, const float y, const float theta);
	void Drive(const float left, const float right);
};

#endif /*_DALEK_DRIVE_H_*/
		
