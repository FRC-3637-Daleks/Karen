#include "WPILib.h"
#include "Maxbotix.h"

Maxbotix::Maxbotix(AnalogChannel *sensorChannel, float maxVoltage, 
		float minDistance, DistanceUnit units)
{
		AnalogModule *module;
		
		m_channel = sensorChannel;
		
		module = m_channel->GetModule();
		module->SetSampleRate(m_sampleRate);
		m_channel->SetAverageBits(m_averageBits);
		m_voltsPerCM = maxVoltage / 1024.0;
		m_minDistance = 20.0;
		m_maxVolts = maxVoltage;
}

Maxbotix::Maxbotix(AnalogChannel &sensorChannel, float maxVoltage, 
		float minDistance, DistanceUnit units)
{
		AnalogModule *module;

		m_channel = &sensorChannel;
		module = m_channel->GetModule();
		module->SetSampleRate(m_sampleRate);
		m_channel->SetAverageBits(m_averageBits);
		m_voltsPerCM = maxVoltage / 1024.0;
		m_minDistance = minDistance;
		m_maxVolts = maxVoltage;
		m_units = units;
}

Maxbotix::Maxbotix(UINT32 sensorChannel, float maxVoltage, 
		float minDistance, DistanceUnit units)
{
	// default to module 1
	Maxbotix(1, sensorChannel, maxVoltage, minDistance, units);
}

Maxbotix::Maxbotix(UINT8 sensorModule, UINT32 sensorChannel, float maxVoltage, 
		float minDistance, DistanceUnit units)
{
	AnalogModule *module;

	m_channel = new AnalogChannel(sensorModule, sensorChannel);
	module = m_channel->GetModule();
	module->SetSampleRate(m_sampleRate);
	m_channel->SetAverageBits(m_averageBits);
	m_voltsPerCM = maxVoltage / 1024.0;
	m_minDistance = minDistance;
	m_minVoltage = minDistance * m_voltsPerCM;
	m_maxVolts = maxVoltage;
	m_units = units;
}

float
Maxbotix::GetVoltage() 
{
	return m_channel->GetAverageVoltage();
}

float
Maxbotix::GetRangeInInches() 
{
	float dist;
	
	dist = GetRangeInCM();
	if(dist < 0)
		return dist;
	return dist * m_cm2in;
}

float
Maxbotix::GetRangeInCM()
{
	float volts;

	volts = m_channel->GetAverageVoltage();
	if(volts < m_minVoltage)
		return -1.0;
	return (volts / m_voltsPerCM);
}

void 
Maxbotix::SetDistanceUnits(DistanceUnit units)
{
	switch(units) {
		case Maxbotix::kInches:
		case Maxbotix::kCentimeters:
			m_units = units;
			break;
		default:
			break;
	}
}

Maxbotix::DistanceUnit 
Maxbotix::GetDistanceUnits()
{
	return m_units;
}

double 
Maxbotix::PIDGet()
{
	switch(m_units) 
	{
		case Maxbotix::kInches:
			return GetRangeInInches();
		case Maxbotix::kCentimeters:
			return GetRangeInCM();
		default:
			return 0.0;
	}
}
