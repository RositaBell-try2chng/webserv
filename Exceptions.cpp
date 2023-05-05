#include "Exceptions.hpp"

const char* exceptionErrno::what() const throw()
{
	Logger::putErrMsg(std::string("Exception: ") + strerror(errno));
	return (strerror(errno));
}

const char* exceptionGetAddrInfo::what() const throw()
{
	return ("Exception: getaddrinfo failed. Read logs.\n");
}