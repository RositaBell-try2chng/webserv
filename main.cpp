#include "Server.hpp"

int main(int argc, char **argv)
{
	if (!Server::checkArgs(argc, argv))
		return (Server::putErrMsg());
	if (!Server::parse(false))
		return (Server::putErrMsg());
	try
	{
		Server::startServer();
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	return (0);
}