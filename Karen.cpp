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
	
#ifndef SHIT_MODE
	DalekDrive *m_dalekDrive;
#else
	RobotDrive *m_robotDrive;
#endif 
	
	Compressor *m_compressor;
	
	Catapult *m_catapult;
	
	Solenoid *m_solenoids[SOLENOIDS::PICKUP_PISTONS_TOP];
	
	Encoder *m_winchPos;
	
	Pickup *m_pickup;
	
	Joystick *m_rightStick, *m_leftStick;
	GamePad *m_gamePad;
	
	Talon *m_winch;
	
	bool pullingBack;
	
	// Local variables to count the number of periodic loops performed
	UINT32 m_autoPeriodicLoops;
	UINT32 m_disabledPeriodicLoops;
	UINT32 m_telePeriodicLoops;
public:

	Karen() {
		printf("PracticeRobot Constructor Started\n");
		
		for(UINT8 i = 0; i < SOLENOIDS::PICKUP_PISTONS_TOP; i++)
			m_solenoids[i] = new Solenoid(i+1);
		
		// Set up the robot for two motor drive
#ifndef SHIT_MODE
		m_dalekDrive = new DalekDrive(DalekDrive::MECANUM_WHEELS, 
				DalekDrive::Motor(CAN_PORTS::DRIVE_FRONT_LEFT, DalekDrive::LEFT_FRONT),
				DalekDrive::Motor(CAN_PORTS::DRIVE_FRONT_RIGHT, DalekDrive::RIGHT_FRONT),
				DalekDrive::Motor(CAN_PORTS::DRIVE_REAR_LEFT, DalekDrive::LEFT_REAR),
				DalekDrive::Motor(CAN_PORTS::DRIVE_REAR_RIGHT, DalekDrive::RIGHT_REAR));
		
		(*m_dalekDrive)[DalekDrive::LEFT_FRONT].SetFlip(true);
		(*m_dalekDrive)[DalekDrive::RIGHT_FRONT].SetFlip(false);
		(*m_dalekDrive)[DalekDrive::LEFT_REAR].SetFlip(true);
		(*m_dalekDrive)[DalekDrive::RIGHT_REAR].SetFlip(false);
#else
		m_robotDrive = new RobotDrive(new CANJaguar(CAN_PORTS::DRIVE_FRONT_LEFT), new CANJaguar(CAN_PORTS::DRIVE_REAR_LEFT), 
				new CANJaguar(CAN_PORTS::DRIVE_FRONT_RIGHT), new CANJaguar(CAN_PORTS::DRIVE_REAR_RIGHT));
#endif
		m_rightStick = new Joystick(USB_PORTS::RIGHT_JOY);
		m_leftStick = new Joystick(USB_PORTS::LEFT_JOY);
		m_gamePad = new GamePad(USB_PORTS::GAMEPAD);
		
		
		// Define joysticks on the Drivers Station
		m_operatorConsole = new OperatorConsole(OperatorConsole::ARCADE_DRIVE, m_rightStick, m_leftStick, m_gamePad);
		
		m_compressor = new Compressor(DIO_PORTS::PRESSURE_SWITCH, RELAY_PORTS::COMPRESSER_RELAY);
		
		m_winchPos = new Encoder(DIO_PORTS::ENCODER_A, DIO_PORTS::ENCODER_B);
		
		m_catapult = new Catapult(m_winch = new Talon(PWM_PORTS::WINCH_TALONS), m_winchPos, m_solenoids[SOLENOIDS::CLUTCH_ON-1], m_solenoids[SOLENOIDS::CLUTCH_OFF-1],
				m_solenoids[SOLENOIDS::LATCH_OUT-1], m_solenoids[SOLENOIDS::LATCH_IN-1], new DigitalInput(DIO_PORTS::ENGAGED));
		
		m_pickup = new Pickup(new Valve(m_solenoids[SOLENOIDS::PICKUP_PISTONS_BOTTOM-1], m_solenoids[SOLENOIDS::PICKUP_PISTONS_TOP-1]), 
				new Valve(m_solenoids[SOLENOIDS::PICKUP_PISTONS_STOP-1], m_solenoids[SOLENOIDS::PICKUP_PISTONS_OPEN]), 
				new Talon(PWM_PORTS::ROLLER_TALONS), new DigitalInput(DIO_PORTS::LEFT_REED), new DigitalInput(DIO_PORTS::RIGHT_REED), 
				Pickup::PICKUP_UP);
		
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
		for(UINT8 i = 0; i < SOLENOIDS::PICKUP_PISTONS_TOP; i++)
			m_solenoids[i]->Set(m_gamePad->GetRawButton(i+1));
		
		m_winch->Set(0.5*m_gamePad->GetAxis(GamePad::LEFT_Y));
		
		SmartDashboard::PutNumber("WinchPosition", double(m_winchPos->Get()));
		printf("Winch Encoder: %d\n", int(m_winchPos->Get()));
	}

	
	void TeleopPeriodic(void) {
		// increment the number of teleop periodic loops completed
		m_telePeriodicLoops++;
		
//		m_dalekDrive->printMotors();
#ifndef SHIT_MODE
		printf("Calling m_dalekDrive->Drive(%f, %f, %f)", m_operatorConsole->GetX(), m_operatorConsole->GetY(), m_operatorConsole->GetTheta());
		if(m_operatorConsole->GetDrive() == OperatorConsole::ARCADE_DRIVE)
			m_dalekDrive->Drive(m_operatorConsole->GetX(), m_operatorConsole->GetY(), m_operatorConsole->GetTheta());
		else
			m_dalekDrive->Drive(m_operatorConsole->GetLeft(), m_operatorConsole->GetRight());
#else
		m_robotDrive->MecanumDrive_Cartesian(m_operatorConsole->GetX(), m_operatorConsole->GetY(), m_operatorConsole->GetTheta());
#endif
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
		
		m_pickup->SetRoller(m_operatorConsole->GetRoller());
		m_pickup->SetPos(m_operatorConsole->GetRollerPosition());
		
		
		SmartDashboard::PutBoolean("LockedAndLoaded",m_catapult->lockedAndloaded());
		SmartDashboard::PutNumber("PickupPosition", m_operatorConsole->GetRollerPosition());
	} // TeleopPeriodic(void)
	
	
};

START_ROBOT_CLASS(Karen);
