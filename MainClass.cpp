# include "MainClass.hpp"

//инициализируем static vars
int         MainClass::maxFd = 0;
Servers*    MainClass::allServers = NULL;

void MainClass::doIt(int args, char **argv)
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

    if (!MainClass::allServers)
    {
        std::cerr << "NO SERVER CREATED, CHECK YOUR CONFIG\n";
        return;
    }
    //MainClass::mainLoop();
}

void MainClass::mainLoop()
{
    timeval                 timeout;
    fd_set                  readFds;
    fd_set                  writeFds;
    int                     selRes;

    timeout.tv_sec = 15;//fix me: возможно менять динамически когда появляется запрос к cgi и нужно ждать ответ
    timeout.tv_usec = 0;
    while (true)
    {
        //fix me add 2.1 проверяем наличие готовых ответов от cgi. если есть - добавляем их в fd для записи
        FD_ZERO(&readFds);
        FD_ZERO(&writeFds);
        maxFd = -1;
        //добавляем использующиеся сокеты
        for (std::map<int, Server*>::iterator it = allServers->getConnections().begin(); it != allServers->getConnections().end(); it++)
        {
            if (it->second->respReady()) //если ответ готов добавляем
                FD_SET(it->first, &writeFds);
            maxFd = it->first;
        }
        //добавляем слушающие сокеты
        for (std::map<int, Server*>::iterator it = allServers->getConnections(true).begin(); it != allServers->getConnections(true).end(); it++)
        {
            FD_SET(it->first, &readFds);
            if (maxFd < it->first)
                maxFd = it->first;
        }
        //select ловим готовые
        selRes = select(maxFd + 1, &readFds, &writeFds, NULL, &timeout);
        if (selRes <= 0) //либо ошибка либо готовых пока нет// идем на новый заход
        {
            if (selRes < 0)//ошибка селекта
            {
                Logger::putMsg(strerror(errno), FILE_ERR, ERR);
                std::cout << "bad select!!!\n";
            }
            continue;
        }
        //проверяем новые подключения // есть -> идем на новый заход чтобы быстро разгрузить всю очередь
//        if (acceptConnections(readFds))
//            continue;
        acceptConnections(&readFds);
        //проверяем готовых для записи/чтения
        std::map<int, Server*>::iterator itR = allServers->getConnections().begin();
        while (itR != allServers->getConnections().end())
        {
            //only one read/write per client per select
            if (FD_ISSET(itR->first, &writeFds))
                MainClass::sendResponse(itR);
            else if (FD_ISSET(itR->first, &readFds))
                MainClass::readRequests(itR); //считываем запросы 2.4
            else//it меняем в readRequest и sendResponse.
                itR++;
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
            fd = accept(it->first, NULL, NULL);
            if (fd < 0 || fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
                Logger::putMsg(strerror(errno), FILE_ERR, ERR);
            else
                MainClass::allServers->addConnection(fd, *(it->second));
            flgConnection = true;
            //std::cout << "accept connection: " << fd << " from: " << it->first << std::endl;
        }
    }
    return (flgConnection);
}

void MainClass::readRequests(std::map<int, Server *>::iterator &it)
{
    ssize_t	recvRes;
    char    buf[BUF_SIZE];

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
            if (MainClass::checkCont(it)) //проверяем кейс если запрос уже был считан полностью но еще не обработан при res == BUF_SIZE
                break;
            Logger::putMsg(std::string("User closed connection ", it->first));
            MainClass::closeConnection(it);
            return;
        }
        default:
        {
            it->second->addToReq(buf);
            if (recvRes == BUF_SIZE) //возможно есть еще что считать
                break;
            MainClass::handleRequest(it);//обработка запроса
        }
    }
    it++;
}


void MainClass::sendResponse(std::map<int, Server *>::iterator &it)
{
    ssize_t	res;
	size_t	len;

    if (it->second->getRes().empty()) // nothing to send
    {
        Logger::putMsg(std::string("NOTHING TO SEND"), FILE_ERR, ERR);
        it->second->resClear();
        return;
    }
    len = it->second->getRes().length();
    res = send(it->first, it->second->getRes().c_str(), len, 0);
    switch (res)
    {
        case -1: //error
        {
            Logger::putMsg(std::string("error while send ") + std::string(strerror(errno)), it->first, FILE_ERR, ERR);
            MainClass::closeConnection(it);
            return;
        }
        case 0:
        {
            return;
        }
        default:
        {
            if (static_cast<size_t>(res) < len) //если отправилось не все, убираем то что не послалось.
                it->second->resizeResponse(res);
            else
                it->second->resClear();
        }
    }
}

void MainClass::closeConnection(std::map<int, Server *>::iterator &it)
{
    int fd = it->first;

    it++;
    close(fd);
    MainClass::allServers->removeConnection(fd);
    //std::cout << "close connection: " << fd << std::endl;
}

void MainClass::handleRequest(std::map<int, Server *>::iterator &it) //fix me: delete GAGs
{
    Logger::putMsg(it->second->getReq(), FILE_REQ, REQ);
    it->second->setResponse(std::string(DEF_RESPONSE));
    it->second->reqClear();
}

bool MainClass::checkCont(std::map<int, Server *>::iterator &it)
{
    if (it->second->getReq().empty())
        return (false);
    MainClass::handleRequest(it);
    return (true);
}

void MainClass::exitHandler(int sig)
{
    if (sig != SIGTERM && sig != 0)
        return;
    if (sig == SIGTERM)
        std::cout << "ExitHandler: SIGTERM received\n";
    else
        std::cout << "EXCEPTION exit. check LOGS\n";
    //system("leaks webserv");
    exit(0);
}