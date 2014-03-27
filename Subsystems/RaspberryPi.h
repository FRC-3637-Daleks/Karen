#ifndef __RASPBERRYPI_H__
#define __RASPBERRYPI_H__

#include "WPILib.h"
#include "Task.h"
#include "Defines.h"

#define DEFAULT_PI_PORT 1130

class RaspberryPi
{
public:
    RaspberryPi(ushort port=DEFAULT_PI_PORT);
    ~RaspberryPi();
    bool isValid();
    bool isTargetHot();
    float targetDistance();
    ushort getPort();
    void parseMsg(char *); 

private:
    void markInvalid();
    void markValid();
    void setHot(bool);
    void setDistance(float);

    ushort  m_port;
    bool    m_invalid;
    bool    m_isHot;
    float   m_distance;
    Task    *m_task;
};
#endif // __RASPBERRYPI_H__
