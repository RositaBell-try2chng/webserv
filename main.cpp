#include <iostream>

#include "Server.hpp"
#include "Logger.hpp"
#include "Exceptions.hpp"
#include "./request_parse/HTTP_Request.hpp"

/*
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
*/

int main(int argc, char **argv)
{


	std::string raw = "GET / HTTP/1.1\r\nHost: alizar.habrahabr.ru";

	std::cout << raw << "\n\n-------------------------\n-------------------------\n\n";

	HTTP_Request req;

	HTTP_Request::ft_strtoreq(raw, req);
	std::cout << "method name:	" << req.method << std::endl;
	std::cout << "URI:			" << req.method << std::endl;
	std::cout << "version:		" << req.method << "\n" << std::endl;

	//std::cout << "Host: " << req.headers.find("Host") << "\n" << std::endl;

	std::cout << "body:			" << req.body << std::endl;
}
