/**
 * Practice robot code based off the sample code provided by WPI
 * Team 3637
 */
#include <cmath>
#include "WPILib.h"
#include "Hardware.h"
#include "Subsystems.h"

class Karen : public IterativeRobot
{
	// Declare variable for the robot drive system
	//RobotDrive *m_robotDrive;		// robot will use PWM 1-4 for drive motors
	
	// Declare variables for the two joysticks being used
	OperatorConsole *m_operatorConsole;
	
	DalekDrive *m_dalekDrive;
	
	Compressor *m_compressor;
	
	
	Catapult *m_catapult;
	
	Solenoid *m_solenoids[SOLENOIDS::LEFT_PISTON_OUT];
	
	Encoder *m_winchPos;
	
	Joystick *m_rightStick, *m_leftStick;
	GamePad *m_gamePad;
	
	bool pullingBack;
	
	// Local variables to count the number of periodic loops performed
	UINT32 m_autoPeriodicLoops;
	UINT32 m_disabledPeriodicLoops;
	UINT32 m_telePeriodicLoops;
public:

	Karen() {
		printf("PracticeRobot Constructor Started\n");
		
		for(UINT8 i = 0; i < SOLENOIDS::LEFT_PISTON_OUT; i++)
			m_solenoids[i] = new Solenoid(i+1);
		
		// Set up the robot for two motor drive
		m_dalekDrive = new DalekDrive(DalekDrive::MECANUM_WHEELS, 
				DalekDrive::Motor(CAN_PORTS::DRIVE_FRONT_LEFT, DalekDrive::LEFT_FRONT),
				DalekDrive::Motor(CAN_PORTS::DRIVE_FRONT_RIGHT, DalekDrive::RIGHT_FRONT),
				DalekDrive::Motor(CAN_PORTS::DRIVE_REAR_LEFT, DalekDrive::LEFT_REAR),
				DalekDrive::Motor(CAN_PORTS::DRIVE_REAR_RIGHT, DalekDrive::RIGHT_REAR));
		
		m_rightStick = new Joystick(USB_PORTS::RIGHT_JOY);
		m_leftStick = new Joystick(USB_PORTS::LEFT_JOY);
		m_gamePad = new GamePad(USB_PORTS::GAMEPAD);
		
		(*m_dalekDrive)[DalekDrive::LEFT_FRONT].SetFlip(false);
		(*m_dalekDrive)[DalekDrive::RIGHT_FRONT].SetFlip(false);
		(*m_dalekDrive)[DalekDrive::LEFT_REAR].SetFlip(false);
		(*m_dalekDrive)[DalekDrive::RIGHT_REAR].SetFlip(false);
		
		// Define joysticks on the Drivers Station
		m_operatorConsole = new OperatorConsole(OperatorConsole::ARCADE_DRIVE, m_rightStick, m_leftStick, m_gamePad);
		
		m_compressor = new Compressor(DIO_PORTS::PRESSURE_SWITCH, RELAY_PORTS::COMPRESSER_RELAY);
		
		m_winchPos = new Encoder(DIO_PORTS::ENCODER_A, DIO_PORTS::ENCODER_B);
		
		m_catapult = new Catapult(new Talon(PWM_PORTS::WINCH_TALONS), m_winchPos, m_solenoids[SOLENOIDS::CLUTCH_ON-1], m_solenoids[SOLENOIDS::CLUTCH_OFF-1],
				m_solenoids[SOLENOIDS::LATCH_OUT-1], m_solenoids[SOLENOIDS::LATCH_IN-1], new DigitalInput(DIO_PORTS::ENGAGED));
		
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
		m_compressor->Start();
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
		
		for(UINT8 i = 0; i < DalekDrive::N_MOTORS; i++)
			printf("motorFunction[%d] = %p\n", i, DalekDrive::Motor::mecFuncs[i]);
		
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
	
	void TestInit(void)
	{
		printf("Beginning Test Init\n");
	}
	
	void TestPeriodic(void) {
		printf("Engaged switch: %d", m_catapult->isAtStop());
		for(UINT8 i = 0; i < SOLENOIDS::LEFT_PISTON_OUT; i++)
			m_solenoids[i]->Set(m_rightStick->GetRawButton(i+1));
		
		SmartDashboard::PutNumber("WinchPosition", double(m_winchPos->Get()));
		printf("Winch Encoder: %d\n", int(m_winchPos->Get()));
	}

	
	void TeleopPeriodic(void) {
		// increment the number of teleop periodic loops completed
		m_telePeriodicLoops++;
		
//		m_dalekDrive->printMotors();
		/*if(m_operatorConsole->GetDrive() == OperatorConsole::ARCADE_DRIVE)
			m_dalekDrive->Drive(m_operatorConsole->GetX(), m_operatorConsole->GetY(), m_operatorConsole->GetTheta());
		else
			m_dalekDrive->Drive(m_operatorConsole->GetLeft(), m_operatorConsole->GetRight());
		*/
		m_dalekDrive->printMotors();
		//m_dalekDrive->Drive(0.0, 0.0, 0.0);
		if(!m_catapult->lockedAndloaded())
		{
			if(!pullingBack && m_operatorConsole->Engage())
			{
				pullingBack = true;
			}
			else if(pullingBack)
			{
				m_catapult->prepareFire();
			}
		}
		else if(m_operatorConsole->Disengage())
		{
			pullingBack = false;
			m_catapult->Fire();
		}
		
		
		SmartDashboard::PutBoolean("LockedAndLoaded",m_catapult->lockedAndloaded());		
	} // TeleopPeriodic(void)
	
	
};

START_ROBOT_CLASS(Karen);
