#include "Exceptions.hpp"

const char* exceptionErrno::what() const throw()
{
	Logger::putMsg(std::string("Exception: ") + strerror(errno), FILE_ERR, ERR);
	return (strerror(errno));
}

const char* exceptionGetAddrInfo::what() const throw()
{
	return ("Exception: getaddrinfo failed. Read logs.\n");
}

const char* noMainStructException::what() const throw()
{
	Logger::putMsg("Exception: try to get struct main, when mainptr is null", FILE_ERR, ERR);
	return ("Exception: getMain failed. Read logs.\n");
}

const char* serversAlreadyExists::what() const throw()
{
	return ("Exception: try to create more than one obj of Servers.");
}

const char* noSuchConnection::what() const throw()
{
	return ("Exception: No connection with fd: ");
}

const char* cannotOpenConfigFile::what() const throw()
{
	return ("Exception: can't open config file");
}

const char* notEnoughFds::what() const throw()
{
	return ("Exception: fds is empty");
}