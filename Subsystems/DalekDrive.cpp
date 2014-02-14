#include "DalekDrive.h"

DalekDrive::Motor::MECANUM_FUNCTION DalekDrive::Motor::mecFuncs[] = {leftFrontAlg, rightFrontAlg, leftRearAlg, rightRearAlg};

DalekDrive::Motor &DalekDrive::Motor::operator =(Motor m)
// swaps resources and allows the local parameter to safely destruct them
{
	std::swap(motor, m.motor);
	std::swap(refCount, m.refCount);
	refCount? ++*refCount:NULL;
	std::swap(flip, m.flip);
	std::swap(location, m.location);
	return *this;
}

DalekDrive::Motor::~Motor()
{
	if(refCount)  // sees if this has a reference counter pointer
	{
		if(!--*refCount && motor)  // if it does it subtracts one from it and checks to see if it's the last one
			delete motor;   // delets motor if necessary
		delete refCount;
	}
}


DalekDrive::DalekDrive(const Wheel_t wheelConfig, const DalekDrive::Motor m [])
{
	for(UINT8 i = 0; i < N_MOTORS; i++)
		motors[i] = m[i];
}

DalekDrive::DalekDrive(const Wheel_t wheelConfig, 
		const Motor & leftFront, 
		const Motor & rightFront, 
		const Motor & leftRear, 
		const Motor & rightRear)
{
	Motor ms[] = {leftFront, rightFront, leftRear, rightRear};
	DalekDrive(wheelConfig, ms);
	// calls alternate constructor with parameters rearranged
}

DalekDrive::DalekDrive(const Wheel_t wheelConfig, const UINT8 leftFront, const UINT8 rightFront, const UINT8 leftRear, const UINT8 rightRear)
{
	DalekDrive(wheelConfig, Motor(leftFront, LEFT_FRONT), Motor(rightFront, RIGHT_FRONT), Motor(leftRear, LEFT_REAR), Motor(rightRear, RIGHT_REAR));
	// calls alternate constructor with paramaters rearranged
}

void DalekDrive::Drive(const float x, const float y, const float theta)
{
	if(wheels == MECANUM_WHEELS)
		for(UINT8 i = 0; i < N_MOTORS; motors[i++].SetMecSpeed(x, y, theta));
	else
		for(UINT8 i = 0; i < N_MOTORS; motors[i++].SetMecSpeed(0.0, y, theta));
}

void DalekDrive::Drive(const float left, const float right)
{
	for(UINT8 i = 0; i < N_MOTORS; i++)
		motors[i].SetSpeed(i == LEFT_FRONT || i == RIGHT_FRONT? left:right);
}
