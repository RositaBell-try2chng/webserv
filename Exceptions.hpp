#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include "webserv.hpp"
# include "Logger.hpp"
# include "Server.hpp"

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

class serversAlreadyExists : public std::exception
{
public:
	const char *what() const throw();
};

class noSuchConnection : public std::exception
{
public:
	const char *what() const throw();
};

class noMainStructException : public std::exception
{
public:
	const char *what() const throw();
};

class exceptionWrongRequest : public std::exception
{
public:
	const char *what() const throw();
};

class cannotOpenConfigFile : public std::exception
{
public:
	const char *what() const throw();
};

class notEnoughFds : public std::exception
{
public:
	const char *what() const throw();
};

class badConfig : public std::exception
{
public:
	const char *what() const throw();
};

class badAlloc : public std::exception
{
public:
	const char *what() const throw();
};
#endif