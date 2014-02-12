/*
 * Hardware.h
 * Team 3637
 * 
 * Constants for all hardware on the robot or driver station
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

#define DEADZONE 0.05

class CAN_PORTS
{
public:
	enum {DRIVE_REAR_LEFT=2, DRIVE_FRONT_LEFT, DRIVE_FRONT_RIGHT, DRIVE_REAR_RIGHT};
};

class PWM_PORTS
{
public:
	enum {ROLLER_TALONS, WINCH_TALONS};
};

class USB_PORTS
{
public:
	enum {LEFT_JOY=1, RIGHT_JOY, GAMEPAD};
};


#endif
