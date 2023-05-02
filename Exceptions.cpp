#include "Exceptions.hpp"

const char* exceptionSocket::what() const throw()
{
	return ("socket failed");
}

const char* exceptionBind::what() const throw()
{
	return (strerror(errno));
}