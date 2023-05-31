#include "CGI.hpp"

void CGI::startCGI(Server &src)
{
    int         fdsBack[2];
    int         fdsForward[2];
    
    if (pipe(fdsForward) == -1)
    {
        CGI::CGIsFailed(src, -1, -1, -1, -1);
        return;
    }
    if (pipe(fdsBack) == -1)
    {
        CGI::CGIsFailed(src, fdsForward[0], fdsForward[1], -1, -1);
        return;
    }
    //push some into pipe
    src->setChPid(fork());
    switch (src->getChPid())
    {
        case -1: {CGI::CGIsFailed(src, fdsForward[0], fdsForward[1], fdsBack[0], fdsBack[1]); return;}
        case 0: {CGI::parentCGI(src, fdsForward, fdsForward); break;}
        default: {CGI::childCGI(src, fdsForward, fdsForward, pathCGI); break;}
    }
}

void CGI::childCGI(Server &src, int *fdsForward, int *fdsBack)
{
    static char* path[2];
    static char* envCGI[1];
    int res;

    if (CGI::path[0])
    {
        delete CGI::path[0];
        CGI::path[0] = NULL;
    }
    close(fdsForward[1]);
    close(fdsBack[0]);
    if (!dup2(fdsForward[0], STDIN_FILENO) || !dup2(fdsBack[1], STDOUT_FILENO))
    {
        Logger::putMsg("dup2 failed:\n" + std::string(strerror(errno)));
        exit(1);
    }
    close(fdsForward[0]);
    close(fdsBack[1]);
    path[0] = CGI::setPath(src);//implement
    res = execve(path[0], path, envCGI);
    CGI::CGIFailed(src, -1, -1, -1, -1);
    exit(res);
}

void    CGI::CGIsFailed(Server &src, int fd1, int fd2, int fd3, int fd4)
{
    HTTP_Answer res;

    if (fd1 >= 0)
        close(fd1);
    if (fd2 >= 0)
        close(fd2);
    if (fd3 >= 0)
        close(fd3);
    if (fd4 >= 0)
        close(fd4);
    res.version = std::string("HTTP/1.1");
    res.status_code = std::string("500");
    res.reason_phrase = std::string("Internal server error");
    res.headers.insert(std::pair<std::string, std::string>(std::string("Content-Length"), std::string("18")));
    res.headers.insert(std::pair<std::string, std::string>(std::string("Connection"), std::string("close")));
    res.body = std::string("CGI launch Failed!");
    Logger::putMsg("CGI failed:\n" + strerror(errno), FILE_ERR, ERR);
    src->setResponse(HTTP_Answer::ft_answtostr(res));
}

char*    CGI::setPath(Server &src)
{
    t_serv  *curServ = src.serv;
    t_serv  *prevServ;
    t_loc   *curLoc;
    t_loc   *prevLoc;
    std::map<std::string, std::string>::iterator it;
    std::string line;
    std::string RealLoc;
    std::string::size_type  i;
    
    //find correct t_serv for request
    it = src.getReq_struct().headers().find("Host");
    if (!(it == src.getReq_struct().headers().end()))
    {
        i = it->second.find(':');
        if (i == std::string::npos)
            line = it->second;
        else
            line = it->second.substr(0, i);
        if (!ConfParser::checkCorrectHost(line))
        {
            while (curServ)
            {
                if (line == curServ->ServerName)
                    break;
                prevServ = curServ;
                curServ = curServ->next;
            }
            if (!curServ)
                curServ = src.serv;
            else
            {
                prevServ->next = curServ->next;
            }
            RealLoc = cur->serv.root;
        }
    }
    //find correct location
    i = src.getReq_struct().uri.rfind('/');
    line = src.getReq_struct().uri.substr(0, i + 1);
    curLoc = curServ->locList;
    while (curLoc)
    {
        if (curLoc->location == line)
            break;
        curLoc = curLoc->next;
    }
    if (!curLoc)
        curLoc = curServ->locList;
    else
    {

    }
    if (curLoc->location != "/")
        RealLoc += curLoc->location;
    
    //setpath
}