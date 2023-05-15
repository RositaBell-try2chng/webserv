#include <Servers.hpp>

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

    for (it = this->connections.begin(); it != this->connections.end(); it++)
        delete it->second;
    this->fds.clear();
    this->connections.clear();
    Servers::flgCreate = false;
    Logger::putMsg("delete obj of servers");
}

std::map<int, Server *>&    Servers::getConnections() { return (this->connections); }

std::set<int>&  Servers::getFds() { return (this->fds);}

Server& Servers::getServer(int fd)
{
    std::map<int, Server *>::iterator it;

    it = this->connections.find(fd);
    if (it == this->connections.end())
        throw noSuchConnection();
    return *(it->second);
}

void    Servers::addConnection(int fd, Server const &src)
{
    std::map<int, Server *>::iterator it;

    it = this->connections.find(fd);
    if (it != this->connections.end())
    {
        Logger::putMsg("This fd already exists: ", FILE_ERR, ERR, fd);
        return;
    }
    this->connection.insert(pair<int, Server *>(fd, src.clone()));
    this->fds.insert(fd);
}

void    Servers::removeConnection(int fd)
{
    std::map<int, Server *>::iterator it;

    it = this->connections.find(fd);
    if (it == this->connections.end())
    {
        Logger::putMsg("There is no such fd: ", FILE_ERR, ERR, fd);
        return;
    }
    delete it->second;
    this->connections.erase(fd);
    this->fds.erase(fd);
}

void    Servers::createServer(std::string const &host, std::string const &port)
{
    Server      tmp(host, port);
    t_listen    all;

    bzero(&all, sizeof(all));
    all.hints.ai_family = AF_INET;
    all.hints.ai_socktype = SOCK_STREAM;
    all.hints.ai_flags = AI_PASSIVE;

    all.stts = getaddrinfo(host, port, &all.hints, &all.info);
    if (all.stts != 0)
        throw exceptionGetAddrInfo();
    all.sockFd = socket(all.info->ai_family, all.info->ai_socktype, all.info->ai_protocol);
    if (all.sockFd < 0 || \
		fcntl(all.sockFd, F_SETFL, O_NONBLOCK) == -1 || \
		bind(all.sockFd, all.info->ai_addr, all.info->ai_addrlen) < 0 || \
		listen(all.sockFd, 20) < 0)
        throw exceptionErrno();
    this->addConnection(all.sockFd, tmp);
    freeaddrinfo(all.info);
}