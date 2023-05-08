#include <iostream>

#include "Server.hpp"
#include "Logger.hpp"
#include "Exceptions.hpp"
//#include "./request_parse/Request_parse.hpp"


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

/*
int main(int argc, char **argv)
{
	std::string raw = "";

	std::cout << raw << "\n\n-----------------\n-----------------\n\n";

	Request_parse::HTTP_request req = Request_parse::ft_strtoreq(raw);

	std::cout << "method name:	" << req->method << std::endl;
	std::cout << "URI:			" << req->method << std::endl;
	std::cout << "version:		" << req->method << "\n" << std::endl;

	std::cout << "Host: " << req->headers.find("Host") << "\n" << std::endl;

	std::cout << "body: " << req->body << std::endl;
}
*/