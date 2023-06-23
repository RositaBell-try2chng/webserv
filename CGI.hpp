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
    int     ParentCGI();
    void    ChildCGI(Server &src);
	int		waitingCGI();

    //inits
    int     CGIsFailed();
    int     startCGI();

    //communicate
    int     sendToPipe(std::map<int, Server *>::iterator &it, fd_set *writes, bool flgLast);
    int     readFromPipe(std::map<int, Server *>::iterator &it, fd_set *reads);

    char            **setEnv(Server &src);
    char            **setArgv(Server &src);
    static char     *getAllocatedCharPointer(std::string src);
    
    //getters
    int         getPipeInForward();
    int         getPipeOutForward();
    int         getPipeInBack();
    int         getPipeOutBack();

    int checkCntTrying(char c, int stage);
	int checkTimeout();

	void	clearCGI();

    std::string PATH_INFO;
    std::string PATH_TRANSLATED;
    std::string SCRIPT_NAME;

	int  prevStage;
};

#endif