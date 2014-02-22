#include "DalekDrive.h"

float leftFrontAlg(float x, float y, float theta) {return x+y+theta;}
float rightFrontAlg(float x, float y, float theta) {return y-x-theta;}
float leftRearAlg(float x, float y, float theta) {return y-x+theta;}
float rightRearAlg(float x, float y, float theta) {return y+x-theta;}

DalekDrive::Motor::MECANUM_FUNCTION DalekDrive::Motor::mecFuncs[] = {leftFrontAlg, rightFrontAlg, leftRearAlg, rightRearAlg};



DalekDrive::Motor &DalekDrive::Motor::operator =(Motor m)
// swaps resources and allows the local parameter to safely destruct them
{
#ifdef DEBUG_DRIVE
	printf("precondition\nDalekDrive::Motor::operator= (.ref: %d)\n this.ref: %d\n", *m.refCount, *refCount);
#endif
	std::swap(motor, m.motor);
	std::swap(refCount, m.refCount);
	refCount? ++*refCount:NULL;
	std::swap(flip, m.flip);
	std::swap(location, m.location);
#ifdef DEBUG_DRIVE
	printf("postcondition\nDalekDrive::Motor::operator= (.ref: %d)\n this.ref: %d\n", *m.refCount, *refCount);
#endif
	return *this;
}

DalekDrive::Motor::~Motor()
{
	if(refCount)  // sees if this has a reference counter pointer
	{
#ifdef DEBUG_DRIVE
		printf("Destroying shared motor[%d] with %d references\n", location, *refCount);
#endif
		if(!--*refCount && motor)  // if it does it subtracts one from it and checks to see if it's the last one
			delete motor;   // delets motor if necessary
		delete refCount;
	}
}


void DalekDrive::init(const Wheel_t wheelConfig, const DalekDrive::Motor m [])
{
	printMotors();
	wheels = wheelConfig;
	for(UINT8 i = 0; i < N_MOTORS; i++)
		motors[i] = m[i];
	printMotors();
}

DalekDrive::DalekDrive(const Wheel_t wheelConfig, const DalekDrive::Motor m[])
{
	init(wheelConfig, m);
}

DalekDrive::DalekDrive(const Wheel_t wheelConfig, 
		const Motor & leftFront, 
		const Motor & rightFront, 
		const Motor & leftRear, 
		const Motor & rightRear)
{
#ifdef DEBUG_DRIVE
	printf("DalekDrive Reference Constructor\nPrecondition");
#endif
	printMotors();
	Motor ms[] = {leftFront, rightFront, leftRear, rightRear};
	init(wheelConfig, ms);
#ifdef DEBUG_DRIVE
	printf("Post condtion\n");
#endif
	printMotors();
	// calls alternate constructor with parameters rearranged
}

DalekDrive::DalekDrive(const Wheel_t wheelConfig, const UINT8 leftFront, const UINT8 rightFront, const UINT8 leftRear, const UINT8 rightRear)
{
#ifdef DEBUG_DRIVE
	printf("DalekDrive Port Constructor\n");
#endif
	Motor ms[] = {Motor(leftFront, LEFT_FRONT), Motor(rightFront, RIGHT_FRONT), Motor(leftRear, LEFT_REAR), Motor(rightRear, RIGHT_REAR)};
	init(wheelConfig, ms);
	// calls alternate constructor with paramaters rearranged
}

void DalekDrive::Drive(const float x, const float y, const float theta)
{
#ifdef DEBUG_DRIVE
	printf("DalekDrive::Drive(%f, %f, %f)\n", x, y, theta);
#endif
	if(wheels == MECANUM_WHEELS)
	{
		for(UINT8 i = 0; i < N_MOTORS; motors[i++].SetMecSpeed(x, y, theta))
#ifdef DEBUG_DRIVE
			printf("Setting motor[%d]\n", i);
#endif
	}
	else
	{
		for(UINT8 i = 0; i < N_MOTORS; motors[i++].SetMecSpeed(0.0, y, theta));
	}
}

void DalekDrive::Drive(const float left, const float right)
{
	for(UINT8 i = 0; i < N_MOTORS; i++)
		motors[i].SetSpeed(i == LEFT_FRONT || i == RIGHT_FRONT? left:right);
}

void DalekDrive::printMotors()
{
#ifdef DEBUG_DRIVE
	printf("Printing data from DalekDrive::motors\n");
	for(UINT8 i = 0; i < N_MOTORS; i++)
	{
		
		printf("motor[%d]: location=%d, CAN-Pointer=%p\n", i, motors[i].GetLocation(), motors[i].GetCan());	
	}
#endif
}

