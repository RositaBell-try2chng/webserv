#ifndef HANDLERREQUEST_HPP
# define HANDLERREQUEST_HPP

#include "webserv.cpp"

class HandlerRequest
{
private:
	HandlerRequest();
	~HandlerRequest();
public:
	prepareToSend(std::map<int, Server*>::iterator it, int Stage);
}

#endif