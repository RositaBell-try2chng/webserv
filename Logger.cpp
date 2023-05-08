#include "Logger.hpp"

Logger::Logger() {};
Logger::~Logger() {};

int Logger::putErrMsg(const std::string &msg)
{
	std::ofstream	out;

	out.open(ERR_LOGS, std::ios::app);
	if (out.is_open())
	{
		out << "==========================\n";
		out << Logger::getTime() << msg << std::endl;
		out << "==========================\n";
		out.close();
	}
	else
		std::cerr << "can't write error in " << ERR_LOGS << ".\nError text: " << msg << std::endl;
	Logger::putInfoMsg(msg, true);
	return (1);
}

void Logger::putInfoMsg(const std::string &msg, bool flgError)
{
	std::ofstream	out;

	out.open(INFO_LOGS, std::ios::app);
	if (out.is_open())
	{
		out << "==========================\n";
		out << Logger::getTime() << (flgError ? "ERROR: " : "INFO: ")  << "\n" << msg << std::endl;
		out << "==========================\n";
		out.close();
	}
	else
		std::cerr << "can't write message in " << INFO_LOGS << ".\nmessage text: " << msg << std::endl;
}

void Logger::putQuery(t_use &use)
{
	std::ofstream	out;
	std::string		msg(use.buf, use.resRecv);

	std::cout << "getQuery, check logs\n";
	out.open(QUERIES, std::ios::app);
	if (out.is_open())
	{
		out << "==========================\n";
		out << Logger::getTime() << "\nQUERY IS:\n" << msg << std::endl;
		out << "==========================\n";
		out.close();
	}
	else
		std::cerr << "can't write query in " << QUERIES << ".\nquery text: " << msg << std::endl;
}

std::string Logger::getTime()
{
	time_t		now;
	now = time(0);

	std::string	res(ctime(&now));
	return (res);
}