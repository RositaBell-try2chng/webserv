#ifndef MAINCLASS_HPP
# define MAINCLASS_HPP

# include "webserv.hpp"
# include "Logger.hpp"
# include "Servers.hpp"
# include "Server.hpp"
# include "Exceptions.hpp"
# include "ConfParser.hpp"
# include "request_parse/HTTP_Request.hpp"
# include "request_parse/HTTP_Answer.hpp"
# include "HandlerRequest.hpp"

typedef struct s_loc t_loc;

class MainClass
{
private:
    MainClass();
    ~MainClass();

    static int      maxFd;
    static Servers* allServers;

	static void     mainLoop();
    static bool     acceptConnections(fd_set *readFd);
    static void     readRequest(std::map<int, Server*>::iterator &it, fd_set *reads);
    static void     sendResponse(std::map<int, Server*>::iterator &it, fd_set *writes);
    static void     closeConnection(std::map<int, Server *>::iterator &it);
    static void     CGIHandlerReadWrite(std::map<int, Server *>::iterator &it, fd_set *reads, fd_set *writes);
    static int      setChunkedResponse(Server &srv);
    static int      setContentLengthResponse(Server &srv);
public:
    static void				doIt(int args, char** argv);
    static void				exitHandler(int sig);
    static void				addToSet(int fd, fd_set *dst);
	static bool				isCorrectRedirection(std::map<int, Server *>::iterator it);
	static bool				checkCorrectHostPortInRedirection(t_loc *locNode, std::string &host, std::string &port, std::string &ip, std::string &location);
    static void             setBadStageError(Server &srv);
};

#endif