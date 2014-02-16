#ifndef MAXBOTIX_H_
#define MAXBOTIX_H_

#include "SensorBase.h"
#include "PIDSource.h"

class AnalogChannel;
class AnalogModule;

/**
 * Maxbotix rangefinder class.
 * The Maxbotix rangefinder measures absolute distance based on the round-trip time
 * of a ping generated by the controller. 
 */
class Maxbotix: public SensorBase, public PIDSource
{
public:
	typedef enum {
		kInches = 0,
		kCentimeters = 1
	} DistanceUnit;

	Maxbotix(AnalogChannel *sensorChannel, float maxVoltage, 
			float minDistance, DistanceUnit units = kInches);
	Maxbotix(AnalogChannel &sensorChannel, float maxVoltage, 
			float minDistance, DistanceUnit units = kInches);
	Maxbotix(UINT32 sensorChannel, float maxVoltage, 
			float minDistance, DistanceUnit units = kInches);
	Maxbotix(UINT8 sensorModuleNumber, UINT32 sensorChannel, float maxVoltage, 
			float minDistance, DistanceUnit units = kInches);
	virtual ~Maxbotix() {};

	float GetRangeInInches();
	float GetRangeInCM();
	float GetVoltage();
	
	double PIDGet();
	void SetDistanceUnits(DistanceUnit units);
	DistanceUnit GetDistanceUnits();

private:
	const static float m_sampleRate  = 1000;
	const static uint32_t m_averageBits = 3;
	const static float m_cm2in = 0.39370;
	AnalogChannel *m_channel;
	DistanceUnit m_units;
	float m_maxVolts;
	float m_minDistance;
	float m_minVoltage;
	float m_voltsPerCM;
	
};

#endif

