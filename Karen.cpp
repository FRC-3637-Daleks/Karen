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
	//OperatorConsole foo;
	
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
		m_rightStick = new Joystick(USB_PORTS::RIGHT_JOY);
		m_leftStick = new Joystick(USB_PORTS::LEFT_JOY);
		m_gamePad = new GamePad(USB_PORTS::GAMEPAD);
		
		// Initialize counters to record the number of loops completed in autonomous and teleop modes
		m_autoPeriodicLoops = 0;
		m_disabledPeriodicLoops = 0;
		m_telePeriodicLoops = 0;
		
		printf("Ex-schwartzinagor Constructor Completed\n");
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
		
		
		
		switch(m_dalekDrive->GetWheels())
		{
		case DalekDrive::MECANUM_WHEELS:
			if(m_leftStick->GetZ() > 0.5)
				m_dalekDrive->Drive(-deadzone(m_rightStick->GetX()), deadzone(m_rightStick->GetY()), deadzone(m_leftStick->GetX()));
			else
			{
				float x = ((m_rightStick->GetX()) +
						(m_leftStick->GetX()))/2;
				float y = (m_rightStick->GetY()) + 
						(m_leftStick->GetY())/2;
				float theta = (m_rightStick->GetY()) - 
						(m_leftStick->GetY() )/2;
				m_dalekDrive->Drive(-deadzone(x), deadzone(y), theta);
			}
			break;
		case DalekDrive::OMNI_WHEELS:
		case DalekDrive::TRACTION_WHEELS:
			if(m_leftStick->GetZ() > 0.5)
				m_dalekDrive->Drive(0.0, deadzone(m_rightStick->GetY()), deadzone(m_rightStick->GetX()));
			else
				m_dalekDrive->Drive(-deadzone(m_leftStick->GetY()), deadzone(m_rightStick->GetY()));
			break;
		}
	} // TeleopPeriodic(void)
	
private:
	const float deadzone(const float in)
	{
		return fabs(in) > DEADZONE? in:0.0;
	}
	
	
};

START_ROBOT_CLASS(Karen);
