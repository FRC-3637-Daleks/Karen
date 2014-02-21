/**
 * Practice robot code based off the sample code provided by WPI
 * Team 3637
 */
#include <cmath>
#include "WPILib.h"
#include "Hardware.h"
#include "Subsystems.h"
#include "NetworkTables/NetworkTable.h"
#include "Defines.h"

class Karen : public IterativeRobot
{
	// Pointers to Subsystem Variables
	OperatorConsole *m_operatorConsole;
	DalekDrive *m_dalekDrive;
	Catapult *m_catapult;
	Pickup /*truck*/ *m_pickup;
//	Maxbotix *m_ultraSonicSensor;
	
	// Pointers to allocated resources
	Solenoid *m_solenoids[SOLENOIDS::PICKUP_PISTONS_TOP];
	Compressor *m_compressor;
	Encoder *m_winchPos;
	Joystick *m_rightStick, *m_leftStick;
	GamePad *m_gamePad;
	Talon *m_winch;
	DigitalInput *m_leftReed, *m_rightReed;
	DigitalInput *m_lightSensor;
	
	NetworkTable *m_table;
	
	// Miscellaneous variables
	bool pullingBack;
	int distance;
	bool isHot;
	
	// Local variables to count the number of periodic loops performed
	UINT32 m_autoPeriodicLoops;
	UINT32 m_disabledPeriodicLoops;
	UINT32 m_telePeriodicLoops;
public:

	Karen() {
		printf("PracticeRobot Constructor Started\n");
		
		m_compressor = 	new Compressor(DIO_PORTS::PRESSURE_SWITCH, RELAY_PORTS::COMPRESSER_RELAY);
		
		for(UINT8 i = 0; i < SOLENOIDS::PICKUP_PISTONS_TOP; i++)
			m_solenoids[i] = new Solenoid(i+1);
		
		m_rightStick = 	new Joystick(USB_PORTS::RIGHT_JOY);
		m_leftStick = 	new Joystick(USB_PORTS::LEFT_JOY);
		m_gamePad = 	new GamePad(USB_PORTS::GAMEPAD);
		
		m_leftReed = 	new DigitalInput(DIO_PORTS::LEFT_REED);
		m_rightReed = 	new DigitalInput(DIO_PORTS::RIGHT_REED);
		m_lightSensor = new DigitalInput(DIO_PORTS::LIGHT_SENSOR);
		
		m_winchPos = 	new Encoder(DIO_PORTS::ENCODER_A, DIO_PORTS::ENCODER_B);
		m_winch = 		new Talon(PWM_PORTS::WINCH_TALONS);
		
//		m_ultraSonicSensor = new Maxbotix(ANALOG_PORTS::ULTRA_SONIC, 5.0, 20, Maxbotix::kCentimeters);
		
		// Set up the robot for two motor drive
		m_dalekDrive = 	new DalekDrive(DalekDrive::MECANUM_WHEELS, CAN_PORTS::DRIVE_FRONT_LEFT, CAN_PORTS::DRIVE_FRONT_RIGHT, 
																  CAN_PORTS::DRIVE_REAR_LEFT,	CAN_PORTS::DRIVE_REAR_RIGHT);
		(*m_dalekDrive)[DalekDrive::LEFT_FRONT].SetFlip(true);
		(*m_dalekDrive)[DalekDrive::RIGHT_FRONT].SetFlip(false);
		(*m_dalekDrive)[DalekDrive::LEFT_REAR].SetFlip(true);
		(*m_dalekDrive)[DalekDrive::RIGHT_REAR].SetFlip(false);
		
		// Define joysticks on the Drivers Station
		m_operatorConsole = new OperatorConsole(OperatorConsole::ARCADE_DRIVE, m_leftStick, m_rightStick, m_gamePad);
		
		// Define catapult members
		m_catapult = 	new Catapult(m_winch, m_winchPos, m_solenoids[SOLENOIDS::CLUTCH_ON-1], m_solenoids[SOLENOIDS::CLUTCH_OFF-1],
				m_solenoids[SOLENOIDS::LATCH_OUT-1], m_solenoids[SOLENOIDS::LATCH_IN-1], new DigitalInput(DIO_PORTS::ENGAGED));
		
		// Define Pickup system members
		m_pickup = 	new Pickup(new Valve(m_solenoids[SOLENOIDS::PICKUP_PISTONS_BOTTOM-1], m_solenoids[SOLENOIDS::PICKUP_PISTONS_TOP-1]), 
								new Valve(m_solenoids[SOLENOIDS::PICKUP_PISTONS_STOP-1], m_solenoids[SOLENOIDS::PICKUP_PISTONS_OPEN]), 
								new Talon(PWM_PORTS::ROLLER_TALONS), m_leftReed, m_rightReed, Pickup::PICKUP_UP);
		
//		m_table = NetworkTable::GetTable("autondata");
		// Initialize counters to record the number of loops completed in autonomous and teleop modes
		m_autoPeriodicLoops = 0;
		m_disabledPeriodicLoops = 0;
		m_telePeriodicLoops = 0;	
		
		printf("Karen Constructor Completed\n");
	}
	
	/********************************** Init Routines *************************************/

	void RobotInit(void) {
		printf("Built: %s %s\n", __DATE__, __TIME__);
//		m_catapult->prepareFire();
		SmartDashboard::init();
		printf("RobotInit() completed.\n");
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
		
		pullingBack = false;
		m_compressor->Start();
		m_telePeriodicLoops = 0;				// Reset the loop counter for teleop mode
	}

	/********************************** Periodic Routines *************************************/
	
	void DisabledPeriodic(void)  {
		// increment the number of disabled periodic loops completed
		m_disabledPeriodicLoops++;
//		distance = m_table->GetNumber("distance");
		// while disabled, print disabled
		//printf("Disabled: %d\r\n", m_disabledPeriodicLoops);
	}

	void AutonomousPeriodic(void) {
		m_autoPeriodicLoops++;
//		SmartDashboard::PutNumber("DistanceFromWall", m_ultraSonicSensor->GetRangeInInches());
//		isHot = m_table->GetBoolean("ishot");
//		distance = m_table->GetNumber("distance");
/*		if(distance < MIN_GOAL_RANGE_INCHES)
		{
			m_dalekDrive->Drive(0.5, 0.5);
		}
		else if(distance > MAX_GOAL_RANGE_INCHES)
		{
			m_dalekDrive->Drive(-0.5, -0.5);
		}
		if(isHot && distance < MAX_GOAL_RANGE_INCHES && distance > MIN_GOAL_RANGE_INCHES)
			m_catapult->Fire();
		else
		{
			m_catapult->prepareFire();
			m_pickup->Down();
		}
		*/
	}
	
	void TestInit(void)
	{
		printf("Beginning Test Init\n");
		m_compressor->Start();
		
	}
	
	void TestPeriodic(void) {
//		printf("Engaged switch: %d", m_catapult->isAtStop());
		for(UINT8 i = 0; i < SOLENOIDS::PICKUP_PISTONS_TOP; i++)
		{
			m_solenoids[i]->Set(m_gamePad->GetRawButton(i+1));
//			m_solenoids[i]->StopLiveWindowMode();
		}
		
		m_winch->Set(0.5*m_gamePad->GetAxis(GamePad::LEFT_Y));
		m_pickup->SetRoller(-m_gamePad->GetAxis(GamePad::RIGHT_Y));
		
		
		SmartDashboard::PutNumber("WinchPosition", double(m_winchPos->Get()));
		printf("Left reed switch: %d; Right reed switch: %d; Light Sensor: %d", (int)m_leftReed->Get(), (int)m_rightReed->Get(), (int)m_lightSensor->Get());
//		printf("Winch Encoder: %d\n", int(m_winchPos->Get()));
	}

	
	void TeleopPeriodic(void) {
		// increment the number of teleop periodic loops completed
		m_telePeriodicLoops++;
		
		m_operatorConsole->SetPrecision((m_leftStick->GetZ()+m_rightStick->GetZ())/2);
		
		if(m_operatorConsole->GetDrive() == OperatorConsole::ARCADE_DRIVE)
			m_dalekDrive->Drive(m_operatorConsole->GetX(), -m_operatorConsole->GetY(), m_operatorConsole->GetTheta());
		else
			m_dalekDrive->Drive(m_operatorConsole->GetLeft(), m_operatorConsole->GetRight());	

#ifdef DEBUG_KAREN
		printf("Drive Complete\n");
#endif
		
		// CATAPULT
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
		else if(m_operatorConsole->Disengage()/* && m_pickup->GetLocation() == Pickup::PICKUP_DOWN*/)
		{
			pullingBack = false;
			m_catapult->Fire();
		}

#ifdef DEBUG_KAREN
		printf("Catapult Complete\n");
#endif
		
		// ROLLER SPEED
		m_pickup->SetRoller(-m_operatorConsole->GetRoller());
		
		// ROLLER POSITION
		printf("Roller Direction: %d\n", m_operatorConsole->GetRollerDirection());
		if(m_operatorConsole->GetRollerDirection() > 0)
			m_pickup->Up();
		else if(m_operatorConsole->GetRollerDirection() < 0)
			m_pickup->Down();
		else
			m_pickup->Stop();

#ifdef DEBUG_KAREN
		printf("Roller Complete\n");
#endif
		
		//m_pickup->SetPos(m_operatorConsole->GetRollerPosition());		
		
		SmartDashboard::PutBoolean("LockedAndLoaded", m_catapult->lockedAndloaded());
		SmartDashboard::PutNumber("PickupPosition", m_operatorConsole->GetRollerPosition());
//		SmartDashboard::PutNumber("DistanceFromWall", m_ultraSonicSensor->GetRangeInInches());
		
#ifdef DEBUG_KAREN
		printf("Teleop Periodic Looped\n");
#endif
	} // TeleopPeriodic(void)
	
	
};

START_ROBOT_CLASS(Karen);
