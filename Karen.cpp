/**
 * Practice robot code based off the sample code provided by WPI
 * Team 3637
 */
#include <cmath>
#include "WPILib.h"
#include "Hardware.h"
#include "Subsystems/Gamepad.h"
#include "Subsystems/DalekDrive.h"
#include "Subsystems/OperatorConsole.h"

class Karen : public IterativeRobot
{
	// Declare variable for the robot drive system
	//RobotDrive *m_robotDrive;		// robot will use PWM 1-4 for drive motors
	
	// Declare variables for the two joysticks being used
	OperatorConsole *OC;
	
	DalekDrive *m_dalekDrive;
	
	Talon *m_winch, *m_roller;	
	
	// Local variables to count the number of periodic loops performed
	UINT32 m_autoPeriodicLoops;
	UINT32 m_disabledPeriodicLoops;
	UINT32 m_telePeriodicLoops;
public:

	Karen() {
		printf("PracticeRobot Constructor Started\n");
		
		// Set up the robot for two motor drive
		m_dalekDrive = new DalekDrive(DalekDrive::MECANUM_WHEELS, 
				CAN_PORTS::DRIVE_FRONT_LEFT, CAN_PORTS::DRIVE_FRONT_RIGHT, CAN_PORTS::DRIVE_REAR_LEFT, CAN_PORTS::DRIVE_REAR_RIGHT);
		
		(*m_dalekDrive)[DalekDrive::LEFT_FRONT].SetFlip(false);
		(*m_dalekDrive)[DalekDrive::RIGHT_FRONT].SetFlip(false);
		(*m_dalekDrive)[DalekDrive::LEFT_REAR].SetFlip(false);
		(*m_dalekDrive)[DalekDrive::RIGHT_REAR].SetFlip(false);
		
		// Define joysticks on the Drivers Station
		OC = new OperatorConsole(OperatorConsole::ARCADE_DRIVE, USB_PORTS::RIGHT_JOY, USB_PORTS::LEFT_JOY, USB_PORTS::GAMEPAD);
		
		// Initialize counters to record the number of loops completed in autonomous and teleop modes
		m_autoPeriodicLoops = 0;
		m_disabledPeriodicLoops = 0;
		m_telePeriodicLoops = 0;
		
		printf("Karen Constructor Completed\n");
	}
	
	/********************************** Init Routines *************************************/

	void RobotInit(void) {
		// Actions which would be performed once (and only once) upon initialization of the
		// robot would be put here.
		
		printf("RobotInit() completed.\n");
		printf("Built: %s %s\n", __DATE__, __TIME__);
		
		SmartDashboard::init();
	}
	
	void DisabledInit(void) {
		m_disabledPeriodicLoops = 0;			// Reset the loop counter for disabled mode
	}

	void AutonomousInit(void) {
		m_autoPeriodicLoops = 0;				// Reset the loop counter for autonomous mode
	}

	void TeleopInit(void) {
		m_telePeriodicLoops = 0;				// Reset the loop counter for teleop mode
	}

	/********************************** Periodic Routines *************************************/
	
	void DisabledPeriodic(void)  {
		// increment the number of disabled periodic loops completed
		m_disabledPeriodicLoops++;
		
		// while disabled, print disabled
		//printf("Disabled: %d\r\n", m_disabledPeriodicLoops);
	}

	void AutonomousPeriodic(void) {
		
		m_autoPeriodicLoops++;
		
		// while autonomous, print disabled
		//printf("Autonomous: %d\r\n", m_autoPeriodicLoops);
	}

	
	void TeleopPeriodic(void) {
		// increment the number of teleop periodic loops completed
		m_telePeriodicLoops++;
		
		if(OC->GetDrive() == OperatorConsole::ARCADE_DRIVE)
			m_dalekDrive->Drive(OC->GetX(), OC->GetY(), OC->GetTheta());
		else
			m_dalekDrive->Drive(OC->GetLeft(), OC->GetRight());
		
	} // TeleopPeriodic(void)
	
	
};

START_ROBOT_CLASS(Karen);
