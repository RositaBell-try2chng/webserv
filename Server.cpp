#include "Server.hpp"
#include "Exceptions.hpp"
#include "Logger.hpp"

Server::Server() {};
Server::~Server() {};

//инициализируем статические переменный класса
std::string 	Server::conf = std::string(DEFAULT_CONF);
t_listen		*Server::main = NULL;

void Server::startServer()
{
	Logger::putMsg("Start launching server");

	Server::prepareServ();
	Server::mainLoop();
}

void Server::fillHints()
{
	Server::main->hints.ai_family = AF_INET;
	Server::main->hints.ai_socktype = SOCK_STREAM;
	Server::main->hints.ai_flags = AI_PASSIVE;
}

void Server::prepareServ()
{
	signal(SIGTERM, Server::exitHandler);

	Server::main = new t_listen;
	bzero(&Server::main->hints, sizeof(Server::main->hints));
	Server::fillHints();

	Server::main->stts = getaddrinfo(NULL, "8081", &Server::main->hints, &Server::main->info);
	if (Server::main->stts != 0)
		throw exceptionGetAddrInfo();
	Server::main->sockFd = socket(Server::main->info->ai_family, Server::main->info->ai_socktype, Server::main->info->ai_protocol);
	if (Server::main->sockFd < 0 || \
		fcntl(Server::main->sockFd, F_SETFL, O_NONBLOCK) == -1 || \
		bind(Server::main->sockFd, Server::main->info->ai_addr, Server::main->info->ai_addrlen) < 0 || \
		listen(Server::main->sockFd, 10) < 0)
		throw exceptionErrno();
	Logger::putMsg("Server ready to accept connections");
}

void Server::mainLoop()
{
	timeval			timeout;
	fd_set			readFds;
	std::set<int>	clients;
	int				maxFd;
	std::string		request;
	int				selRes;

	timeout.tv_sec = 15;
	timeout.tv_usec = 0;
	clients.clear();

	while (true)
	{
		FD_ZERO(&readFds);
		FD_SET(Server::main->sockFd, &readFds);
		for (std::set<int>::iterator it = clients.begin(); it != clients.end(); it++)
			FD_SET(*it, &readFds);
		maxFd = Server::main->sockFd;
		if (!clients.empty())
			maxFd = std::max(maxFd, *max_element(clients.begin(), clients.end()));
		selRes = select(maxFd + 1, &readFds, NULL, NULL, &timeout);
		if (selRes <= 0)
		{
			if (selRes < 0)
				Logger::putMsg(strerror(errno), FILE_ERR, ERR);
			continue;
		}

		if (FD_ISSET(Server::main->sockFd, &readFds))
		{
			Server::acceptNewConnection(clients);
			continue;
		}
		Server::communicate(clients, request, &readFds);
	}
}

void Server::communicate(std::set<int> &clients, std::string &request, fd_set *pReadFds)
{
	std::set<int>::iterator it;

	it = clients.begin();
	while (it != clients.end())
	{
		request.clear();
		if (FD_ISSET(*it, pReadFds))
			recvAll(it, clients, request);
		else
			it++;
	}
}

void Server::acceptNewConnection(std::set<int> &clients)
{
	int	fd;

	fd = accept(Server::main->sockFd, NULL, NULL);
	if (fd < 0 || fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		Logger::putMsg(strerror(errno), FILE_ERR, ERR);
	else
		clients.insert(fd);
}

void Server::recvAll(std::set<int>::iterator &it, std::set<int> &clients, std::string &res)
{
	int			recvRes;
	char		buf[BUF_SIZE];
	int			fd;

	fd = *it;
	recvRes = recv(*it, buf, BUF_SIZE, 0);
	while (recvRes > 0)
	{
		res += std::string(buf, recvRes);
		recvRes = recv(*it, buf, BUF_SIZE, 0);
	}
	Logger::putMsg(res, FILE_REQ, REQ);
	//handler of requests
	send(fd, ANSWER, ANSWER_LEN, 0);
	if (recvRes <= 0)
	{
		if (recvRes < 0)
			Logger::putMsg(std::string("error while recv ") + strerror(errno), FILE_ERR, ERR);
		close(fd);
		it++;
		clients.erase(fd);
	}
}

bool Server::checkArgs(int args, char **argv)
{
	Logger::putMsg("start parsing arguments");
	if (args == 1)
	{
		Logger::putMsg(std::string(NOT_PASSED_ARG), FILE_ERR, ERR);
		Logger::putMsg(std::string(TRY_DEFAULT) + Server::conf);
	}
	else
		Server::conf = std::string(argv[1]);
	if (args > 2)
		Logger::putMsg(std::string(TOO_MANY_ARGS) + Server::conf, FILE_ERR, ERR);
	if (Server::conf.rfind(".conf") != Server::conf.length() - 5)
	{
		Logger::putMsg(std::string(WRONG_EXTENSION_OF_FILE) + Server::conf, FILE_ERR, ERR);
		std::cerr << "Server have not started. More information in logs\n";
		return (false);
	}
	Logger::putMsg("parsing arguments SUCCESS");
	return (true);
}

bool Server::parse()
{
	Logger::putMsg("start parsing config file");
	//fix me: add deep parsing of config file
	std::ifstream in;

	in.open(Server::conf.c_str());
	if (!in.is_open())
	{
		Logger::putMsg(std::string(strerror(errno)) + std::string("\n") + std::string(Server::conf), FILE_ERR, ERR);
		Logger::putMsg(std::string("TRY to open same file in 'conf' directory\n"));
		in.open((std::string("./conf/") + Server::conf).c_str());
	}
	if (!in.is_open())
	{
		Logger::putMsg(std::string(strerror(errno)) + std::string("\n") + std::string(Server::conf), FILE_ERR, ERR);
		return (false);
	}
	in.close();
	Logger::putMsg("parsing config file SUCCESS");
	return (true);
}

void Server::exitHandler(int sig)
{
	if (sig != SIGTERM && sig != -1)
		return ;
	std::cout << "clear exit\n";
	if (Server::main != NULL)
	{
		close(Server::main->sockFd);
		freeaddrinfo(Server::main->info);
		delete Server::main;
		Server::main = NULL;
	}
	if (sig == -1)
		Logger::putMsg("Exception Exit", FILE_ERR, ERR);
	else
		Logger::putMsg("Get SIGTERM. server shutdown");
	Logger::putMsg(DELIMITER_END);
	exit(0);
}

const t_listen& Server::getMain()
{
	if (Server::main)
		return *(Server::main);
	throw noMainStructException();
}
