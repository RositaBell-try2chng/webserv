# include "MainClass.hpp"

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

    if (!MainClass::allServers || MainClass::allServers->getConnections(true).empty())
    {
        std::cerr << "NO SERVER CREATED, CHECK YOUR CONFIG\n";
        return;
    }
    std::cout << "Server has been launched!\n";
    MainClass::envCGI = env;
    MainClass::mainLoop();
	//printAllServ(MainClass::allServers);
}

void MainClass::mainLoop()
{
    timeval                 timeout;
    fd_set                  readFds;
    fd_set                  writeFds;

    timeout.tv_sec = 15;//fix me: возможно менять динамически когда появляется запрос к cgi и нужно ждать ответ
    timeout.tv_usec = 0;
    while (true)
    {
        //2.0 clean sets
        FD_ZERO(&readFds);
        FD_ZERO(&writeFds);
        maxFd = -1;
        //добавляем использующиеся сокеты
        for (std::map<int, Server*>::iterator it = allServers->getConnections().begin(); it != allServers->getConnections().end(); it++)
        {
            switch (it->second->getStage())
            {
                case 0:
                case 1: { MainClass::addToSet(it->first, maxFd, &readFds); break; }
                case 10:
                case 11:
                case 12: { MainClass::addToSet(it->first, maxFd, &writeFds); break; }
                case 20:
                case 21:
                case 22:
                case 23: {
                    MainClass::handlerCgi(itR);
                    break;
                }
                default:
                {
                    if (it->second->getStage() >= 30 && it->second->getStage() < 40)
                        ;
                    else
                        std::cout << it->first << "has incorrect stage!\n";
                    break;
                }
            }
        }
        //2.1.2 add listen fds
        for (std::map<int, Server*>::iterator it = allServers->getConnections(true).begin(); it != allServers->getConnections(true).end(); it++)
            MainClass::addToSet(it->first, maxFd, &readFds);
        //select ловим готовые
        switch (select(maxFd + 1, &readFds, &writeFds, NULL, &timeout))
        {
            case -1: { //select error
                Logger::putMsg(strerror(errno), FILE_ERR, ERR); 
                std::cout << "bad select!!!\n";
                continue; //fix me: need to continue or exit from server?
            }
            case 0: {continue;} //timeout. try another select
            default: {break;} //have somthing to do
        }
        acceptConnections(&readFds);
        //проверяем готовых для записи/чтения
        std::map<int, Server*>::iterator itR = allServers->getConnections().begin();
        while (itR != allServers->getConnections().end())
        {
            switch (itR->second->getStage())
            {
                case 0: { 
                    if (FD_ISSET(itR->first, &readFds))
                        MainClass::readRequest(itR);
                    break;
                }
                case 1: {
                    if (FD_ISSET(itR->first, &readFds))
                        MainClass::readNextChunk(itR);
                    break;
                }
                case 10:
                case 11:
                case 12: {
                    if (FD_ISSET(itR->first, &writeFds))
                        MainClass::sendResponse(itR);
                    break;
                }
                case 20:
                case 21:
                case 22:
                case 23: {
                    MainClass::handlerCGI(itR);
                    break;
                }
                default:
                {
                    if (itR->second->getStage() >= 30 && itR->second->getStage() < 40)
                        ;
                    else
                        std::cout << itR->first << "has incorrect stage!\n";//fix me: what is wrong with stage???
                    itR++;
                    break;
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

void MainClass::readRequest(std::map<int, Server *>::iterator &it)
{
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
        default:
        {
            it->second->addToReq(std::string(buf, recvRes));
            it->second->setStage(30);
            MainClass::handleRequest(it);
        }
    }
    it++;
}

void MainClass::readNextChunk(std::map<int, Server *>::iterator &it)
{
    ssize_t	recvRes;
    char    buf[BUF_SIZE];

    std::cout << "RECV next chunk request from: " << it->first << std::endl;
    recvRes = recv(it->first, buf, BUF_SIZE, 0);
    switch (recvRes)
    {
        case -1: //error recv
        {
            Logger::putMsg(std::string("error while recv ") + std::string(strerror(errno)), it->first, FILE_ERR, ERR);
            MainClass::closeConnection(it);
            return;
        }
        case 0: //nothing to read
        {
            if (MainClass::checkTimout(it->second))//fix me: implement this
            {
                Logger::putMsg(std::string("waiting chunk timeout"), it->first, FILE_ERR, ERR);
                MainClass::closeConnection(it);
                return;
            }
            else
                Logger::putMsg(std::string("Waiting next chunk"), it->first);
            return;
        }
        default:
        {
            if (!it->second->addToChunk(std::string(buf, recvRes)))
            {
                Logger::putMsg(std::string("incorrect chunk: ") + std::string(buf), it->first, FILE_ERR, ERR);
                MainClass::closeConnection(it);
                return;
            }
        }
    }
    it++;
}

void MainClass::sendResponse(std::map<int, Server *>::iterator &it)
{
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
		case toSend.length():
		{
			it->second->resClear();
			it->second->CntTryingSendZero();
			if (it->second->getStage() != 11)
				it->second->setStage(0);
			break;
		}
		default:
		{
			toSend.erase(0, sendRes);
			it->second->setResponse(toSend);
			it->second->CntTryingSendZero();
			break;
		}
	}
	it++;
}

void MainClass::closeConnection(std::map<int, Server *>::iterator &it)
{
    int fd = it->first;

    it++;
    close(fd);
    MainClass::allServers->removeConnection(fd);
    std::cout << "close connection: " << fd << std::endl;
}

void MainClass::handleRequest(std::map<int, Server *>::iterator &it)
{
   // Logger::putMsg(it->second->getReq(), FILE_REQ, REQ);
//    it->second->setReq_struct(HTTP_Request::ft_strtoreq(it->second->getReq(), it->second->serv->getMaxBodySize()));
//    it->second->setAnsw_struct(HTTP_Answer::ft_reqtoansw(it->second->getReq_struct()));
    // if (it->second->getCGIsFlg())
    //     MainClass::startCGI(*makeit->second);//implement
//    if (!it->second->respReady())
//        it->second->setResponse(HTTP_Answer::ft_answtostr(it->second->getAnsw_struct()));
    it->second->setResponse(DEF_RESPONSE);
    it->second->reqClear();
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

void addToSet(int fd, int &maxFd, fd_set *dst)
{
    FD_SET(fd, dst);
    if (fd > maxFd)
        maxFd = fd;
}