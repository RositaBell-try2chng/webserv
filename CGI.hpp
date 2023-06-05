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
    static void     childCGI(Server &src, int *fdsForward, int *fdsBack);
    static void     parentCGI(Server &src, int *fdsForward, int *fdsBack);

    static char*    setPath(Server &src);
    static void     CGIsFailed(Server &src, int fd1, int fd2, int fd3, int fd4);
public:
    CGI();
    ~CGI();
    static void     startCGI(Server &src);
};

#endif