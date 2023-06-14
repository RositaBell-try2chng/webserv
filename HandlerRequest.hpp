#ifndef HANDLERREQUEST_HPP
# define HANDLERREQUEST_HPP

#include "webserv.hpp"
#include "Server.hpp"

class HandlerRequest
{
private:
	HandlerRequest();
	~HandlerRequest();

	//CGIs
	static void prepareToSendCGI(Server &srv);
	static void startCGI(Server &srv);
	static void CGIHandler(Server &srv);
public:
	static void	mainHandler(Server &srv);
}

#endif