#include "Exceptions.hpp"

const char* exceptionErrno::what() const throw()
{
	Logger::putMsg(std::string("Exception: ") + strerror(errno), FILE_ERR, ERR);
	return (strerror(errno));
}

const char* exceptionGetAddrInfo::what() const throw()
{
	Logger::putMsg(std::string("getaddrinfo Failed\n") + std::string(gai_strerror(Server::getMain().stts)), FILE_ERR, ERR);
	return ("Exception: getaddrinfo failed. Read logs.\n");
}

const char* noMainStructException::what() const throw()
{
	Logger::putMsg("Exception: try to get struct main, when mainptr is null", FILE_ERR, ERR);
	return ("Exception: getMain failed. Read logs.\n");
}