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

    std::string PATH_INFO;
    std::string PATH_TRANSLATED;
    std::string SCRIPT_NAME;

    int  prevStage; //0 - start CGI
                    //1 - send to pipe
                    //2 - last send to pipe
                    //20 - not all sent
                    //3 - fork + launch script
                    //4 - wait end of pid
                    //5 - read from pipe first
                    //50 - read from pipe next chunk
                    //6 - end of pipe riched + clean all
                    //9 - CGI failed + clean
};

#endif