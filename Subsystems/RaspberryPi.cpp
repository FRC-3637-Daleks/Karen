#include "WPILib.h"
#include "RaspberryPi.h"
#include "sockLib.h"
#include "inetLib.h"
#include "stdioLib.h"
#include "strLib.h"
#include "ioLib.h"
#include "fioLib.h"

#define BAD_DISTANCE        -1.0

typedef struct clientInfo {
    char message[1024];
} clientInfo_t;

int
udpServer(RaspberryPi *p)
{
    struct sockaddr_in serverAddr; /* server's socket address */
    struct sockaddr_in clientAddr; /* client's socket address */
    clientInfo_t clientMsg;        /* message from client     */
    int sockAddrSize;              /* size of socket address structure */
    int sFd;                       /* socket file descriptor */
    char inetAddr[INET_ADDR_LEN];

    sockAddrSize = sizeof(struct sockaddr_in);
    bzero((char *)&serverAddr, sockAddrSize);
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_port        = htons(p->getPort());
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
    if((sFd = socket(AF_INET, SOCK_DGRAM, 0)) == ERROR) {
        perror("socket");
        return ERROR;
    }

    if(bind(sFd, (struct sockaddr *)&serverAddr, sockAddrSize) == ERROR) {
        perror("bind");
        close(sFd);
        return ERROR;
    }
    
    for(;;) {
        if(recvfrom(sFd, (char *)&clientMsg, sizeof(clientMsg), 0,
                (struct sockaddr *)&clientAddr, &sockAddrSize) == ERROR) {
            perror("recvfrom");
            close(sFd);
            return ERROR;
        }
        inet_ntoa_b(clientAddr.sin_addr, inetAddr);
        printf("MSG FROM CLIENT (Internet Address %s, port %d):\n%s\n",
                inetAddr, ntohs (clientAddr.sin_port), clientMsg.message);

        // parse message and setting appropriate values based on message
        p->parseMsg(clientMsg.message);
    }
}

RaspberryPi::RaspberryPi(ushort port)
{
    m_invalid  = true;
    m_isHot    = false;
    m_distance = BAD_DISTANCE;
    m_port     = port;
    m_task = new Task("udpPiInfo", (FUNCPTR)udpServer, RASPBERRY_PRIORITY);
    m_task->Start((UINT32)this);
}

RaspberryPi::~RaspberryPi()
{
    m_task->Stop();
    delete m_task;
    return;
}

bool
RaspberryPi::isValid()
{
    return (!m_invalid);
}

bool
RaspberryPi::isTargetHot()
{
    if(isValid())
        return m_isHot;
    return false;
}

float
RaspberryPi::targetDistance()
{
    if(isValid())
        return m_distance;
    return BAD_DISTANCE;
}

ushort
RaspberryPi::getPort()
{
    return m_port;
}

void
RaspberryPi::setHot(bool state)
{
    m_isHot = state;
}

RaspberryPi::setDistance(float dist)
{
    m_distance = dist;
}

void
RaspberryPi::markInvalid()
{
    m_invalid = true;
}

void
RaspberryPi::markValid()
{
    m_invalid = false;
}

// parse client message of the following form:
// HOT=<true|false> DIST=<truncated(distance * 100)>

void
RaspberryPi::parseMsg(char *msg)
{
    unsigned long d;
    float distance;
    char *p;

    // mark state invalid till we finish parsing the message
    markInvalid();
    p = msg;
    // extract is hot info
    if(strncmp(p, "HOT=", 4) != 0) {
        printf("Missing HOT= token\n");
        return;
    }

    p += 4; 
    if(strncmp(p, "true", 4) == 0) {
        setHot(true);
        p += 5;
    }
    else if(strncmp(p, "false", 5) == 0) {
        setHot(false);
        p += 6;
    }
    else {
        printf("garbled message from client: %s\n", p);
        return;
    }

    // extract distance info
    if(strncmp(p, "DIST=", 5) != 0) {
        printf("Missing DIST= token\n");
        return;
    }
    p += 5;
    d = strtoul(p, NULL, 10);
    distance = (float)d / 100.0;
    setDistance(distance);

    markValid();
    return;
}
