#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"

#define BUF_SIZE 4096

//Mesages
#define TOO_MANY_ARGS			"Too many args\nUse only first argument\n"
#define WRONG_EXTENSION_OF_FILE	"Wrong extension of config file: "
#define NOT_PASSED_ARG			"Config file doesn't passed."
#define TRY_DEFAULT				"Trying to use default config file:\n"

#define DEFAULT_CONF "./conf/serv.conf"

# define DELIMITER_START "//////////////////\n\nSTART OF ANOTHER LAUNCH\n\n//////////////////"
# define DELIMITER_END "//////////////////\n\nEND OF ANOTHER LAUNCH\n\n//////////////////"

#define ANSWER "HTTP/1.1 200 OK"
#define ANSWER_LEN 15

class Server
{
private:
	Server();

	std::string	host;
	std::string	port;
	std::string	request;
	std::string response;
	bool		responseReadyFlg;
	bool		cgiConnectionFlg;
	pid_t		childPid;
public:
	Server(std::string const& _host, std::string const& _port)
	~Server();

	std::string const&	getHost();
	std::string const&	getPort();
	std::string	const&	getReq();
	std::string	const&	getRes();
	bool				respReady(); // get responseReadyFlg
	bool				cgiFlg(); // get cgiConnection
	pid_t				getChPid(); // get childPid
	void				setRespReady(bool flg);

	void				reqClear();
	void				resClear();
	void				addToRes(std::string const& src);

	Server*				clone();
};

#endif