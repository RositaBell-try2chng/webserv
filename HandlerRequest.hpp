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
	static void	checkReadyToHandle(Server &srv);
	static void prepareToSendCGI(Server &srv);
	static void startCGI(Server &srv);
	static void CGIHandler(Server &srv);
	static void redirectResponse(Server &srv, t_loc *locNode);
public:
	static void	mainHandler(Server &srv);
};

#endif