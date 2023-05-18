#ifndef MAINCLASS_HPP
# define MAINCLASS_HPP

# include "webserv.hpp"
# include "Logger.hpp"
# include "Servers.hpp"
# include "Server.hpp"
# include "Exceptions.hpp"
# include "ConfParser.hpp"

# define BUF_SIZE 4096

#define DEF_RESPONSE "HTTP/1.1 200 OK"

class MainClass
{
private:
    MainClass();
    ~MainClass();

    static int      stage;
    static int      maxFd;
    static Servers* allServers;
    static void     mainLoop();
    static void     acceptConnections(std::map<int, Server*>::iterator &it);
    static void     readRequests(std::map<int, Server*>::iterator &it, fd_set *wFds);
    static void     sendRequests(std::map<int, Server*>::iterator &it, fd_set *wFds);
    static bool     checkCont(std::map<int, Server *>::iterator &it, fd_set *wFds);
    static void     handleRequest(std::map<int, Server *>::iterator &it, fd_set *wFds);
public:
    static void     doIt(int args, char** argv);
    static void     exitHandler(int sig);
};

#endif