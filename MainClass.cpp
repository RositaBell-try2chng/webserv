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
    int     recvRes;
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
    int res;

    if (it->second->getRes().empty()) // nothing to send
    {
        Logger::putMsg(std::string("NOTHING TO SEND"), FILE_ERR, ERR);
        it->second->resClear();
        return;
    }
    res = send(it->first, it->second->getRes().c_str(), it->second->getRes().length(), 0);
    if (res < 1)
    {
        Logger::putMsg(std::string(strerror(errno)), FILE_ERR, ERR);//ошибка отправки
        MainClass::closeConnection(it);
    }
    it->second->resClear();
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
    //fix me when addr already use - segmentation fault
    if (sig != SIGTERM && sig != 0)
        return;
    if (sig == SIGTERM)
        std::cout << "ExitHandler: SIGTERM received\n";
    else
        std::cout << "EXCEPTION exit. check LOGS\n";
    exit(0);
}
