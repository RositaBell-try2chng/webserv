#include "Servers.hpp"

bool Servers::flgCreate = false;

Servers::Servers()
{
    if (Servers::flgCreate)
        throw serversAlreadyExists();
    Servers::flgCreate = true;
    Logger::putMsg("Create obj of servers");
}

Servers::~Servers()
{
    std::map<int, Server *>::iterator it;

    for (it = this->lst.begin(); it != this->lst.end(); it++)
        delete it->second;
    for (it = this->connections.begin(); it != this->connections.end(); it++)
        delete it->second;
    this->fds.clear();
    Servers::flgCreate = false;
    Logger::putMsg("delete obj of servers");
}

std::map<int, Server *>&    Servers::getConnections(bool lstFlg)
{
    if (!lstFlg)
        return (this->connections);
    return (this->lst);
}

std::set<int>&  Servers::getFds() { return (this->fds);}

Server& Servers::getServer(int fd)
{
    std::map<int, Server *>::iterator it;

    it = this->connections.find(fd);
    if (it == this->connections.end())
        throw noSuchConnection();
    return *(it->second);
}

void    Servers::addConnection(int fd, Server const &src, bool lstFlg)
{
    std::set<int>::iterator it;

    it = this->fds.find(fd);
    if (it != this->fds.end())
    {
        Logger::putMsg("This fd already exists: ", fd, FILE_ERR, ERR);
        return;
    }
    if (!lstFlg)
        this->connections.insert(std::pair<int, Server *>(fd, src.clone()));
    else
        this->lst.insert(std::pair<int, Server *>(fd, src.clone()));
    this->fds.insert(fd);
    //std::cout << "add connection: " << fd << std::endl;
    Logger::putMsg(std::string("add connection "), fd);
}

void    Servers::removeConnection(int fd, bool lstFlg)
{
    std::map<int, Server*>::iterator    it;

    if (!lstFlg)
        it = this->connections.find(fd);
    else
        it = this->lst.find(fd);
    if ((!lstFlg && it == this->connections.end()) || (lstFlg && it == this->lst.end()))
    {
        Logger::putMsg("There is no such fd: ", fd, FILE_ERR, ERR);
        return;
    }
    delete it->second;
    if (!lstFlg)
        this->connections.erase(fd);
    else
        this->lst.erase(fd);
    this->fds.erase(fd);
    Logger::putMsg(std::string("remove connection "), fd);
}

void    Servers::createServer(std::string const &host, std::string const &port)
{
    Server      tmp(host, port);
    t_listen    all;

    bzero(&all, sizeof(all));
    all.hints.ai_family = AF_INET;
    all.hints.ai_socktype = SOCK_STREAM;
    all.hints.ai_flags = AI_PASSIVE;

    all.stts = getaddrinfo(host.c_str(), port.c_str(), &all.hints, &all.info);
    if (all.stts != 0) {
        Logger::putMsg(std::string("getaddrinfo Failed\n") + std::string(gai_strerror(all.stts)), FILE_ERR, ERR);
        throw exceptionGetAddrInfo();
    }
    all.sockFd = socket(all.info->ai_family, all.info->ai_socktype, all.info->ai_protocol);
    if (all.sockFd < 0 || \
		fcntl(all.sockFd, F_SETFL, O_NONBLOCK) == -1 || \
		bind(all.sockFd, all.info->ai_addr, all.info->ai_addrlen) < 0 || \
		listen(all.sockFd, 20) < 0)
        throw exceptionErrno();
    this->addConnection(all.sockFd, tmp, true);
    freeaddrinfo(all.info);
}