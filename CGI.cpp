#include "CGI.hpp"

void CGI::startCGI(Server &src)
{
    int fds[2];
    //fix me need?
//    static char **cgiEnv;

//    if (!cgiEnv)
//        cgiEnv = CGI::clearEnv();
    if (pipe(fds) != 0)
    {
        CGI::CGIsFailed(src);
        return;
    }
    //push some into pipe fd[1];
    src->setChPid(fork());
    switch (src->getChPid())
    {
        case -1: {CGI::CGIsFailed(src); return;}
        case 0: {CGI::parentCGI(src, fds[0], fds[1]); break;}
        default: {CGI::childCGI(src, fds[0], fds[1]); break;}
    }
}

void CGI::childCGI(Server &src, int fdIn, int fdOut)
{
    if (!dup2(0, fdIn) || !dup2())
}

void CGI::CGIsFailed(Server &src)
{
    HTTP_Answer res;

    res.version = std::string("HTTP/1.1");
    res.status_code = std::string("500");
    res.reason_phrase = std::string("Internal server error");
    res.headers.insert(std::pair<std::string, std::string>(std::string("Content-Length"), std::string("18")));
    res.headers.insert(std::pair<std::string, std::string>(std::string("Connection"), std::string("close")));
    res.body = std::string("CGI launch Failed!");
    Logger::putMsg("CGI failed:\n" + strerror(errno), FILE_ERR, ERR);
    src->setResponse(HTTP_Answer::ft_answtostr(res));
}

//fix me need???
//char** CGI::clearEnv(char **env)
//{
//    for (int i = 0; env[i] != NULL; i++)
//        delete env[i];
//    delete env[i];
//    delete env;
//    return (NULL);
//}