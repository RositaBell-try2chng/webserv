#ifndef MAINCLASS_HPP
# define MAINCLASS_HPP

# include "webserv.hpp"
# include "Logger.hpp"
# include "Servers.hpp"
# include "Server.hpp"
# include "Exceptions.hpp"
# include "ConfParser.hpp"

class MainClass
{
private:
    MainClass();
    ~MainClass();

    static int      stage;
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
    static void     doIt(int args, char** argv);
    static void     exitHandler(int sig);
};

#endif