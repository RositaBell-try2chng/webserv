#ifndef HANDLERREQUEST_HPP
# define HANDLERREQUEST_HPP

# include "webserv.hpp"
# include "Server.hpp"
# include "dir_listing/DirListing.hpp"

typedef struct s_serv t_serv;
typedef struct s_loc t_loc;

class HandlerRequest
{
private:
	HandlerRequest();
	~HandlerRequest();

	static void	start(Server &srv);
	static void	parserRequest(Server &srv);
	static void	handleRequest(Server &srv);
	static void	prepareToSendError(Server &srv);
	static void	GET(Server &srv, t_serv *servNode, std::string &fileName, bool CGIflg);
	static void	POST(Server &srv, t_serv *servNode, t_loc *locNode, std::string &fileName);
	static void	DELETE(Server &srv, t_serv *servNode, t_loc *locNode, std::string &fileName);
	static void	CGIerrorManager(Server &srv);
	static bool	isBodyNeed(Server &srv);
	static void	handleDirectoryResponse(Server &srv, std::string realPath, t_loc *locNode);
	static char changeBodyIfBoundryIsSet(HTTP_Request *req);
	static void addFileNameEnv(HTTP_Request *req, char result);
	//CGIs
	static void	checkReadyToHandle(Server &srv);
	static void	prepareToSendCGI(Server &srv);
	static void	startCGI(Server &srv);
	static void	CGIHandler(Server &srv);
	static void	redirectResponse(Server &srv, t_loc *locNode);
	static bool	haveErrorPage(Server &srv, t_serv *servNode, int code);
	
public:
	static void	mainHandler(Server &srv);
};

#endif