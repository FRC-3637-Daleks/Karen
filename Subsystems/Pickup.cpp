#include "Pickup.h"

Pickup::Pickup(Valve * const direction, Solenoid * const stop,
		Solenoid * const move, Talon * const roller, DigitalInput *midLeft,
		DigitalInput *midRight) {
	m_direction			= direction;
	m_stop				= stop;
	m_move				= move;
	m_roller			= roller;
	m_middleLeftReed	= midLeft;
	m_middleRightReed	= midRight;
	m_state				= PICKUP_STATE_MIDDLE;
	m_targetState		= PICKUP_STATE_NONE;
	allocated			= false;
	m_reedWatch = new Task("reedMonitor", (FUNCPTR) WatchReeds, REEDWATCH_PRIORITY);
	m_reedWatch->Start((uint32_t) this);
	Lock();

}

Pickup::Pickup(Valve& direction, Solenoid& stop, Solenoid& move, Talon& roller,
		DigitalInput& midLeft, DigitalInput& midRight) {
	m_direction			= &direction;
	m_stop				= &stop;
	m_move				= &move;
	m_roller			= &roller;
	m_middleLeftReed	= &midLeft;
	m_middleRightReed	= &midRight;
	m_state				= PICKUP_STATE_MIDDLE;
	m_targetState		= PICKUP_STATE_NONE;
	allocated 			= false;
	m_reedWatch = new Task("reedMonitor", (FUNCPTR) WatchReeds, REEDWATCH_PRIORITY);
	m_reedWatch->Start((uint32_t) this);
	Lock();
}

Pickup::Pickup(const UINT8 top, const UINT8 bottom, const UINT8 stop_a,
		const UINT8 stop_b, const UINT8 roller, const UINT8 midLeft,
		const UINT8 midRight) {
	m_direction			= new Valve(top, bottom);
	m_stop				= new Solenoid(stop_a);
	m_move				= new Solenoid(stop_b);
	m_roller			= new Talon(roller);
	m_middleLeftReed	= new DigitalInput(midLeft);
	m_middleRightReed	= new DigitalInput(midRight);
	m_state				= PICKUP_STATE_MIDDLE;
	m_targetState		= PICKUP_STATE_NONE;
	allocated			= true;
	m_reedWatch = new Task("reedMonitor", (FUNCPTR) WatchReeds, REEDWATCH_PRIORITY);
	m_reedWatch->Start((uint32_t) this);
	Lock();
}

void Pickup::Up() {
	Unlock();
	m_movementDirection = PICKUP_DIRECTION_UP;
	m_direction->Close();
}

void Pickup::Down() {
	Unlock();
	m_movementDirection = PICKUP_DIRECTION_DOWN;
	m_direction->Open();
}

void Pickup::Lock() {
	m_movementDirection = PICKUP_DIRECTION_LOCKED;
	m_stop->Set(true);
	m_move->Set(false);
}

void Pickup::Unlock() {
	m_stop->Set(false);
	m_move->Set(true);
}

bool Pickup::CenterArms() {
	if (m_state = PICKUP_STATE_MIDDLE) {
		return true;
	}

	switch(m_state) {
	case PICKUP_STATE_UP:
		Down();
		break;
	case PICKUP_STATE_DOWN:
		Up();
		break;
	default:
		break;
	}
	m_targetState = PICKUP_STATE_MIDDLE;
}

void Pickup::CheckArms() {
	if (!m_middleLeftReed || !m_middleRightReed)
		return;

	// Lock arms immediately when reached middle position. Must be done here to catch the reed switch.
	if (m_targetState == PICKUP_STATE_MIDDLE && (!m_middleLeftReed->Get() || !m_middleRightReed->Get())) {
		Lock();
		m_state = PICKUP_STATE_MIDDLE;
		m_targetState = PICKUP_STATE_NONE;
	}

	switch (m_movementDirection) {
	case PICKUP_DIRECTION_UP:
		if (!m_middleLeftReed->Get() || !m_middleRightReed->Get()) {
			// Triggered midpoint going down -> up
			m_state = PICKUP_STATE_UP;
		}
		break;
	case PICKUP_DIRECTION_DOWN:
		if (!m_middleLeftReed->Get() || !m_middleRightReed->Get()) {
			// Triggered midpoint going up -> down
			m_state = PICKUP_STATE_DOWN;
		}
		break;
	default:
		break;
	}
	return;
}

Pickup::~Pickup() {
	if (m_reedWatch) {
		m_reedWatch->Stop();
		delete m_reedWatch;
	}
	if (!allocated)
		return;

	if (m_roller)
		delete m_roller;

	if (m_middleLeftReed)
		delete m_middleLeftReed;

	if (m_middleRightReed)
		delete m_middleRightReed;

	if (m_stop)
		delete m_stop;
	if (m_move)
		delete m_move;
	if (m_direction)
		delete m_direction;
}

void WatchReeds(Pickup *armsObj) {
	while (true) {
		armsObj->CheckArms();
		sleep(1); // Give up the thread so we do not cause a busy wake
	}
}
