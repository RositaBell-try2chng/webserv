#include "Server.hpp"
#include "Exceptions.hpp"
#include "Logger.hpp"

Server::Server() {};
Server::~Server() {};

//инициализируем статические переменный класса
std::string Server::conf = std::string(DEFAULT_CONF);
t_listen	*Server::main = nullptr;

void Server::startServer()
{
	Logger::putMsg("Start launching server");

	t_use	use;
	signal(SIGTERM, Server::exitHandler);
	Server::main = new t_listen;
	bzero(&Server::main->hints, sizeof(Server::main->hints));
	Server::main->hints.ai_family = AF_INET;
	Server::main->hints.ai_socktype = SOCK_STREAM;
	Server::main->hints.ai_flags = AI_PASSIVE;

	Server::main->stts = getaddrinfo(nullptr, "8080", &Server::main->hints, &Server::main->info);
	if (Server::main->stts != 0)
		throw exceptionGetAddrInfo();
	Server::main->sockFd = socket(Server::main->info->ai_family, Server::main->info->ai_socktype, Server::main->info->ai_protocol);
	if (Server::main->sockFd < 0 || \
		bind(Server::main->sockFd, Server::main->info->ai_addr, Server::main->info->ai_addrlen) < 0 || \
		listen(Server::main->sockFd, 20) < 0)
		throw exceptionErrno();
	Logger::putMsg("Server ready to accept connections, start main loop");
	while (1)
	{
		use.addrSize = sizeof(use.addr);
		use.sockFd = accept(Server::main->sockFd, (struct sockaddr *)&use.addr, &use.addrSize);
		if (use.sockFd < 0)
		{
			Logger::putMsg(strerror(errno), FILE_ERR, ERR);
			continue;
		}
		use.resRecv = recv(use.sockFd, use.buf, 1000, 0);
		if (use.resRecv < 0)
			Logger::putMsg(strerror(errno), FILE_ERR, ERR);
		Logger::putMsg(std::string(use.buf, use.resRecv), FILE_REQ, REQ);
		if (send(use.sockFd, use.buf, use.resRecv, 0) < 0)
			Logger::putMsg(strerror(errno), FILE_ERR, ERR);
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

	in.open(Server::conf);
	if (!in.is_open())
	{
		Logger::putMsg(std::string(strerror(errno)) + std::string("\n") + std::string(Server::conf), FILE_ERR, ERR);
		Logger::putMsg(std::string("TRY to open same file in 'conf' directory\n"));
		in.open(std::string("./conf/") + Server::conf);
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
	if (sig != SIGTERM)
		return ;
	std::cout << "clear exit\n";
	if (Server::main != nullptr)
	{
		freeaddrinfo(Server::main->info);
		delete Server::main;
		Server::main = nullptr;
	}
	Logger::putMsg("Get SIGTERM. server shutdown");
	exit(0);
}

const t_listen& Server::getMain() { return *(Server::main); }
