#include "CGI.hpp"

CGI::CGI() 
{
    this->pid = 0;
    this->PipeInForward = 0;
    this->PipeOutForward = 0;
    this->PipeInBack = 0;
    this->PipeOutBack = 0;
}

CGI::~CGI()
{
    if (this->pid != 0)
        kill(pid);
    if (this->PipeInForward != 0)
        close(this->PipeInForward);
    if (this->PipeOutForward != 0)
        close(this->PipeOutForward);
    if (this->PipeInBack != 0)
        close(this->PipeInBack);
    if (this->PipeOutBack != 0)
        close(this->PipeOutBack);
}

int CGI::startCGI(Server &src)
{
    int         fdsForward[2];
    int         fdsBack[2];
    
    if (pipe(fdsForward) == -1 || fcntl(fdsForward[0], F_SETFL, O_NONBLOCK) == -1)// || fcntl(fdsForward[1], F_SETFL, O_NONBLOCK) == -1) fix me: need?
        return (this->CGIsFailed());
    this->PipeInForward = fdsForward[0];
    this->PipeOutForward = fdsForward[1];
    if (pipe(fdsBack) == -1 /*|| fcntl(fdsBack[0], F_SETFL, O_NONBLOCK) == -1*/ || fcntl(fdsBack[1], F_SETFL, O_NONBLOCK) == -1)
        return (this->CGIsFailed());
    this->PipeInBack = fdsBack[0];
    this->PipeOutBack = fdsBack[1];
    return (20);
}

int CGI::ForkCGI(Server &src)
{
    this->pid = fork();
    switch (this->pid)
    {
        case -1: { return(this->CGIFailed()); }
        case 0: { return(this->ParentCGI(src)); }
        default: { (ChildCGI(src); }
    }
    return (29); // -1/0 - return own int, Child exit before //fix me: implement
}

int CGI::ParentCGI()
{
    int stts;
    
    switch (waitpid(this->pid, &stts, WHOHANG))
    {
        case -1: { return(this->CGIFailed()); } //error
        case 0: { return this->checkTimout(); } //child not ready //fix me: implement
        case this->pid: { 
            if (stts == 0) //child finished ok
                return (23); 
            return (29); //child finished bad
        }
        default: { break; } 
    }
    return this->checkTimout(); //wrong pid returned, need to wait correct pid //fix me: implement
}

int CGI::ChildCGI(Server &src)
{
    char **env;
    char **argv;
    std::string PATH_INFO; //virtual path
    std::string PATH_TRANSLATED; //real path
    std::string SCRIPT_NAME;

    //change STDIN and STDOUT
    if (!dup2(this->PipeInForward, STDIN_FILENO) || !dup2(this->PipeOutBack, STDOUT_FILENO))
    {
        Logger::putMsg("dup2 failed:\n" + std::string(strerror(errno)));
        exit(1);
    }
    close(this->PipeInBack);
    close(this->PipeOutForward);
    try {
        argv = this->setArgv(src, PATH_INFO, PATH_TRANSLATED, SCRIPT_NAME); //fix me: implement
        env = this->setEnv(src, PATH_INFO, PATH_TRANSLATED, SCRIPT_NAME);
    }
    catch {
        Logger::putMsg(std::string("env/argv sets failed! in child", FILE_ERR, ERR);
        exit(1);
    }
    execve(argv[0], argv, env);
    exit(1);
}


char** CGI::setArgv(Server &src, std::string &PATH_INFO, std::string &PATH_TRANSLATED, std::string &SCRIPT_NAME)
{
    PATH_INFO = src.getReq_struct().uri;
    std::string loc;
    std::string::size_type i;
    //find server
    t_serv *curServ = src.findServer(src.getReq_struct().host);
    //find location
    i = PATH_INFO.rfind('/');
    loc = PATH_INFO.substr(0, i + 1);
    t_loc *curLoc = src.findLocation(loc, curServ);
    SCRIPT_NAME = Server::findFile(PATH_INFO.substr(i + 1, PATH_INFO.length() - i));
    PATH_TRANSLATED = curServ->root + loc + curLoc->root + SCRIPT_NAME;
}

char**  CGI::setEnv(Server &src, std::string &PATH_INFO, std::string &PATH_TRANSLATED, std::string &SCRIPT_NAME)
{
    char **res;
    size_t i;
    size_t size = src->getAnsw_struct().headers.size() + STANDART_ENV_VARS_CNT + 1;
    std::map<std::string, std::string>::iterator it = src.getAnsw_struct().headers.begin();

    res = new char**[size];
    res[0] = strdup("SERVER_SOFTWARE=AMANIX");
    res[1] = strdup((std::string("SERVER_NAME=") + src.getReq_struct().host).c_str());
    res[2] = strdup("GATEWAY_INTERFACE=CGI/1.1");
    res[3] = strdup("SERVER_PROTOCOL=HTTP/1.1");
    res[4] = strdup((std::string("SERVER_PORT=") + src.getReq_struct().port).c_str());
    res[5] = strdup((std::string("REQUEST_METHOD=") + src.getReq_struct().method).c_str());
    res[6] = strdup((std::string("SCRIPT_NAME=") + SCRIPT_NAME).c_str()); //fix me: need script name
    res[7] = strdup("REMOTE_ADDR=");
    res[8] = strdup((std::string("PATH_INFO=") + PATH_INFO).c_str());
    res[8] = strdup((std::string("PATH_TRANSLATED=") + PATH_TRANSLATED).c_str());
    for (i = 0; i < 8; i++)
        if (!res[i])
            throw badAlloc();
    res[size] = NULL;
    for (; i < size; i++)
    {
        res[i] = strdup((it->first + std::string("=") + it->second).c_str());
        it++;
    }
    return res;
}

 int    CGI::CGIsFailed()
{
    HTTP_Answer res;

    if (PipeInForward > 0)
        close(PipeInForward);
    if (PipeOutForward > 0)
        close(PipeOutForward);
    if (PipeInBack > 0)
        close(PipeInBack);
    if (PipeOutBack > 0)
        close(PipeOutBack);
    Logger::putMsg(std::string("CGI failed!") + std::string(strerror(errno)), FILE_ERR, ERR);
    return (29);
}

int     getPipeInForward() { return this->PipeInForward; }
int     getPipeOutForward() { return this->PipeOutForward; }
int     getPipeInBack()) { return this->PipeInBack; }
int     getPipeOutBack()) { return this->PipeOutBack; }