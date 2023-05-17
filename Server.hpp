#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"

#define BUF_SIZE 4096

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
	Server(std::string const& _host, std::string const& _port);
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
	void				addToReq(char const* src);

	Server*				clone() const;
};

#endif