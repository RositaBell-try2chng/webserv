#include "Server.hpp"

//инициализируем статические переменный класса
std::string Server::errorMsg = std::string("none");
std::string Server::configFile = std::string("./conf/serv.conf");
int			Server::socketFd = -1;


void Server::startServer()
{
	Server::putMsg("Start Server");
	//Server::socketFd = socket(AF_INET, SOCK_STREAM);
}

int Server::putErrMsg(std::string msg)
{
	std::cerr << "Error: " << msg << std::endl;
	return (1);
}

int Server::putMsg(std::string msg)
{
	std::cout << "INFO: " << msg << std::endl;
	return (1);
}

bool Server::checkArgs(int args, char **argv)
{
	size_t		i;

	if (args == 1)
		Server::putMsg(std::string(NOT_PASSED_ARG) + std::string(TRY_DEFAULT) + Server::configFile);
	else
		Server::configFile = std::string(argv[1]);
	if (args > 2)
		Server::putMsg(std::string(TOO_MANY_ARGS) + std::string(argv[1]));
	if (Server::configFile.rfind(".conf") != Server::configFile.length() - 5)
	{
		Server::errorMsg = std::string(WRONG_EXTENSION_OF_FILE);
		return (false);
	}
	return (true);
}

bool Server::parse(bool flgSecond)
{
	//fix me: add deep parsing of config file
	std::ifstream in(Server::configFile);

	if (!in.is_open() && flgSecond)
	{
		Server::errorMsg = strerror(errno);
		return (false);
	}
	if (!in.is_open() && !flgSecond)
	{
		Server::configFile = std::string("./conf/") + Server::configFile;
		Server::putMsg(std::string("File not found, trying to find file in conf directory:\n") + Server::configFile);
		return Server::parse(true);
	}
	in.close();
	return (true);
}