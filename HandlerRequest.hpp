#ifndef HANDLERREQUEST_HPP
# define HANDLERREQUEST_HPP

#include "webserv.hpp"
#include "Server.hpp"

class HandlerRequest
{
private:
	HandlerRequest();
	~HandlerRequest();
public:
	static void prepareToSend(std::map<int, Server*>::iterator it, int Stage);
	static void mainHandler(Server *srv, int Stage);
}

#endif