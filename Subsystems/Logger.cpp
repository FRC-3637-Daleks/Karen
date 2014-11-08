#include "WPILib.h"
#include "Logger.h"
#include "sockLib.h"
#include "inetLib.h"
#include "stdioLib.h"
#include "strLib.h"

const char* SERVICE::text[] = {
		"GENERAL", "POWER", "SENSORS", "MOTORS", "PNEUMATICS"
};

const char* LEVEL::text[] = {
		"EMER", "ALERT", "CRIT", "ERR", "WARN", "NOTICE", "INFO", "DEBUG"
};

Logger* Logger::m_logger = NULL;

Logger::Logger(const char * addr, const unsigned short port) {
	m_addr = addr;
	m_port = port;
	m_socket = 0;
}

Logger::~Logger() {
	if (m_socket != 0)
		close(m_socket);
	return;
}

void Logger::sendPacket(char * data) {
	struct sockaddr_in serverAddr;
	memset((char *) &serverAddr, 0, sizeof(sockaddr_in));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, m_addr, &(serverAddr.sin_addr));
	serverAddr.sin_port = htons(m_port);

	if (m_socket == 0) {
		if ((m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
			perror("socket");
			return;
		}
	}

	if (sendto(m_socket, data, strlen(data), MSG_DONTWAIT,
			(struct sockaddr *) &serverAddr, sizeof(sockaddr_in)) < 0) {
		perror("sendto");
	}
}

void Logger::Log(int service, int level, const char * msg) {
	char data[1024];
	int code = (service << 3) + level;
	snprintf(data, 1024, "<%d>%s %s %s", code, LOG_HOST, SERVICE::text[service], msg);

	sendPacket(data);
}
