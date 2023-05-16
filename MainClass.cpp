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
        if (ConfParser::parseConf(arg, MainClass::allServers))
            std::cout << "parse config SUCCESS\n";
        else
            std::cerr << "parse config SUCCESS with WARNING\n";
    }
    catch (std::exception &e)
    {
        std::cerr << "PARSE CONFIG FAILED\n" << e.what() << std::endl;
        if (MainClass::allServers)
            delete MainClass::allServers;
        return;
    }

    if (!allServers)
    {
        std::cerr << "NO SERVER CREATED, CHECK YOUR CONFIG\n";
        return;
    }
    MainClass::mainLoop();
}

void MainClass::mainLoop()
{
    std::map<int, Server> connections = MainClass::allServers->getConnections();
    std::set<int> fds = MainClass::allServers->getFds();

    std::cout << "connections\n";
    for (std::map<int, Server>::iterator it = connections.begin(); it != connections.end(); it++)
        std::cout << "fd is " << it->first << "\nserver host is " << it->second->getHost() << "\nport is " <<
            it->second->getPort();
    delete MainClass::allServers;
}

void MainClass::exitHandler(int sig)
{
    if (sig != SIGTERM)
        return;
    if (MainClass::allServers)
        delete MainClass::allServers;
    std::cout << "ExitHandler: SIGTERM received\n";
    exit(0);
}
