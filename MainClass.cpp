# include "MainClass.hpp"

//инициализируем static vars
int         MainClass::stage = 0;
int         MainClass::maxFd = 0;
Servers*    MainClass::allServers = NULL;

void MainClass::doIt(int args, char **argv)
{
    bool        flg;
    char*       arg;

    arg = NULL;
    flg = ConfParser::checkArgs(args, argv);
    if (flg)
    {
        arg = argv[1];
        std::cout << "Check args SUCCESS\n";
    }
    else
        std::cout << "Check args SUCCESS with WARNING\n";

    try
    {
        if (ConfParser::parseConf(arg, &MainClass::allServers))
            std::cout << "parse config SUCCESS\n";
        else
            std::cerr << "parse config SUCCESS with WARNING. try Default config file\n";
    }
    catch (std::exception &e)
    {
        std::cerr << "PARSE CONFIG FAILED\n" << e.what() << std::endl;
        if (MainClass::allServers)
            delete MainClass::allServers;
        return;
    }

    if (!MainClass::allServers)
    {
        std::cerr << "NO SERVER CREATED, CHECK YOUR CONFIG\n";
        return;
    }
    MainClass::mainLoop();
}

void MainClass::mainLoop()
{
    std::map<int, Server*>  lsts = allServers->getConnections(true);
    std::map<int, Server*>  connections = allServers->getConnections(false);
    std::set<int>           fds = allServers->getFds();

    timeval                 timeout;
    fd_set                  readFds;
    fd_set                  writeFds;
    int                     selRes;

//    {
//        std::cout << "connections\n";
//        for (std::map<int, Server*>::iterator it = connections.begin(); it != connections.end(); it++)
//           std::cout << "fd is " << it->first << "\nserver host is " << it->second->getHost() << "\nport is " <<
//                it->second->getPort() << std::endl;
//        std::cout << "fds\n";
//        for (std::set<int>::iterator it = fds.begin(); it != fds.end(); it++)
//            std::cout << "fd is " << *it << std::endl;
//    }

    timeout.tv_sec = 15;//fix me: возможно менять динамически когда появляется запрос к cgi и нужно ждать ответ
    timeout.tv_usec = 0;
    while (true)
    {
        //fix me add 2.1 проверяем наличие готовых ответов от cgi. если есть - добавляем их в fd для записи
        FD_ZERO(&readFds);
        for (std::set<int>::iterator it = fds.begin(); it != fds.end(); it++)
            FD_SET(*it, &readFds);
        if (fds.empty())
            throw notEnoughFds();
        maxFd = *max_element(fds.begin(), fds.end());
        selRes = select(maxFd + 1, &readFds, &writeFds, NULL, &timeout);
        if (selRes <= 0)
        {
            if (selRes < 0) {
                Logger::putMsg(strerror(errno), FILE_ERR, ERR);
                std::cout << "bad select!!!\n";
            }
            continue;
        }
        for (std::map<int, Server*>::iterator it = lsts.begin(); it != lsls.end(); it++)
        {
            if (FD_ISSET(it->first, &readFds))
                MainClass::acceptConnections(it);
        }
        //fix me add 2.3 делаем сенд для всех готовых для записи фд.
        std::map<int, Server*>::iterator itR = connections.begin();
        while (itR != connections.end())
        {
            if (FD_ISSET(itR->first, &writeFds))
                MainClass::sendResponse(it, &writeFds);
            if (FD_ISSET(itR->first, &readFds))
                MainClass::readRequests(it, &writeFds); //считываем запросы 2.4
            else//it меняем в readRequest, так как там возможно удаление fd;
                it++;
        }
    }
}

void MainClass::sendRequests(std::map<int, Server *>::iterator &it, fd_set *wFds)
{
    if (send(it->second->getRes().c_str, it->second->getRes().length(), 0) == -1) //ошибка отправки
        Logger::putMsg(str::string(strerror(errno)), FILE_ERR, ERR);
    it->second->resClear();
    FD_CLR(it->first, wFds);
}

void MainClass::readRequests(std::map<int, Server *>::iterator &it, fd_set *wFds)
{
    int     recvRes;
    char    buf[BUF_SIZE];
    Server* serv = it->second;

    recvRes = recv(it->first;, buf, BUF_SIZE, 0);
    switch (recvRes)
    {
        case SOCKET_ERROR: //ошибка чтения -1
        {
            Logger::putMsg(std::string("error while recv ") + std::string(strerror(errno)), fd, FILE_ERR, ERR);
            MainClass::closeConnection(it);
            break;
        }
        case 0: //нечего читать
        {
            if (!MainClass::checkCont(it)) //проверяем кейс если запрос уже был считан полностью но еще не обработан при res = BUF_SIZE
                MainClass::closeConnection(it);
            break;
        }
        default:
        {
            serv->addToReq(buf, recvRes);
            if (recvRes == BUF_SIZE) //возможно есть еще что считать
                break;
            MainClass::handleRequest(it, wFds);//обработка запроса
        }
    }
    it++;
}

void MainClass::acceptConnections(std::map<int, Server *>::iterator &it)
{
    int fd;

    fd = accept(it->first, NULL, NULL);
    if (fd < 0 || fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
        Logger::putMsg(strerror(errno), FILE_ERR, ERR);
    else
        MainClass::allServers->addConnection(fd, *(it->second));
}

void MainClass::handleRequest(std::map<int, Server *>::iterator &it, fd_set *wFds) //fix me: delete GAGs
{
    Logger::putMsg(it->second->getReq(), FILE_REQ, REQ);
    it->second->setResponse(std::string(DEF_RESPONSE));
    it->second->reqClear();
    if (it->second->respReady())
        FD_SET(it->first, wFds);
}

bool MainClass::checkCont(std::map<int, Server *>::iterator &it, fd_set *wFds)
{
    if (it->second->getReq().empty())
        return (false);
    MainClass::handleRequest(it, wFds);
    return (true);
}

void MainClass::exitHandler(int sig)
{
    if (sig != SIGTERM)
        return;
    if (allServers)
        delete allServers;
    std::cout << "ExitHandler: SIGTERM received\n";
    exit(0);
}
