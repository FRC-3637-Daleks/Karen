#ifndef __LOGGER_H__
#define __LOGGER_H__

#define DEFAULT_LOG_ADDR "10.36.37.31"
#define DEFAULT_LOG_PORT 1140

#define LOG_HOST "CRIO"

class SERVICE {
public:
	enum {GENERAL=0, POWER, SENSORS, MOTORS, PNEUMATICS};
	static const char* text[];
};

class LEVEL {
public:
	enum {EMER=0, ALERT, CRIT, ERR, WARN, NOTICE, INFO, DEBUG};
	static const char* text[];
};

class Logger {
public:
	static Logger* GetInstance() {
		if (!m_logger) {
			m_logger = new Logger();
		}
		return m_logger;
	};
	~Logger();
	void sendPacket(char * data);
	void Log(int service, int level, const char * msg);

private:
	Logger(const char * addr = DEFAULT_LOG_ADDR,
			const unsigned short port = DEFAULT_LOG_PORT);
	static Logger *m_logger;
	int m_socket;
	const char * m_addr;
	int m_port;
};

#define log( service, level, msg ) \
    Logger::GetInstance()->Log(service, level, msg); \
    printf("[%s] %s", LEVEL::text[level], msg);

#endif // __LOGGER_H__
