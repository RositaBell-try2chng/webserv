#include "Server.hpp"
#include "Logger.hpp"
#include "Exceptions.hpp"

int main(int argc, char **argv)
{
	Logger::putMsg(DELIMITER_START);
	if (!Server::checkArgs(argc, argv))
		return (1);
	if (!Server::parse())
		return (1);
	try
	{
		Server::startServer();
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		Server::exitHandler(-1);
	}
	return (0);
}