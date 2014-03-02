/**
 * Practice robot code based off the sample code provided by WPI
 * Team 3637
 */
#include <cmath>
#include "WPILib.h"
#include "Hardware.h"
#include "Subsystems.h"
#include "NetworkTables/NetworkTable.h"

class Karen : public IterativeRobot
{
	// Pointers to Subsystem Variables
	OperatorConsole *m_operatorConsole;
	DalekDrive *m_dalekDrive;
	Catapult *m_catapult;
	Pickup /*truck*/ *m_pickup;
	Maxbotix *m_ultraSonicSensor;

	// Pointers to allocated resources
	Solenoid *m_solenoids[SOLENOIDS::PICKUP_PISTONS_TOP];
	Valve *m_stop, *m_direction;
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
	bool fired;
	double range;
	bool isHot;
	Timer autonTime;
	Timer testWait;
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

		m_ultraSonicSensor = new Maxbotix(ANALOG_PORTS::ULTRA_SONIC, 5.0, 20, Maxbotix::kCentimeters);

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
		m_pickup = 	new Pickup(m_direction = new Valve(m_solenoids[SOLENOIDS::PICKUP_PISTONS_BOTTOM-1], m_solenoids[SOLENOIDS::PICKUP_PISTONS_TOP-1]), 
				m_solenoids[SOLENOIDS::PICKUP_PISTONS_OPEN-1], m_solenoids[SOLENOIDS::PICKUP_PISTONS_STOP-1], 
				new Talon(PWM_PORTS::ROLLER_TALONS), m_leftReed, m_rightReed, Pickup::PICKUP_UP);
		
		try
		{
			m_table = NetworkTable::GetTable("autondata");
		}
		catch(exception e)
		{
		}
		printf("Karen Constructor Completed\n");
	}

	/********************************** Init Routines *************************************/

	void RobotInit(void) {
		printf("Built: %s %s\n", __DATE__, __TIME__);
		SmartDashboard::init();
		printf("RobotInit() completed.\n");
	}

	void DisabledInit(void) {
	}

	void AutonomousInit(void) {
		autonTime.Stop();
		autonTime.Reset();
		fired = false;
	}

	void TeleopInit(void) {
		
#ifdef DEBUG_DRIVE
		for(UINT8 i = 0; i < DalekDrive::N_MOTORS; i++)
			printf("motorFunction[%d] = %p\n", i, DalekDrive::Motor::mecFuncs[i]);
#endif

		pullingBack = false;
		m_compressor->Start();
	}

	/********************************** Periodic Routines *************************************/

	void DisabledPeriodic(void)  {
#ifdef AUTONOMOUS
		try
		{
			range = m_table->GetNumber("range");
		}
		catch(exception e)
		{
		}
#endif
		/*if(!SmartDashboard::GetBoolean("Auton"))
			return;
		*/
	}

	void AutonomousPeriodic(void) {
#ifdef AUTONOMOUS
		if(!SmartDashboard::GetBoolean("Auton"))
			return;
		getNetData();
		static float vel = 0.0;
		if(range < MIN_GOAL_RANGE_INCHES)
		{
			vel = vel > 0.75? 0.75:vel+0.05;
		}
		else if(range > MAX_GOAL_RANGE_INCHES)
		{
			vel = vel < -0.75? -0.75:vel-0.05;
		}
		else
		{
			vel -= fabs(vel)/vel*0.05;
			m_dalekDrive->Drive(0.0, 0.0, 0.0);
		}
		
		m_dalekDrive->Drive(0.0, vel, 0.0);
		
		if((isHot || autonTime.Get() > 8.0) && range < MAX_GOAL_RANGE_INCHES && range > MIN_GOAL_RANGE_INCHES)
			m_catapult->Fire();
		else
		{
			m_catapult->prepareFire();
			m_pickup->SetPos(Pickup::PICKUP_DOWN);
		}
		
		m_pickup->SetRoller(-1.0);
#else
//		if(range > MAX_GOAL_RANGE_INCHES)
		if(fired)
		{
			if(autonTime.Get() < 6.0)
				m_dalekDrive->Drive(0.0, -0.3, 0.0);
			else
				m_dalekDrive->Drive(0.0, 0.0, 0.0);
		}
		else if(!m_catapult->lockedAndloaded())
		{
			m_catapult->prepareFire();
			m_pickup->SetPos(Pickup::PICKUP_MIDDLE);
		}
		else
		{
			autonTime.Start();
			m_pickup->SetPos(Pickup::PICKUP_DOWN);
			if(autonTime.Get() > 1.5)
			{
				fired = true;
				m_catapult->Fire();
			}
		}
#endif
	}

	void TestInit(void)
	{
		printf("Beginning Test Init\n");
		m_compressor->Start();
		testWait.Start();
	}

	void TestPeriodic(void) {
		static int stage = 0;
		static bool waiting = true;
		if(stage >= 8)
			return;
		
		if(testWait.Get() > 1.0)
		{
			switch(stage)
			{
			case 0:
				{
					static float rollerVel = 0.0;
					if(waiting)
						printf("Rollers ramping up\n");
					m_pickup->SetRoller(rollerVel += 0.05 > 1.0? 1.0:rollerVel);
					if(rollerVel == 1.0)
						printf("Rollers at 100%%\n");
					if(testWait.Get() > 5.0)
					{
						printf("Stopping\n");
						m_pickup->SetRoller(0.0);
						testWait.Reset();
					}
					break;
				}
			case 1:
				{
					static float rollerBackVel = 0.0;
					if(waiting)
						printf("Rollers ramping backwards\n");
					m_pickup->SetRoller(rollerBackVel -= 0.05 < -1.0? -1.0:rollerBackVel);
					if(rollerBackVel == -1.0)
						printf("Rollers at -100%%\n");
					if(testWait.Get() > 5.0)
					{
						printf("Stopping\n");
						m_pickup->SetRoller(0.0);
						testWait.Reset();
					}
					break;
				}
			case 2:
				{
					if(waiting)
						printf("Moving arms up in 1 second, clear!\n");
					if(testWait.Get() > 2.0)
					{
						m_pickup->SetPos(Pickup::PICKUP_UP);
					}
					if(testWait.Get() > 5.0)
					{
						printf("Up\n");
						testWait.Reset();
					}
					break;
				}
			case 3:
				{
					if(waiting)
						printf("Moving arms to center in 1 second, clear!\n");
					if(testWait.Get() > 2.0)
					{
						m_pickup->SetPos(Pickup::PICKUP_MIDDLE);
					}
					if(testWait.Get() > 5.0)
					{
						printf("Stopped at Middle\n");
						testWait.Reset();
					}
					break;
				}
			case 4:
				{
					if(waiting)
						printf("Moving arms down in 1 second, clear!\n");
					if(testWait.Get() > 2.0)
					{
						m_pickup->SetPos(Pickup::PICKUP_DOWN);
					}
					if(testWait.Get() > 5.0)
					{
						printf("Down\n");
						testWait.Reset();
					}
					break;
				}
			case 5:
				{
					if(waiting)
						printf("Cocking capapult\n");
					m_catapult->prepareFire();
					if(testWait.Get() > 5.0)
					{
						printf("Cocked\n");
						testWait.Reset();
					}
					break;
				}
			case 6:
				{
					if(waiting)
						printf("Gently releasing, maybe\n");
					m_solenoids[SOLENOIDS::CLUTCH_ON-1]->Set(true);
					if(testWait.Get() > 2.0)
					{
						m_solenoids[SOLENOIDS::LATCH_IN-1]->Set(true);
						m_solenoids[SOLENOIDS::LATCH_OUT-1]->Set(false);
						m_winch->Set(-0.1);
					}
					break;
				}
			case 7:
				{
					printf("Okai bi\n");
					stage++;
				}
			}
			waiting = false;
		}
		else
		{
			if(!waiting)
				stage++;
			if(!m_operatorConsole->GetTestContinue())
				testWait.Reset();
			waiting = true;			
		}
		
		
		/*ManualPeriodic();
		if(m_gamePad->GetAxis(GamePad::PAD_Y) > 0.5)
		{
			m_direction->Open();
			m_stop->Open();
		}
		else if(m_gamePad->GetAxis(GamePad::PAD_Y) < -0.5)
		{
			m_stop->Open();
			m_direction->Close();
		}
		else
		{
			m_stop->Close();
		}

		m_winch->Set(0.5*m_gamePad->GetAxis(GamePad::LEFT_Y));
		m_pickup->SetRoller(-m_gamePad->GetAxis(GamePad::RIGHT_Y));
		printf("Left reed switch: %d; Right reed switch: %d; Light Sensor: %d", (int)m_leftReed->Get(), (int)m_rightReed->Get(), (int)m_lightSensor->Get());
		*/
	}

	void ManualPeriodic(void)
	{
		for(UINT8 i = 0; i < SOLENOIDS::PICKUP_PISTONS_TOP; i++)
		{
			m_solenoids[i]->Set(m_gamePad->GetRawButton(i+1));
		}
		
		printf("Engaged: %d\n", (int)m_catapult->isAtStop());
		m_winch->Set(0.75*m_gamePad->GetAxis(GamePad::LEFT_Y));	
	}

	void TeleopPeriodic(void) {
		// increment the number of teleop periodic loops completedgit

//		m_operatorConsole->SetPrecision(-1.0);
		m_operatorConsole->SetSquared(m_rightStick->GetZ() > 0.5);		

		if(m_operatorConsole->GetDrive() == OperatorConsole::ARCADE_DRIVE)
			m_dalekDrive->Drive(-m_operatorConsole->GetX(), m_operatorConsole->GetY(), -m_operatorConsole->GetTheta());
		else
			m_dalekDrive->Drive(m_operatorConsole->GetLeft(), m_operatorConsole->GetRight());	
		
#ifdef DEBUG_KAREN
		printf("Drive Complete\n");
#endif

		if(!m_operatorConsole->GetOverride())
		{
#ifdef DEBUG_KAREN
			printf("Override Mode\n");
#endif

			// CATAPULT
			/*
			if(m_operatorConsole->Safe())
				m_catapult->unprepareFire();
				*/
			m_catapult->setOverride(m_operatorConsole->ManualEngage());
				
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
				else
				{
//					m_catapult->unprepareFire();
				}
			}
			else if(m_operatorConsole->Safe())
			{
				pullingBack = false;
			}
			else if(m_operatorConsole->Disengage())// && m_pickup->GetLocation() == Pickup::PICKUP_DOWN)
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
			//printf("Roller Direction: %d\n", m_operatorConsole->GetRollerDirection());
			if(m_operatorConsole->GetRollerDirection() > 0)
				m_pickup->SetPos(Pickup::PICKUP_UP);
			else if(m_operatorConsole->GetRollerDirection() < 0)
				m_pickup->SetPos(Pickup::PICKUP_DOWN);
			else
			{
				m_pickup->Stop();
			}

#ifdef DEBUG_KAREN
			printf("Roller Complete\n");
#endif
		} 
		else
		{
			pullingBack = false;
			m_pickup->SetRoller(-m_operatorConsole->GetRoller());
			ManualPeriodic();
		}
			//m_pickup->SetPos(m_operatorConsole->GetRollerPosition());
			//		SmartDashboard::PutNumber("DistanceFromWall", m_ultraSonicSensor->GetRangeInInches());

		UpdateDash();
#ifdef DEBUG_KAREN
			printf("Teleop Periodic Looped\n");
#endif
		} // TeleopPeriodic(void)

private:
	void getNetData()
	{
#ifdef AUTONOMOUS
		try
		{
			if(m_table)
			{
				isHot = m_table->GetBoolean("ishot");
				range = m_table->GetNumber("range");
			}
		}
		catch(exception e)
		{
		}
#else
		if(m_ultraSonicSensor)
			range = m_ultraSonicSensor->GetRangeInInches();
			
#endif
	}
	
	void UpdateDash()
	{
		SmartDashboard::PutBoolean("LockedAndLoaded", m_catapult->lockedAndloaded());  // Make Indicator Light
		
		SmartDashboard::PutNumber("PickupPosition", m_operatorConsole->GetRollerPosition());  // Make Progress Bar
		switch(m_pickup->GetLocation()) 
		{
		case Pickup::PICKUP_DOWN:
			SmartDashboard::PutString("PickupAt", "DOWN");
			break;
		case Pickup::PICKUP_MIDDLE:
			SmartDashboard::PutString("PickupAt", "VERTICAL");
			break;
		case Pickup::PICKUP_UP:
			SmartDashboard::PutString("PickupAt", "UP");
			break;
		default:
			SmartDashboard::PutString("PickupAt", "???");
			break;
		}
		
		//SmartDashboard::PutBoolean("Hot", isHot);  // Make Indicator Light

		SmartDashboard::PutNumber("Range", range); // Make at least hard print, at best progress bar with print
		
		SmartDashboard::PutNumber("X", m_operatorConsole->GetX());
		SmartDashboard::PutNumber("Y", m_operatorConsole->GetY());
		SmartDashboard::PutNumber("Theta", m_operatorConsole->GetTheta());
		
//		SmartDashboard::PutNumber("Precision", m_operatorConsole->GetPrecision());  // Make Indicator Light
//		SmartDashboard::PutBoolean("InRange", range < MAX_GOAL_RANGE_INCHES && range > MIN_GOAL_RANGE_INCHES);  // Prints if it's in right range
		
	}
	
	
};

	START_ROBOT_CLASS(Karen);
