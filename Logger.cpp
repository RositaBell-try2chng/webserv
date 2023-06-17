#include "Logger.hpp"

Logger::Logger() {};
Logger::~Logger() {};

//	msg			- message to log
//	filename	- name of file in dir ./logs
//	typeOfMsg	- type of message
void	Logger::putMsg(std::string const &msg, std::string const &filename, std::string const &typeOfMsg)
{
	std::ofstream	out;

	static bool     flgLogsDir;

    if (access(LOGS, W_OK) != 0)
    {
        if (flgLogsDir)
            return;
        std::cerr << "\n\nATTENTION: can't use logs because:\n" << strerror(errno) << std::endl << std::endl;
        flgLogsDir = true;
        return;
    }
	out.open((std::string(LOGS) + filename).c_str(), std::ios::app);
	if (!out.is_open())
	{
	    if (filename != std::string(DEF_FILE))
		    std::cerr << "==========================\n" << "Can't log " << typeOfMsg << " in " << filename << " in directory " << LOGS << " because:\n" << \
		strerror(errno) << std::endl << "Message:\n" << msg << std::endl << "==========================\n";
		return;
	}
	out << "==========================\n";
	out << Logger::getTime() << typeOfMsg << ":\n" << msg << std::endl;
	out << "==========================\n";
	out.close();
	if (filename == std::string(FILE_ERR))
		Logger::putMsg(msg, DEF_FILE, typeOfMsg);
}

void	Logger::putMsg(std::string const &msg, int fd, std::string const &filename, std::string const &typeOfMsg)
{
	std::ofstream	out;

	static bool     flgLogsDir;

	if (access(LOGS, W_OK) != 0)
	{
		if (flgLogsDir)
			return;
		std::cerr << "\n\nATTENTION: can't use logs because:\n" << strerror(errno) << std::endl << std::endl;
		flgLogsDir = true;
		return;
	}
	out.open((std::string(LOGS) + filename).c_str(), std::ios::app);
	if (!out.is_open())
	{
		if (filename != std::string(DEF_FILE))
			std::cerr << "==========================\n" << "Can't log " << typeOfMsg << " in " << filename << " in directory " << LOGS << " because:\n" << \
		strerror(errno) << std::endl << "Message:\n" << msg << fd << std::endl << "==========================\n";
		return;
	}
	out << "==========================\n";
	out << Logger::getTime() << typeOfMsg << ":\n" << msg << fd << std::endl;
	out << "==========================\n";
	out.close();
	if (filename == std::string(FILE_ERR))
		Logger::putMsg(msg, fd, DEF_FILE, typeOfMsg);
}

std::string Logger::getTime()
{
	time_t		now;
	now = time(0);

	std::string	res(ctime(&now));
	return (res);
}