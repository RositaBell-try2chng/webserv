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

class MainClass
{
private:
    MainClass();
    ~MainClass();

    static int      maxFd;
    static Servers* allServers;
    static void     mainLoop();
    static bool     acceptConnections(fd_set *readFd);
    static void     readRequests(std::map<int, Server*>::iterator &it);
    static void     sendResponse(std::map<int, Server*>::iterator &it);
    static bool     checkCont(std::map<int, Server *>::iterator &it);
    static void     handleRequest(std::map<int, Server *>::iterator &it);
    static void     closeConnection(std::map<int, Server *>::iterator &it);
public:
    static char             **envCGI;
    static std::set<int>    readsCGI;
    static std::set<int>    writeCGI;
    static void     doIt(int args, char** argv, char** env);
    static void     exitHandler(int sig);
};

#endif