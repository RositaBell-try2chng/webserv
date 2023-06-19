#include "CGI.hpp"

CGI::CGI() 
{
    this->pid = 0;
    this->PipeInForward = 0;
    this->PipeOutForward = 0;
    this->PipeInBack = 0;
    this->PipeOutBack = 0;
    this->prevStage = -1;
}

CGI::~CGI()
{
    if (this->pid != 0)
        kill(pid, SIGTERM);
    if (this->PipeInForward != 0)
        close(this->PipeInForward);
    if (this->PipeOutForward != 0)
        close(this->PipeOutForward);
    if (this->PipeInBack != 0)
        close(this->PipeInBack);
    if (this->PipeOutBack != 0)
        close(this->PipeOutBack);
}

int CGI::startCGI()
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
    return (1);
}

int CGI::ForkCGI(Server &src)
{
    this->pid = fork();
    switch (this->pid)
    {
        case -1: { return(this->CGIsFailed()); }
        case 0: { return(this->ParentCGI()); }
        default: { ChildCGI(src); }
    }
    return (this->CGIsFailed()); // -1/0 - return own int, Child exit before
}

int	CGI::ParentCGI()
{
	this->timeCGIStarted.tv_sec = time(NULL);
	this->timeCGIStarted.tv_usec = 0;
	return (4);
}

int CGI::waitingCGI()
{
    int		stts;
	pid_t	resPid;

	resPid = waitpid(this->pid, &stts, WNOHANG);
    switch (resPid)
    {
        case -1: { return(this->CGIsFailed()); } //error
        case 0: { return (this->checkTimeout()); } //child not ready
        default:
		{
			if (resPid == this->pid && stts == 0) //child finished ok
				return (5);
			else if (resPid == this->pid && stts != 0) //child finished bad
				return (9);
		}
    }
    return (this->checkTimeout()); //wrong pid returned, need to wait correct pid
}

void	CGI::ChildCGI(Server &src)
{
    char **env = NULL;
    char **argv = NULL;
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
    //set argv/env
    argv = this->setArgv(src, PATH_INFO, PATH_TRANSLATED, SCRIPT_NAME);
    if (!argv)
        exit(1);
    env = this->setEnv(src, PATH_INFO, PATH_TRANSLATED, SCRIPT_NAME);
    if (!env)
        exit(1);
    //start script
    execve(argv[0], argv, env);
    exit(1);
}

int    CGI::CGIsFailed()
{
    if (PipeInForward > 0)
        close(PipeInForward);
    if (PipeOutForward > 0)
        close(PipeOutForward);
    if (PipeInBack > 0)
        close(PipeInBack);
    if (PipeOutBack > 0)
        close(PipeOutBack);
    Logger::putMsg(std::string("CGI failed!") + std::string(strerror(errno)), FILE_ERR, ERR);
    return (9);
}

int CGI::sendToPipe(std::map<int, Server *>::iterator &it, fd_set *writes, bool flgLast)
{
	ssize_t		wrRes;

    if (!FD_ISSET(this->PipeOutForward, writes))
        return (20); //repeat write
	wrRes = write(this->PipeOutForward, it->second->getReq_struct()->body.c_str(), it->second->getReq_struct()->body.length());
	switch (wrRes)
	{
		case -1: //error
		{
			Logger::putMsg(std::string("ERROR while writing to ") + Size_tToString(this->PipeOutForward, DEC_BASE) + std::string(":\n") + std::string(strerror(errno)), FILE_ERR, ERR);
			return (this->CGIsFailed());
		}
		case 0:
		{
			Logger::putMsg(std::string("write 0 bytes CGI") + std::string(strerror(errno)), this->PipeInBack, FILE_ERR, ERR);
			return (this->checkCntTrying('w', it->second->CGIStage));
		}
		default:
		{
            it->second->updateLastActionTime();
			this->cntTryingWriting = 0;
            if (static_cast<size_t>(wrRes) == it->second->getReq_struct()->body.length())
            {
                if (flgLast)
                    return (4);
                else
                {
                    it->second->Stage = 1;
                    return (2);
                }
            }
			it->second->getReq_struct()->body.erase(0, wrRes);
			return (20); //repeat write
		}
	}
}

int CGI::readFromPipe(std::map<int, Server *>::iterator &it, fd_set *reads)
{
    if (!FD_ISSET(this->PipeInBack, reads))
        return (it->second->CGIStage);

	ssize_t		rdRes;
	char		buf[BUF_SIZE_PIPE];

	rdRes = read(this->PipeInBack, buf, BUF_SIZE_PIPE);
	switch (rdRes)
	{
		case -1: //error
		{
			Logger::putMsg(std::string("ERROR while reading from PipeInBack:\n") + std::string(strerror(errno)), FILE_ERR, ERR);
			return (this->CGIsFailed());
		}
		case 0:
		{
			Logger::putMsg(std::string("read 0 bytes from PipeInBack:\n") + std::string(strerror(errno)), FILE_ERR, ERR);
			return (this->checkCntTrying('r', it->second->CGIStage));
		}
		default: //maybe somthing else in PIPE
		{
            it->second->updateLastActionTime();
			this->cntTryingReading = 0;
			it->second->setResponse(std::string(buf, rdRes));
            it->second->Stage = 5;
            if (rdRes == BUF_SIZE)
			    return (50);
            else
                return (6);
		}
	}
}

//checker counters
int CGI::checkCntTrying(char c, int stage)
{
	char *checks;

    if (c == 'r')
        checks = &this->cntTryingReading;
    else
        checks = &this->cntTryingWriting;
	++(*checks);
	if (*checks < CNT_TRYING)
		return (stage);
	Logger::putMsg(std::string("read/write 0 bytes 3 times in a raw: ") + std::string(strerror(errno)), FILE_ERR, ERR);
	return (this->CGIsFailed());
}

char** CGI::setArgv(Server &src, std::string &PATH_INFO, std::string &PATH_TRANSLATED, std::string &SCRIPT_NAME)
{
    char **res = NULL;
    PATH_INFO = src.getReq_struct()->base.start_string.uri;
    std::string loc;
    std::string::size_type i;
    //find server
    t_serv *curServ = src.findServer(src.getReq_struct()->host);
    //find location
    i = PATH_INFO.rfind('/');
    loc = PATH_INFO.substr(0, i + 1);
    t_loc *curLoc = src.findLocation(loc, curServ);
    SCRIPT_NAME = PATH_INFO.substr(i + 1, PATH_INFO.length() - i);
    PATH_TRANSLATED = curServ->root + loc + curLoc->root + SCRIPT_NAME;
    try {
        res = new char*[2];
        res[0] = NULL;
        res[0] = CGI::getAllocatedCharPointer(PATH_TRANSLATED);
        res[1] = NULL;
    }
    catch (std::exception &e)
    {
        Logger::putMsg(std::string("BAD ALLOC:\n") + std::string(e.what()), FILE_ERR, ERR);
        if (res)
        {
            if (res[0])
                delete [] res[0];
            delete [] res;
        }
        res = NULL;
    }
	if (!res)
		return NULL;
    return res;
}

char**  CGI::setEnv(Server &src, std::string &PATH_INFO, std::string &PATH_TRANSLATED, std::string &SCRIPT_NAME)
{
    char **res = NULL;
    size_t i = 0;
    size_t size = src.getAnsw_struct()->headers.size() + STANDART_ENV_VARS_CNT;
    std::map<std::string, std::string>::iterator it = src.getAnsw_struct()->headers.begin();
    try
    {
        res = new char*[size + 1];
		res[i] = NULL;
        res[i++] = CGI::getAllocatedCharPointer(std::string("SERVER_SOFTWARE=AMANIX"));
		res[i] = NULL;
        res[i++] = CGI::getAllocatedCharPointer(std::string(std::string("SERVER_NAME=") + src.getReq_struct()->host));
		res[i] = NULL;
        res[i++] = CGI::getAllocatedCharPointer(std::string("GATEWAY_INTERFACE=CGI/1.1"));
		res[i] = NULL;
        res[i++] = CGI::getAllocatedCharPointer(std::string("SERVER_PROTOCOL=HTTP/1.1"));
		res[i] = NULL;
        res[i++] = CGI::getAllocatedCharPointer(std::string(std::string("SERVER_PORT=") + src.getReq_struct()->port));
		res[i] = NULL;
        res[i++] = CGI::getAllocatedCharPointer(std::string(std::string("REQUEST_METHOD=") + src.getReq_struct()->base.start_string.method));
		res[i] = NULL;
        res[i++] = CGI::getAllocatedCharPointer(std::string(std::string("SCRIPT_NAME=") + SCRIPT_NAME));
		res[i] = NULL;
        res[i++] = CGI::getAllocatedCharPointer(std::string("REMOTE_ADDR="));
		res[i] = NULL;
        res[i++] = CGI::getAllocatedCharPointer(std::string(std::string("PATH_INFO=") + PATH_INFO));
		res[i] = NULL;
        res[i++] = CGI::getAllocatedCharPointer(std::string(std::string("PATH_TRANSLATED=") + PATH_TRANSLATED));
        res[size] = NULL;
        for (; i < size; i++)
        {
            res[i] = CGI::getAllocatedCharPointer(std::string(it->first + std::string("=") + it->second));
            it++;
        }
    }
    catch (std::exception &e)
    {
        Logger::putMsg(std::string("BAD ALLOC:\n") + std::string(e.what()), FILE_ERR, ERR);
        if (res)
        {
            for (size_t j = 0; j < i; j++)
                delete [] res[j];
            delete [] res;
        }
        return NULL;
    }
    return res;
}

char    *CGI::getAllocatedCharPointer(std::string src)
{
    size_t len = src.length();
    char *res = new char[len + 1];

    for (size_t i = 0; i < len; i++)
        res[i] = src[i];
    return (res);
}

//getters
int     CGI::getPipeInForward() { return this->PipeInForward; }
int     CGI::getPipeOutForward() { return this->PipeOutForward; }
int     CGI::getPipeInBack() { return this->PipeInBack; }
int     CGI::getPipeOutBack() { return this->PipeOutBack; }

int CGI::checkTimeout()
{
    if (time(NULL) - this->timeCGIStarted.tv_sec > TIMEOUT)
        return (9);
    return (4);
}