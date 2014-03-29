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
	Pickup *m_pickup;
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

	enum auton_state {
		AUTON_STATE_PREPARE_FIRE,
		AUTON_STATE_FIRE,
		AUTON_STATE_FIRED,
		AUTON_STATE_LOAD_ROLLER,
		AUTON_STATE_LOAD_ARMS,
		AUTON_STATE_DONE
	};
	
	enum auton_mode {
		AUTON_MODE_ONE_BALL,
		AUTON_MODE_TWO_BALL,
		AUTON_MODE_PUSH_BALL
	};

	auton_state m_autonState;
	auton_mode  m_autonMode;

	// Miscellaneous variables
	double range;
	bool isHot;
	Timer autonTime;
	Timer testWait;
	bool firing;

public:
	Karen(): isHot(false),firing(false)
	{
		printf("Karen Constructor Started\n");

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
				new Talon(PWM_PORTS::ROLLER_TALONS), m_leftReed, m_rightReed);

		try
		{
			m_table = NetworkTable::GetTable("autondata");
		}
		catch(exception e)
		{
			m_table = NULL;
		}
		printf("Karen Constructor Completed\n");
	}

	/********************************** Init Routines *************************************/

	void RobotInit(void)
	{
		printf("Built: %s %s\n", __DATE__, __TIME__);
		SmartDashboard::init();
		printf("RobotInit() completed.\n");
	}

	void DisabledInit(void)
	{
		// Nada
	}

	void AutonomousInit(void)
	{
		m_catapult->CheckPosition();
		m_compressor->Start();
		autonTime.Stop();
		autonTime.Reset();
		autonTime.Start();
		m_autonState = AUTON_STATE_PREPARE_FIRE;
		m_autonMode = AUTON_MODE_ONE_BALL;
		if(m_autonMode == AUTON_MODE_PUSH_BALL)
		{
			m_autonState = AUTON_STATE_DONE;
		}
	}

	void TestInit(void)
	{
		// Nada
	}

	void TeleopInit(void)
	{
#ifdef DEBUG_DRIVE
		for(UINT8 i = 0; i < DalekDrive::N_MOTORS; i++)
			printf("motorFunction[%d] = %p\n", i, DalekDrive::Motor::mecFuncs[i]);
#endif
		m_compressor->Start();
	}

	/********************************** Periodic Routines *************************************/

	void DisabledPeriodic(void)
	{
		// Nada
		PollSensorData();
		UpdateDash();
	}

	void AutonomousPeriodic(void) {
		PollSensorData();
		switch (m_autonState) {
		case AUTON_STATE_PREPARE_FIRE:
			printf("AUTON_STATE_PREPARING FIRE");
			m_pickup->SetRoller(0.0);
			m_catapult->PrepareFire();
			if (m_catapult->GetState() == Catapult::CATAPULT_STATE_READY) {
				m_pickup->Down();
				if (m_pickup->GetState() == Pickup::PICKUP_STATE_DOWN && autonTime.Get() > 1.5) {
					if(isHot || autonTime.Get() > 5.0) {
						m_autonState = AUTON_STATE_FIRE;
					}
				}
			}
			break;
		case AUTON_STATE_FIRE:
			m_catapult->Fire();
			if (autonTime.Get() > 1.6) {
				m_autonState = AUTON_STATE_FIRED;
			}
			break;
		case AUTON_STATE_FIRED:
			if(m_autonMode == AUTON_MODE_TWO_BALL)
			{
				m_autonMode = AUTON_MODE_ONE_BALL;
				m_autonState = AUTON_STATE_LOAD_ROLLER;
			}
			else if(m_autonMode == AUTON_MODE_ONE_BALL)
			{
				m_autonMode = AUTON_MODE_PUSH_BALL;
				m_autonState = AUTON_STATE_DONE;
			}
			break;
		case AUTON_STATE_LOAD_ROLLER:
			if(autonTime.Get() < 4.0)
			{
				m_pickup->SetRoller(-1.0);
			}
			else
			{
				m_autonState = AUTON_STATE_LOAD_ARMS;
			}
			break;
		case AUTON_STATE_LOAD_ARMS:
			m_pickup->Up();
			autonTime.Reset();
			m_autonState = AUTON_STATE_PREPARE_FIRE;
			break;
		case AUTON_STATE_DONE:
			// Now move forward
			autonTime.Reset();
			if(autonTime.Get() < 2.0) {
				m_dalekDrive->Drive(0.0, -0.3, 0.0);
			} else {
				m_dalekDrive->Drive(0.0, 0.0, 0.0);
			}
			break;
		}
		UpdateDash();
	}

	void TestPeriodic(void)
	{
		PollSensorData();
		UpdateDash();
	}

	void TeleopPeriodic(void)
	{
		PollSensorData();
		// increment the number of teleop periodic loops completed

		m_operatorConsole->SetSquared(m_rightStick->GetZ() > 0.5);		

		if(m_operatorConsole->GetDrive() == OperatorConsole::ARCADE_DRIVE)
			m_dalekDrive->Drive(-m_operatorConsole->GetX(), m_operatorConsole->GetY(), -m_operatorConsole->GetTheta());
		else
			m_dalekDrive->Drive(m_operatorConsole->GetLeft(), m_operatorConsole->GetRight());

		// Pickup roller
		m_pickup->SetRoller(-m_operatorConsole->GetRoller());

		if(!m_operatorConsole->GetOverride())
		{
			// Normal Control Mode

			// CATAPULT
			
			if (m_operatorConsole->CatapultEmergencyRelease())
			{
				firing = false;
				m_catapult->UnprepareFire();
			}
			else if (m_operatorConsole->CatapultFire()) {
				// The catapult should only fire if the pickup arms are down
				m_catapult->Fire();
				firing = false;
			}
			else if (m_operatorConsole->CatapultPrepareFire() || firing) {
				m_catapult->PrepareFire();
				firing = true;
			}
			
			// ROLLERS
			if (m_operatorConsole->RollerUp()) {
				m_pickup->Up();
			} else if (m_operatorConsole->RollerDown()) {
				m_pickup->Down();
			} else if (m_operatorConsole->RollerCenter()) {
				m_pickup->CenterArms();
			}

		} 
		else
		{
			// Debug manual mode
			for(UINT8 i = 0; i < SOLENOIDS::PICKUP_PISTONS_TOP; i++)
			{
				m_solenoids[i]->Set(m_gamePad->GetRawButton(i+1));
			}

			m_winch->Set(0.75*m_gamePad->GetAxis(GamePad::LEFT_Y));
		}
		UpdateDash();
	} // TeleopPeriodic(void)

private:
	void PollSensorData()
	{
		try
		{
			if(m_table)
			{
				isHot = m_table->GetBoolean("ishot");
				//range = m_table->GetNumber("range");
			}
			else if(m_ultraSonicSensor)
			{
				isHot = true;
				//range = m_ultraSonicSensor->GetRangeInInches();
			}
		}
		catch(exception e)
		{
			printf(e.what());
			isHot = true;
		}
	}

	void UpdateDash()
	{
		SmartDashboard::PutNumber("CatapultState", (int)m_catapult->GetState());

		switch(m_pickup->GetState())
		{
		case Pickup::PICKUP_STATE_DOWN:
			SmartDashboard::PutString("PickupAt", "DOWN");
			break;
		case Pickup::PICKUP_STATE_MIDDLE:
			SmartDashboard::PutString("PickupAt", "VERTICAL");
			break;
		case Pickup::PICKUP_STATE_UP:
			SmartDashboard::PutString("PickupAt", "UP");
			break;
		default:
			SmartDashboard::PutString("PickupAt", "???");
			break;
		}
		SmartDashboard::PutNumber("Range", range); // Make at least hard print, at best progress bar with print
		SmartDashboard::PutBoolean("Hot", isHot);

		SmartDashboard::PutNumber("X", m_operatorConsole->GetX());
		SmartDashboard::PutNumber("Y", m_operatorConsole->GetY());
		SmartDashboard::PutNumber("Theta", m_operatorConsole->GetTheta());

	}
};

START_ROBOT_CLASS(Karen);
