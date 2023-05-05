#include "Server.hpp"
#include "Exceptions.hpp"
#include "Logger.hpp"

Server::Server() = default;
Server::~Server() = default;

//инициализируем статические переменный класса
std::string Server::conf = std::string(DEFAULT_CONF);
t_listen	*Server::main = nullptr;

void Server::startServer()
{
	std::cout << "start launching server\n";

	t_use	use;
	signal(SIGTERM, Server::exitHandler);
	Server::main = new t_listen;
	bzero(&Server::main->hints, sizeof(Server::main->hints));
	Server::main->hints.ai_family = AF_INET;
	Server::main->hints.ai_socktype = SOCK_STREAM;
	Server::main->hints.ai_flags = AI_PASSIVE;

	Server::main->stts = getaddrinfo(nullptr, "8080", &Server::main->hints, &Server::main->info);
	if (Server::main->stts != 0)
	{
		Logger::putErrMsg(std::string(gai_strerror(Server::main->stts)));
		throw exceptionGetAddrInfo();
	}

	Server::main->sockFd = socket(Server::main->info->ai_family, Server::main->info->ai_socktype, Server::main->info->ai_protocol);
	if (Server::main->sockFd < 0 || \
		bind(Server::main->sockFd, Server::main->info->ai_addr, Server::main->info->ai_addrlen) < 0 || \
		listen(Server::main->sockFd, 20) < 0)
		throw exceptionErrno();
	while (1)
	{
		use.addrSize = sizeof(use.addr);
		use.sockFd = accept(Server::main->sockFd, (struct sockaddr *)&use.addr, &use.addrSize);
		if (use.sockFd < 0)
		{
			Logger::putErrMsg(strerror(errno));
			continue;
		}
		use.resRecv = recv(use.sockFd, use.buf, 1000, 0);
		if (use.resRecv < 0)
			Logger::putErrMsg(strerror(errno));
		Logger::putQuery(use);
		if (send(use.sockFd, use.buf, use.resRecv, 0) < 0)
			Logger::putErrMsg(strerror(errno));
	}
}

bool Server::checkArgs(int args, char **argv)
{
	if (args == 1)
	{
		Logger::putErrMsg(std::string(NOT_PASSED_ARG));
		Logger::putInfoMsg(std::string(TRY_DEFAULT) + Server::conf, false);
	}
	else
		Server::conf = std::string(argv[1]);
	if (args > 2)
	{
		Logger::putErrMsg(std::string(TOO_MANY_ARGS) + Server::conf);
	}
	if (Server::conf.rfind(".conf") != Server::conf.length() - 5)
	{
		Logger::putErrMsg(std::string(WRONG_EXTENSION_OF_FILE));
		std::cerr << "Server have not started. More information in Logs\n";
		return (false);
	}
	return (true);
}

bool Server::parse()
{
	//fix me: add deep parsing of config file
	std::ifstream in;

	in.open(Server::conf);
	if (!in.is_open())
	{
		Logger::putErrMsg(std::string(strerror(errno)));
		Logger::putInfoMsg(std::string("TRY to open same file in 'conf' directory"), false);
		in.open(std::string("./conf/") + Server::conf);
	}
	if (!in.is_open())
	{
		Logger::putErrMsg(std::string(strerror(errno)));
		return (false);
	}
	in.close();
	return (true);
}

void Server::exitHandler(int sig)
{
	if (sig != SIGTERM)
		return ;
	std::cout << "clear exit\n";
	if (Server::main != nullptr)
	{
		freeaddrinfo(Server::main->info);
		delete Server::main;
		Server::main = nullptr;
	}
	exit(0);
}