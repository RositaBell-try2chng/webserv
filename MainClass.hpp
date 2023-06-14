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

class MainClass
{
private:
    MainClass();
    ~MainClass();

    static int      maxFd;
    static Servers* allServers;

	static void     mainLoop();
    static bool     acceptConnections(fd_set *readFd);
    static void     readRequest(std::map<int, Server*>::iterator &it, int Stage, fd_set *reads);
    static void     readNextChunk(std::map<int, Server*>::iterator &it);
    static void     sendResponse(std::map<int, Server*>::iterator &it, fd_set *writes);
    static void     handleRequest(std::map<int, Server *>::iterator &it);
    static void     closeConnection(std::map<int, Server *>::iterator &it);
    static void     firstSend(std::map<int, Server *>::iterator &it);
    static void     CGIHandlerReadWrite(std::map<int, Server *>::iterator &it, fd_set *reads, fd_set *writes);
public:
    static char             **envCGI;
    static std::set<int>    readsCGI;
    static std::set<int>    writeCGI;
    static void     doIt(int args, char** argv, char** env);
    static void     exitHandler(int sig);
    static void     addToSet(int fd, fd_set *dst);
    static void     prepareToSend(Server *src, int Stage);
};

#endif