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
public:
    static void     doIt(int args, char** argv);
    static void     exitHandler(int sig);
};

#endif