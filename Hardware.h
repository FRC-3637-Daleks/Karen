/*
 * Hardware.h
 * Team 3637
 * 
 * Constants for all hardware on the robot or driver station
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

class CAN_PORTS
{
public:
	enum {DRIVE_REAR_LEFT=2, DRIVE_FRONT_LEFT, DRIVE_FRONT_RIGHT, DRIVE_REAR_RIGHT};
};

class PWM_PORTS
{
public:
	enum {ROLLER_TALONS=1, WINCH_TALONS=3};
};

class SOLENOIDS
{
public:
	enum {LATCH_IN=1, LATCH_OUT, CLUTCH_ON, CLUTCH_OFF /*not for use*/, RIGHT_PISTON_IN, RIGHT_PISTON_OUT, LEFT_PISTON_IN, LEFT_PISTON_OUT};
};


class USB_PORTS
{
public:
	enum {LEFT_JOY=1, RIGHT_JOY, GAMEPAD};
};


#endif
