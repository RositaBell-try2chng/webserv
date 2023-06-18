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
    Servers::flgCreate = false;
    Logger::putMsg("delete obj of servers");
}

std::map<int, Server *>&    Servers::getConnections(bool lstFlg)
{
    if (!lstFlg)
        return (this->connections);
    return (this->lst);
}

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
    std::map<int, Server*>::iterator    it;

    it = this->connections.find(fd);
    if (it != this->connections.end())
    {
        Logger::putMsg("This fd already exists: ", fd, FILE_ERR, ERR);
        return;
    }
    it = this->lst.find(fd);
    if (it != this->lst.end())
    {
        Logger::putMsg("This fd already exists: ", fd, FILE_ERR, ERR);
        return;
    }
    if (!lstFlg)
        this->connections.insert(std::pair<int, Server *>(fd, src.clone()));
    else
        this->lst.insert(std::pair<int, Server *>(fd, src.clone()));
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
    Logger::putMsg(std::string("remove connection "), fd);
}

void    Servers::createServer(std::string const &host, std::string const &port, std::map<std::string, std::string> &S, std::map <std::string, std::map<std::string, std::string> > &L, std::vector<std::string> &SN, std::vector<std::string> &E)
{
    Server      tmp(host, port);
    t_listen    all;
    std::map<std::string, std::string>::iterator it;
    ssize_t     SizeMax;

	tmp.setServList(S, L, SN, E);
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
	{
		freeaddrinfo(all.info);
		throw exceptionErrno();
	}
    it = S.find("limitBodySize");
    if (it == S.end())
        tmp.setMaxBodySize(-1);
    else
    {
        SizeMax = strToSSize_t(it->second, 100000);
        if (SizeMax == -1)
            throw badConfig();
        else
            tmp.setMaxBodySize(SizeMax);
    }
        
    this->addConnection(all.sockFd, tmp, true);
    freeaddrinfo(all.info);
}

void Servers::addServers(std::map<std::string, std::string> &S, std::map <std::string, std::map<std::string, std::string> > &L, std::vector<std::string> &P,
					  std::vector<std::string> &E)
{
    std::map<std::string, std::string>::iterator    it;
    std::vector<std::string>::iterator              it2;
    std::string host(DEF_HOST);
    std::string port;
    std::vector<std::string>                        servNames;
    int                                             fd;

    if (P.empty() || L.empty()) {
        Logger::putMsg("HAVE NO PORTS OR LOCATIONS", FILE_ERR, ERR);
        throw badConfig();
    }
    it = S.find("host");
    if (it != S.end()) {
        host = it->second;
        S.erase(it);
    }
    it = S.find("server_name");
    if (it != S.end()) {
        Servers::setServNames(it->second, servNames);
        S.erase(it);
    }
    for (it2 = P.begin(); it2 != P.end(); it2++)
	{
        port = *it2;
        fd = this->checkExistsHostPort(host, port);
		if (fd == -1)
		{
            try {
                this->createServer(host, port, S, L, servNames, E);
				std::cout << "Server on " << host << ":" << port << " created successfully\n";
            }
            catch (std::exception &e) {
                Logger::putMsg("Creating server failed:\n" + host + ':' + port + ".\n" + e.what(), FILE_ERR, ERR);
				std::cout << "Server on " << host << ":" << port << " created failed\n";
            }
        }
		else if (!servNames.empty())
			this->addToConnection(fd, S, L, servNames, E);
		else
		{
			Logger::putMsg("SERVER ALREADY EXISTS ON:\n" + host + ':' + port, FILE_ERR, ERR);
			throw badConfig();
		}
    }
}

void Servers::addToConnection(int fd, std::map<std::string, std::string> &S, std::map<std::string, std::map<std::string, std::string> > &L, std::vector<std::string> &SN,
						   std::vector<std::string> &E)
{
    std::map<std::string, std::string>::iterator    it2;
    ssize_t                                         SizeMax;
	std::vector<std::string>::iterator              it;
	Server                                          *dst;
	t_serv                                          *cur;

	dst = this->lst.find(fd)->second;
    it2 = S.find("limitBodySize");
    if (it2 != S.end())
    {
        SizeMax = strToSSize_t(*it, 100000);
        if (SizeMax == -1)
            throw badConfig();
        else if (SizeMax > dst->getMaxBodySize())
            dst->setMaxBodySize(SizeMax);
    }
	it = SN.begin();
	while (it != SN.end())
	{
		cur = dst->serv;
		while (cur)
		{
			if (cur->ServerName == *it)
			{
				it = SN.erase(it);
				break;
			}
			cur = cur->next;
		}
		if (!cur)
			it++;
	}
	if (SN.empty())
	{
		Logger::putMsg("HAVE NO UNIQ SERVER NAME", FILE_ERR, ERR);
		throw badConfig();
	}
	dst->setServList(S, L, SN, E);
}

int Servers::checkExistsHostPort(const std::string &H, const std::string &P)
{
	std::map<int, Server*>::iterator	it;

	for (it = this->getConnections(true).begin(); it != this->getConnections(true).end(); it++)
	{
		if (it->second->getHost() == H && it->second->getPort() == P)
			return (it->first);
	}
	return (-1);
}

void Servers::setServNames(std::string &src, std::vector<std::string> &Names)
{
    std::string name;
    std::string::size_type  i = 0;
    std::string::size_type  from;
    std::string::size_type  len = src.length();

    while (i < len)
    {
        //found
        from = i;
        while (i < len && !std::isspace(src[i]))
            i++;
        //substr
        name = src.substr(from, i - from);
        if (std::find(Names.begin(), Names.end(), name) == Names.end())
            Names.push_back(name);
		else
			Logger::putMsg("BAD CONFIG: DOUBLE SERVER NAME:\n" + name, FILE_ERR, ERR);
        //skip spaces
        while (i < len && std::isspace(src[i]))
            i++;
    }
}