#include "CGI.hpp"

CGI::CGI() 
{
    this->pid = 0;
    this->PipeInForward = 0;
    this->PipeOutForward = 0;
    this->PipeInBack = 0;
    this->PipeOutBack = 0;
    this->prevStage = -1
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
    return (21);
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
        case 0: { return (this->checkTimout()); } //child not ready //fix me: implement
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
    return (29);
}

int CGI::sendToPipe(std::string &src) //fix me: перед выставлением stage 24 нужно записать предыдущий Stage в CGI class
{
	ssize_t		wrRes;

    wrRes = write(this->PipeOutForward, src.c_str(), src.length());
    switch (wrRes)
    {
        case -1: //error
        {
        	Logger::putMsg(std::string("ERROR while writing to ") + ConfParser::Size_tToString(this->PipeOutForward) + std::string(":\n") + std::string(strerror(errno)));
        	return (this->CGIFailed());
        }
    	case 0:
		{
			Logger::putMsg(std::string("write 0 bytes"), this->PipeInBack + std::string(":\n") + std::string(strerror(errno)));
			return (this->checkCntTrying('w'));
		}
        case (src.length()): //all sent
        {
        	this->cntErrorsWriting = 0;
			switch (this->prevStage)
			{
				case 1: { return(1); } //chunked request -> waiting next chunk
				case 5: { return(22); } //request is over -> waiting CGI response
				default: {std::cout << "invalid prev stage in CGI::sendToPipe: " << this->prevStage << std::endl; return(CGIFailed());}
			}
        }
		default: //part sent
		{
			this->cntErrorsWriting = 0;
			src.erase(0, wrRes);
			return (24); //repeat write
		}
    }
}

int CGI::readFromPipe(Server &thisServer)
{
	ssize_t		rdRes;
	std::string res;
	char		buf[BUF_SIZE_PIPE];

	rdRes = read(this->PipeInBack, buf, BUF_SIZE_PIPE);
	switch (rdRes)
	{
		case -1: //error
		{
			Logger::putMsg(std::string("ERROR while reading from "), this->PipeInBack + std::string(":\n") + std::string(strerror(errno)));
			return (this->CGIFailed());
		}
		case 0:
		{
			Logger::putMsg(std::string("read 0 bytes"), this->PipeInBack + std::string(":\n") + std::string(strerror(errno)));
			return (this->checkCntTrying('r'));
		}
		case BUF_SIZE_PIPE: //maybe somthing else in PIPE
		{
			this->cntTryingReading = 0;
			res = std::string(buf, rdRes);
			thisServer.setResponse(res);
			switch (thisServer.getStage())
			{
				case 23: {return(25);}
				case 25:
				case 26: {return(26);}
			}
			break;
		}
		default: //end of file
		{
			this->cntTryingReading = 0;
			res = std::string(buf, rdRes);
			thisServer.setResponse(res);
			switch (thisServer.getStage())
			{
				case 23: {return(28);}
				case 25:
				case 26: {return(27);}
			}
		}
	}
	std::cout << "BAD STAGE in read from PIPE: " << thisServer.getStage() << std::endl;
	return (this->CGIFailed());
}

//checker counters
int CGI::checkCntTrying(char c, int stage)
{
	char *checks;

	switch (c)
	{
		case 'r': {checks = &cntTryingReading; break;} //read count
		case 'w': {checks = &cntTryingWriting; break;} //write count
		default: {std::cout << "wrong c in CGI check counter: " << static_cast<int>(c);}
	}
	++(*checks);
	if (*checks < CNT_TRYING)
		return (stage);
	Logger::putMsg(std::string("read/write 0 bytes 3 times in a raw: ") + std::string(&c, 1), this->PipeInBack + std::string(":\n") + std::string(strerror(errno)));
	return (this->CGIFailed());
}


char** CGI::setArgv(Server &src, std::string &PATH_INFO, std::string &PATH_TRANSLATED, std::string &SCRIPT_NAME)
{
    char **res = NULL;
    PATH_INFO = src.getReq_struct().uri; //fix me: need to substr after '?' ?
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
    try {
        res = new char[2];
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
    return res;
}

char**  CGI::setEnv(Server &src, std::string &PATH_INFO, std::string &PATH_TRANSLATED, std::string &SCRIPT_NAME)
{
    char **res = NULL;
    size_t i = 0;
    size_t size = src->getAnsw_struct().headers.size() + STANDART_ENV_VARS_CNT + 1;
    std::map<std::string, std::string>::iterator it = src.getAnsw_struct().headers.begin();
    try
    {
        res = new char**[size](NULL);
        res[i++] = CGI::getAllocatedCharPointer(std::string("SERVER_SOFTWARE=AMANIX"));
        res[i++] = CGI::getAllocatedCharPointer(std::string(std::string("SERVER_NAME=") + src.getReq_struct().host));
        res[i++] = CGI::getAllocatedCharPointer(std::string("GATEWAY_INTERFACE=CGI/1.1"));
        res[i++] = CGI::getAllocatedCharPointer(std::string("SERVER_PROTOCOL=HTTP/1.1"));
        res[i++] = CGI::getAllocatedCharPointer(std::string(std::string("SERVER_PORT=") + src.getReq_struct().port));
        res[i++] = CGI::getAllocatedCharPointer(std::string(std::string("REQUEST_METHOD=") + src.getReq_struct().method));
        res[i++] = CGI::getAllocatedCharPointer(std::string(std::string("SCRIPT_NAME=") + SCRIPT_NAME));
        res[i++] = CGI::getAllocatedCharPointer(std::string("REMOTE_ADDR=");
        res[i++] = CGI::getAllocatedCharPointer(std::string(std::string("PATH_INFO=") + PATH_INFO);
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

char    CGI::getAllocatedCharPointer(std::string const src)
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
int     CGI::getPipeInBack()) { return this->PipeInBack; }
int     CGI::getPipeOutBack()) { return this->PipeOutBack; }
const std::string & CGI::getTosendToPipe() { return this->toSendToPipe; }

//setters
void CGI::setToSendToPipe(const std::string &src) {this->toSendToPipe = src;}