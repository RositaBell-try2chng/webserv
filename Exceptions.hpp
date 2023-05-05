#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include "webserv.hpp"
# include "Logger.hpp"

class exceptionErrno : public std::exception
{
public:
	const char *what() const throw();
};

class exceptionGetAddrInfo : public std::exception
{
public:
	const char *what() const throw();
};

#endif