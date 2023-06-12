#ifndef CGI_HPP
# define CGI_HPP

#include "webserv.hpp"
#include "MainClass.hpp"
#include "Server.hpp"
#include "Servers.hpp"
#include "Logger.hpp"

class CGI
{
private:
    int PipeInForward;
    int PipeOutForward;
    int PipeInBack;
    int PipeOutBack;

	timeval	timeCGIStarted;

    char cntTryingReading;
    char cntTryingWriting;

    pid_t   pid;
public:
    CGI();
    ~CGI();

	int		ForkCGI(Server &src);
    int     ParentCGI(Server &src);
    void    ChildCGI(Server &src);
	int		waitingCGI();

    //inits
    int     CGIsFailed();
    int     startCGI();

    //communicate
    int     sendToPipe(std::string &src);
    int     readFromPipe(Server &thisServer);

    char            **setEnv(Server &src, std::string &PATH_INFO, std::string &PATH_TRANSLATED, std::string &SCRIPT_NAME);
    char            **setArgv(Server &src, std::string &PATH_INFO, std::string &PATH_TRANSLATED, std::string &SCRIPT_NAME);
    static char     *getAllocatedCharPointer(std::string const src);
    
    //getters
    int         getPipeInForward();
    int         getPipeOutForward();
    int         getPipeInBack();
    int         getPipeOutBack();

    int checkCntTrying(char c, int stage);
};

#endif