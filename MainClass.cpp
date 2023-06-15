#include "MainClass.hpp"

//инициализируем static vars
int         MainClass::maxFd = 0;
Servers*    MainClass::allServers = NULL;

//fix me delete this
void printAllServ(Servers* src)
{
	std::map<int, Server*>::iterator it;

	for (it = src->getConnections(true).begin(); it != src->getConnections(true).end(); it++)
	{
		std::cout << "fd is: " << it->first << "; config is:\n";
		std::cout << "host:port is: " << it->second->getHost() << ":" << it->second->getPort() << std::endl;
		t_serv* cur;
		cur = it->second->serv;
		while (cur)
		{
			std::cout << "servername is: |" << cur->ServerName << "|" << std::endl;
			std::cout << "limit client body size is: " << cur->limitCLientBodySize << std::endl;
			std::cout << "root is: " << cur->root << std::endl;
			std::cout << "err pages is:\n";
			for (std::map<int, std::string>::iterator it2 = cur->errPages.begin(); it2 != cur->errPages.end(); it2++)
				std::cout << "code is: " << it2->first << "; page is: " << it2->second << std::endl;
			t_loc *cur2 = cur->locList;
			while (cur2)
			{
				std::cout << "location is: |" << cur2->location << "|" << std::endl;
				std::cout << "GET/POST/DELETE is :" << cur2->flgGet << "/" << cur2->flgPost << "/" << cur2->flgDelete << "\n";
				for (std::map<int, std::string>::iterator itR = cur2->redirect.begin(); itR != cur2->redirect.end(); itR++)
					std::cout << "redirect is: " << itR->first << " - " << itR->second << std::endl;
				std::cout << "root is: |" << cur2->root << "|\n";
				std::cout << "dirListFlg is: |" << cur2->dirListFlg << "|\n";
				std::cout << "defFileIfDir is: |" << cur2->defFileIfDir << "|\n";
				std::cout << "uploadPath is: |" << cur2->uploadPath << "|\n";
				for (std::set<std::string>::iterator it3 = cur2->CGIs.begin(); it3 != cur2->CGIs.end(); it3++)
					std::cout << "CGIs extention is: |" << *it3 << "|\n";
				cur2 = cur2->next;
			}
			cur = cur->next;
		}
	}
}

void MainClass::doIt(int args, char **argv, char **env)
{
    bool        flg;
    char*       arg;

    arg = NULL;
    flg = ConfParser::checkArgs(args, argv);
    if (flg)
        arg = argv[1];
    try
    {
        ConfParser::parseConf(arg, &MainClass::allServers);
        std::cout << "parse config SUCCESS\n";
    }
    catch (std::exception &e)
    {
        std::cerr << "PARSE CONFIG FAILED\n" << e.what() << std::endl;
        return;
    }

	for (std::map<int, Server *>::iterator it = allServers.begin(); it != allServers.end(); it++)
	{
		if (MainClass::isRedirectionInARaw(it))
			return;
	}

    if (!MainClass::allServers || MainClass::allServers->getConnections(true).empty())
    {
        std::cerr << "NO SERVER CREATED, CHECK YOUR CONFIG\n";
        return;
    }
    std::cout << "Server has been launched!\n";

    MainClass::mainLoop();
	//printAllServ(MainClass::allServers);
}

void MainClass::mainLoop()
{
    timeval                             timeout = {1, 0};
    fd_set                              readFds;
    fd_set                              writeFds;
    std::map<int, Server*>::iterator    it;
    int                                 Stage;

    while (true)
    {
        //2.0 clean sets
        FD_ZERO(&readFds);
        FD_ZERO(&writeFds);
        MainClass::maxFd = -1;
        //handle all request until read/write or waiting child(22)
        for (it = allServers->getConnections().begin(); it != allServers->getConnections().end(); it++)
            HandlerRequest::mainHandler(it, &readFds, &writeFds);
        //2.1.2 add listen fds
        for (it = allServers->getConnections(true).begin(); it != allServers->getConnections(true).end(); it++)
            MainClass::addToSet(it->first, &readFds);
        //select ловим готовые
        switch (select(MainClass::maxFd + 1, &readFds, &writeFds, NULL, &timeout))
        {
            case -1: { //select error
                Logger::putMsg(strerror(errno), FILE_ERR, ERR); 
                std::cout << "bad select!!!\n";
                continue; //fix me: need to continue or exit from server?
            }
            case 0: {continue;} //timeout. try another select
            default: {break;} //have something to do
        }
        acceptConnections(&readFds);
        // doing write/read depend on stage for all servers
        it = allServers->getConnections().begin();
        while (it != allServers->getConnections().end())
        {
            Stage = it->second->getStage();
            switch (Stage)
            {
                //read from socket
                case 1: { MainClass::readRequest(it, Stage, &readFds); break; }
                //CGI
                case 4: { MainClass::CGIHandlerReadWrite(it, &readFds, &writeFds); break; }
                //write to socket
                case 5: { MainClass::sendResponse(it, &writeFds); break; }
                default://all servers should be in write/read stage or CGI, if not then ERROR
                {
                    std::cout << "bad stage for server: " << it->first << " stage is: " << Stage << std::endl;
                }
            }
        }
    }
}


bool MainClass::acceptConnections(fd_set *readFds)
{
    int     fd;
    bool    flgConnection = false;

    for (std::map<int, Server*>::iterator it = allServers->getConnections(true).begin(); it != allServers->getConnections(true).end(); it++)
    {
        if (FD_ISSET(it->first, readFds))
        {
            std::cout << "accept from " << it->first << " to ";
            fd = accept(it->first, NULL, NULL);
            std::cout << fd << std::endl;
            if (fd < 0 || fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
                Logger::putMsg(strerror(errno), FILE_ERR, ERR);
            else
                MainClass::allServers->addConnection(fd, *(it->second));
            flgConnection = true;
        }
    }
    return (flgConnection);
}

void MainClass::readRequest(std::map<int, Server *>::iterator &it, int Stage, fd_set *reads)
{
	if (!FD_ISSET(it->first, reads))
		return;

    ssize_t	recvRes;
    char    buf[BUF_SIZE];

    std::cout << "RECV request from: " << it->first << std::endl;
    recvRes = recv(it->first, buf, BUF_SIZE, 0);
    switch (recvRes)
    {
        case -1: //ошибка чтения
        {
            Logger::putMsg(std::string("error while recv ") + std::string(strerror(errno)), it->first, FILE_ERR, ERR);
            MainClass::closeConnection(it);
            return;
        }
        case 0: //нечего читать
        {
            Logger::putMsg(std::string("User closed connection ", it->first));
            MainClass::closeConnection(it);
            return;
        }
        default: {
			it->second->addToReq(std::string(buf, recvRes));
			it->second->Stage = 2;
		}
    }
    it++;
}

void MainClass::sendResponse(std::map<int, Server *>::iterator &it, fd_set *writes)
{
	if (!FD_ISSET(it->first, writes))
		return;
    std::cout << "SEND response to: " << it->first << std::endl;

	ssize_t sendRes;
	std::string toSend = it->second->getResponse();

	if (toSend.empty()) // nothing to send = ERROR //should not to happen //fix me: test this
	{
		Logger::putMsg(std::string("NOTHING TO SEND"), it->first, FILE_ERR, ERR);
		std::cout << it->first << ": ERROR, NOTHING TO SEND\n";
		MainClass::closeConnection(it);
		return;
	}
	sendRes = send(it->first, toSend.c_str(), toSend.length(), 0);
	switch (sendRes)
	{
		case -1: //error
		{
			Logger::putMsg(std::string("error while send ") + std::string(strerror(errno)), it->first, FILE_ERR, ERR);
			MainClass::closeConnection(it);
			return;
		}
		case 0:
		{
			if (it->second->checkCntTryingSend())
			{
				Logger::putMsg(std::string("error while send:\n3 times zero send in a raw"), it->first, FILE_ERR, ERR);
				MainClass::closeConnection(it);
			}
			break;
		}
		default:
		{
			it->second->CntTryingSendZero();
			if (static_cast<size_t>(sendRes) == toSend.length())
			{
				if (!it->second->isChunkedResponse || it->second->CGIStage == 6)
                    it->second->resClear();
                else if (it->second->CGIStage == 5 || it->second->CGIStage == 50)
                    it->second->Stage = 4;
                //fix me: maybe something else???
                else
                    std::cout << it->first << ": bad stages in send:\nStage is: " << it->second->Stage << ". CGIStage is: " << it->second->CGIStage << std::endl;
			}
			else
			{
				toSend.erase(0, sendRes);
				it->second->setResponse(toSend);
			}
			break;
		}
	}
	it++;
}

void MainClass::CGIHandlerReadWrite(std::map<int, Server *>::iterator &it, fd_set *reads, fd_set *writes)
{
    switch (it->second->CGIStage)
    {
        case 1: //1 - send to pipe
        {
            it->second->CGIStage = it->second->getCGIptr()->sendToPipe(it, writes, false);
            it->second->getCGIptr()->prevStage = 1;
            break;
        }
        case 2: //2 - last send to pipe
        {
            it->second->CGIStage = it->second->getCGIptr()->sendToPipe(it, writes, true);
            it->second->getCGIptr()->prevStage = 2;
            break;
        }
        case 20: { it->second->CGIStage = it->second->getCGIptr()->sendToPipe(it, writes, it->second->getCGIptr()->prevStage == 2); break; } //repeat write
        case 5:
        {
            it->second->CGIStage = it->second->getCGIptr()->readFromPipe(it, reads);
            if (it->second->CGIStage == 50)
                ;//fix me: find body and add chunk size
            break;
        }//read from pipe first
        case 50: // read from pipe next chunk
        {
            it->second->CGIStage = it->second->getCGIptr()->readFromPipe(it, reads, false);
            ;//fix me: handle add chunk size + add NULL-chunk if need
            break;
        }
        default: { std:: cout << "BAD Stage CGI: " << it->second->Stage << " - " << it->second->CGIStage << std::endl; return; }
    }
}

void MainClass::closeConnection(std::map<int, Server *>::iterator &it)
{
    int fd = it->first;

    it++;
    close(fd);
    MainClass::allServers->removeConnection(fd);
    std::cout << "close connection: " << fd << std::endl;
}

void MainClass::exitHandler(int sig)
{
    if (sig != SIGTERM && sig != 0)
        return;
    if (sig == SIGTERM)
        std::cout << "ExitHandler: SIGTERM received\n";
    else
        std::cout << "EXCEPTION exit. check LOGS\n";
    system("leaks webserv");
    exit(0);
}

void MainClass::addToSet(int fd, fd_set *dst)
{
    FD_SET(fd, dst);
    if (fd > MainClass::maxFd)
        MainClass::maxFd = fd;
}

bool MainClass::isRedirectionInARaw(std::map<int, Server *>::iterator it)
{
	t_serv		*chNodeServ;
	t_loc		*chNodeLoc;
	std::string	redirectString;
	std::string name;
	std::string ip;
	std::string port;

	for (chNodeServ = it->second->serv; chNodeServ != NULL; chNodeServ = chNodeServ->next)
	{
		for (chNodeLoc = chNodeServ->locList; chNodeLoc != NULL; chNodeLoc = chNodeLoc->next)
		{
			if (chNodeLoc->redirect.empty())
				continue;
			redirectString = chNodeLoc->redirect.begin()->second;
			if (!MainClass::checkCorrectHostPortInRedirection(redirectString, name, port))
				return (true);
			//find Server *
			for (std::map<int, Server *>::iterator itA = allServers->getConnections(true).begin(); itA != allServers->getConnections(true).end(); itA++)
			{
				if (ip == itA->second->getHost())
				{
					t_serv	*curServ = itA->second->findServer(name);
					t_loc	*curLoc = Server::findLocation(redirectString, curServ);
					if (!curLoc->redirect.empty())
					{
						Logger::putMsg("Two or more redirection in a raw: " + ip + name + ":" + port, FILE_ERR, ERR);
						return (true);
					}
				}
			}
		}
	}
	return (false);
}

bool MainClass::checkCorrectHostPortInRedirection(std::string &src, std::string &name, std::string &port, std::string &ip)
{
	std::string::size_type	i;
	std::string				tmp;

	i = src.find("http://");
	if (i != 0)
	{
		i = src.find("https://");
		if (i != 0)
			return (false);
		else
		{
			src.erase(0, 8);
			port = "443";
		}
	}
	else
	{
		port = "80";
		src.erase(0, 7);
	}
	i = src.find("www.");
	if (i == 0)
		src.erase(0, 4);
	i = src.find('/');
	tmp = substr(0, i);
	src.erase(0, i);
	i = src.rfind('/');
	src.resize(i + 1);
	i = tmp.find(':');
	name = tmp.substr(0, i);
	tmp.erase(0, i + 1);
	if (!tmp.empty())
		port = tmp;
	//check port
	for (i = 0; i < 6; i++)
	{
		if (i >= 5 || !std::isdigit(port[i]))
		{
			Logger::putMsg("BAD Port in redirection: " + port + "host", FILE_ERR, ERR);
			return (false);
		}
	}
	while (port.length() < 5)
		port = '0' + port;
	if (port > "65535")
	{
		Logger::putMsg("BAD Port in redirection: " + port + "host", FILE_ERR, ERR);
		return (false);
	}
	//check host/name
	if (ConfParser::checkCorrectHost(name))
	{
		ip = name;
		name.clear();
	}
	if (name == "localhost")
		ip = "127.0.0.1";
	return (true);
}

bool MainClass::hasInfLoop(std::map<int, Server *>::iterator it, t_serv *srcSrv, t_loc *srcLoc, std::string dst)
{

}