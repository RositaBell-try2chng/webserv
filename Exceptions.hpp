#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

#include <iostream>

class exceptionSocket : public std::exception
{
public:
	const char *what() const throw();
};

class exceptionBind : public std::exception
{
public:
	const char *what() const throw();
};

#endif