/**
 * Practice robot code based off the sample code provided by WPI
 * Team 3637
 */
#include <cmath>
#include "WPILib.h"
#include "Hardware.h"
#include "Gamepad.h"

class Karen : public IterativeRobot
{
	// Declare variable for the robot drive system
	//RobotDrive *m_robotDrive;		// robot will use PWM 1-4 for drive motors
	
	// Declare variables for the two joysticks being used
	Joystick *m_rightStick;			// joystick 1 (arcade stick or right tank stick)
	Joystick *m_leftStick;			// joystick 2 (tank left stick)
	GamePad *m_gamePad;				// Gamepad (3) (subsystem controls)
	
	CANJaguar *m_rightFront, *m_leftFront, *m_rightRear, *m_leftRear;
	RobotDrive *m_robotDrive;
	
	Talon *m_winch, *m_roller;	
	
	// Local variables to count the number of periodic loops performed
	UINT32 m_autoPeriodicLoops;
	UINT32 m_disabledPeriodicLoops;
	UINT32 m_telePeriodicLoops;
	
	typedef enum {TRACTION_WHEELS, OMNI_WHEELS, MECANUM_WHEELS} Wheel_t;
	typedef enum {ARCADE_DRIVE, TANK_DRIVE} Drive_t;
	
	Wheel_t wheelConfig;
	Drive_t driveConfig;
	bool precision;
	bool squaredInputs;
	
	const float precisionFactor;
		
public:

	Karen(): precisionFactor(0.3) {
		printf("PracticeRobot Constructor Started\n");
		
		m_rightFront = new CANJaguar(CAN_PORTS::DRIVE_FRONT_RIGHT);
		m_rightRear  = new CANJaguar(CAN_PORTS::DRIVE_REAR_RIGHT);
		m_leftFront  = new CANJaguar(CAN_PORTS::DRIVE_FRONT_LEFT);
		m_leftRear   = new CANJaguar(CAN_PORTS::DRIVE_REAR_LEFT);
		
		// Set up the robot for two motor drive
		m_robotDrive = new RobotDrive(m_leftFront, m_leftRear, m_rightFront, m_rightRear);
		
		m_robotDrive->SetInvertedMotor(RobotDrive::kFrontLeftMotor, false);
		m_robotDrive->SetInvertedMotor(RobotDrive::kFrontRightMotor, true);
		m_robotDrive->SetInvertedMotor(RobotDrive::kRearLeftMotor, false);
		m_robotDrive->SetInvertedMotor(RobotDrive::kRearRightMotor, true);
		
		// Define talons
		
		//m_winch = new Talon(PWN_PORTS::WINCH_TALONS);
		
		
		// Define joysticks on the Drivers Station
		m_rightStick = new Joystick(USB_PORTS::RIGHT_JOY);
		m_leftStick = new Joystick(USB_PORTS::LEFT_JOY);
		m_gamePad = new GamePad(USB_PORTS::GAMEPAD);
		
		// Initialize counters to record the number of loops completed in autonomous and teleop modes
		m_autoPeriodicLoops = 0;
		m_disabledPeriodicLoops = 0;
		m_telePeriodicLoops = 0;
		
		driveConfig = ARCADE_DRIVE;
		wheelConfig = MECANUM_WHEELS;
		
		precision = false;
		squaredInputs = true;
		
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
		
		
		
		switch(wheelConfig)
		{
		case MECANUM_WHEELS:
			if(driveConfig == ARCADE_DRIVE)
				m_robotDrive->MecanumDrive_Cartesian(-deadzone(m_rightStick->GetX() * (squaredInputs? fabs(m_rightStick->GetX()):1.0) * (precision? precisionFactor:1.0)), 
						deadzone(m_rightStick->GetY()*(squaredInputs? fabs(m_rightStick->GetY()):1.0)*(precision? precisionFactor:1.0)), 
						deadzone(m_leftStick->GetX()*(squaredInputs? fabs(m_leftStick->GetX()):1.0))*(precision? precisionFactor:1.0));
			else
			{
				float x = ((m_rightStick->GetX() * (squaredInputs? fabs(m_rightStick->GetX()):1.0) * (precision? precisionFactor:1.0)) +
						(m_leftStick->GetX() * (squaredInputs? fabs(m_leftStick->GetX()):1.0) * (precision? precisionFactor:1.0)))/2;
				float y = (m_rightStick->GetY()*(squaredInputs? fabs(m_rightStick->GetY()):1.0)*(precision? precisionFactor:1.0) + 
						(m_leftStick->GetY() * (squaredInputs? fabs(m_leftStick->GetY()):1.0)*(precision? precisionFactor:1.0)))/2;
				float theta = (m_rightStick->GetY()*(squaredInputs? fabs(m_rightStick->GetY()):1.0)*(precision? precisionFactor:1.0) - 
						(m_leftStick->GetY() * (squaredInputs? fabs(m_leftStick->GetY()):1.0)*(precision? precisionFactor:1.0)))/2;
				m_robotDrive->MecanumDrive_Cartesian(-deadzone(x), deadzone(y), theta);
			}
			break;
		case OMNI_WHEELS:
		case TRACTION_WHEELS:
			if(driveConfig == ARCADE_DRIVE)
				m_robotDrive->ArcadeDrive(deadzone(m_rightStick->GetX())*(precision? precisionFactor:1.0), 
						deadzone(m_rightStick->GetY())*(precision? precisionFactor:1.0),
						squaredInputs);
			else
				m_robotDrive->TankDrive(-deadzone(m_leftStick->GetY()*(precision? precisionFactor:1.0)), 
						deadzone(m_rightStick->GetY()*(precision? precisionFactor:1.0)), squaredInputs);
			break;
		}
		
		ButtonUpdate();
		SmartDashboard::PutBoolean("Precision", precision);
		SmartDashboard::PutBoolean("Squard_Inputs", squaredInputs);
		SmartDashboard::PutString("ControlMode", driveConfig == ARCADE_DRIVE? "Arcade":"Tank Drive");
		SmartDashboard::PutString("Wheel_Setup", wheelConfig == MECANUM_WHEELS? "Mecanum":"Traction");
	} // TeleopPeriodic(void)
	
	
	void ButtonUpdate()
	{
		static bool precisionButton = false;
		static bool squaredButton = false;
		
		// Precision: press either trigger
		if(m_rightStick->GetTrigger() && m_leftStick->GetTrigger())
		{
			precisionButton = true;
		}
		else if(precisionButton)
		{
			precisionButton = false;
			precision = !precision;
			printf("Precision Driving factor set to: %f\n", (float)precisionFactor*(precision));
		}
		
		// Squared Toggle: 
		if(m_rightStick->GetRawButton(2) && m_leftStick->GetRawButton(2))
		{
			squaredButton = true;
		}
		else if(squaredButton)
		{
			squaredButton = false;
			squaredInputs = !squaredInputs;
			printf("Squared Input is set to: %i", (int)squaredInputs);
		}
		
		// If right stick Z is up, activate Arcade Arcade Mode
		if(m_rightStick->GetZ() > 0.5)
		{
			if(driveConfig != ARCADE_DRIVE)
				printf("Arcade Mode\n");
			driveConfig = ARCADE_DRIVE;
		}
		else
		{
			if(driveConfig != TANK_DRIVE)
				printf("Tank Mode\n");
			driveConfig = TANK_DRIVE;
		}
		
		// If left stick Z is up, this signifies Mecanum setup 
		if(m_leftStick->GetZ() > 0.5)
		{
			if(wheelConfig != MECANUM_WHEELS)
				printf("Driving for Mecanum wheels\n");
			wheelConfig = MECANUM_WHEELS;
		}
		else
		{
			if(wheelConfig != TRACTION_WHEELS)
				printf("Driving for traction/omni wheels\n");
			wheelConfig = TRACTION_WHEELS;
		}
	}
	
private:
	const float deadzone(const float in)
	{
		return fabs(in) > DEADZONE? in:0.0;
	}
	
	
};

START_ROBOT_CLASS(Karen);
