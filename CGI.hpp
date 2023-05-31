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
    CGI();
    ~CGI();
    static void     CGIFailed(Server &src);
    static void     childCGI(Server &src, int fdIn, int fdOut);
    static void     parentCGI(Server &src, int fdIn, int fdOut);
    static char**   clearEnv(char **env);
public:
    static void     startCGI(Server &src);
};

#endif