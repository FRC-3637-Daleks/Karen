#include "DalekDrive.h"

float leftFrontAlg(float x, float y, float theta) {return x+y+theta;}
float rightFrontAlg(float x, float y, float theta) {return y-x-theta;}
float leftRearAlg(float x, float y, float theta) {return y-x+theta;}
float rightRearAlg(float x, float y, float theta) {return y+x-theta;}

DalekDrive::Motor::MECANUM_FUNCTION DalekDrive::Motor::mecFuncs[] = {leftFrontAlg, rightFrontAlg, leftRearAlg, rightRearAlg};



DalekDrive::Motor &DalekDrive::Motor::operator =(Motor m)
// swaps resources and allows the local parameter to safely destruct them
{
	printf("precondition\nDalekDrive::Motor::operator= (.ref: %d)\n this.ref: %d\n", *m.refCount, *refCount);
	std::swap(motor, m.motor);
	std::swap(refCount, m.refCount);
	refCount? *refCount:NULL;
	std::swap(flip, m.flip);
	std::swap(location, m.location);
	printf("postcondition\nDalekDrive::Motor::operator= (.ref: %d)\n this.ref: %d\n", *m.refCount, *refCount);
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
	printf("DalekDrive::Drive(%f, %f, %f)\n", x, y, theta);
	if(wheels == MECANUM_WHEELS)
		for(UINT8 i = 0; i < N_MOTORS; motors[i++].SetMecSpeed(x, y, theta))
			printf("Setting motor[%d]\n", i);
	else
		for(UINT8 i = 0; i < N_MOTORS; motors[i++].SetMecSpeed(0.0, y, theta));
}

void DalekDrive::Drive(const float left, const float right)
{
	for(UINT8 i = 0; i < N_MOTORS; i++)
		motors[i].SetSpeed(i == LEFT_FRONT || i == RIGHT_FRONT? left:right);
}

void DalekDrive::printMotors()
{
	printf("Printing data from DalekDrive::motors\n");
	for(UINT8 i = 0; i < N_MOTORS; i++)
	{
		printf("motor[%i]: location=%d, CAN-Pointer=%p\n", i, motors[i].GetLocation(), motors[i].GetCan());	
	}
}








