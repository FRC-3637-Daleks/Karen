#ifndef __VALVE_H__
#define __VALVE_H__

#include "Defines.h"
#include "WPILib.h"

class Solenoid;

class Valve {
public:
	Valve(uint32_t aChannel, uint32_t bChannel, bool open=true);
	Valve(uint8_t aModuleNumber, uint32_t aChannel, 
		  uint8_t bModuleNumber, uint32_t bChannel, bool open=true);
	Valve(Solenoid *aSource, Solenoid *bSource, bool open=true);
	Valve(Solenoid &aSource, Solenoid &bSource, bool open=true);
	virtual ~Valve() {};
	
	bool isOpen();
	bool isClosed();
	bool isValid();
	void Close();
	void Open();

private:
	Solenoid *m_a;
	Solenoid *m_b;
	bool m_needFree;
	bool m_invalid;
};

#endif
