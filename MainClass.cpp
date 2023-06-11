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
    timeval                             timeout;
    fd_set                              readFds;
    fd_set                              writeFds;
    std::map<int, Server*>::iterator    it;
    int                                 Stage;

    timeout.tv_sec = 15;//fix me: возможно менять динамически когда появляется запрос к cgi и нужно ждать ответ
    timeout.tv_usec = 0;
    while (true)
    {
        //2.0 clean sets
        FD_ZERO(&readFds);
        FD_ZERO(&writeFds);
        maxFd = -1;
        //handle all request until read/write or waiting child(22)
        for (it = allServers->getConnections().begin(); it != allServers->getConnections().end(); it++)
        {
            Stage = it->second->getStage();
            switch (Stage)
            {
                //reading
                case 4: //something else in request after -> parsing
                case 30: //запрос только считан -> parsing
                case 50: //start parsing -> parsing
                case 5: //ready to handle -> handle
                //CGI
                case 20: //CGI creating -> создание объекта
                case 21: //fork + start script -> fork + запуск
                case 22: //waiting child check timeout -> ждем ребенка
                case 25: //first chunk from pipe was read -> send
                case 26: //next chunk from pipe was read -> send
                case 27: //ответ считан полностью после чанка -> send
                case 28: //ответ считан полностью -> send
                
                case 40: //открыть файл -> чтение файла
                case 43: HandlerRequest::prepareToSend(it, Stage);//ответ полностью считан -> send
                
                case 51: // - start string was fully parsed
				case 52: // - headers was fully parsed
				case 53: // - body was fully parsed
            }
        }
        //добавляем использующиеся сокеты
        for (it = allServers->getConnections().begin(); it != allServers->getConnections().end(); it++)
        {
            switch (it->second->getStage())
            {
                case 0:
                case 1: 
                case 2: 
                case 3: { MainClass::addToSet(it->first, maxFd, &readFds); break; }
                case 10:
                case 11:
                case 12: { MainClass::addToSet(it->first, maxFd, &writeFds); break; }
                case 23: 
                case 25:
                case 26: { MainClass::addToSet(it->second->getCGIptr()->PipeInBack, maxFd, &readFds); break; }
                case 24: { MainClass::addToSet(it->second->getCGIptr()->PipeOutForward, maxFd, &readFds); break; }
                //fix me: add 40 - 99 need stages
                default:
                {
                    break;//fix me: check correct stage???
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
        // doing write/read depend on stage for all servers
        it = allServers->getConnections().begin();
        while (it != allServers->getConnections().end())
        {
            Stage = it->second->getStage();
            switch (Stage)
            {
                //read from socket
                case 0:
                case 1:
                case 2:
                case 3: { MainClass::readRequest(it, Stage, &readFds, &writeFds); break; }
                //write to socket
                case 10:
                case 11:
                case 12: { MainClass::sendResponse(it, Stage, &readFds, &writeFds); break; }
                //CGI
                case 22: {break;} //waiting child process
                case 23:
                case 24: { MainClass::writeReadCGI(it, Stage, &readFds, &writeFds); break; } //fix me: implement
                //read from file
                case 41:
                case 42: { MainClass::readFiles(it, Stage, &readFds); break; }
                //errors
                case 29: 
                case 39:
                case 49: 
                case 59:
                case 99: {MainClass::errorManager(it, Stage, &readFds, &writeFds); break; }
                default://all servers should be in write/read stage or waiting child(22), if not then ERROR
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
            if (it->second->getStage() == 1)
                 Logger::putMsg(std::string("waiting next chunk", it->first));
            else
            {
                Logger::putMsg(std::string("User closed connection ", it->first));
                MainClass::closeConnection(it);
            }
            return;
        }
        default: { it->second->addToReq(std::string(buf, recvRes)); }
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