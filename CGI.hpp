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

    pid_t   pid;
public:
    CGI();
    ~CGI();

    int     ParentCGI(Server &src);
    void    ChildCGI(Server &src);

    //inits
    int    CGIFailed();
    int    startCGI();
    

    char **setEnv(Server &src, std::string &PATH_INFO, std::string &PATH_TRANSLATED, std::string &SCRIPT_NAME);
    char **setArgv(Server &src, std::string &PATH_INFO, std::string &PATH_TRANSLATED, std::string &SCRIPT_NAME);
    //getters
    int     getPipeInForward();
    int     getPipeOutForward();
    int     getPipeInBack());
    int     getPipeOutBack());
};

#endif