#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"

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
	void				setResponse(std::string const& src);
	void				resizeResponse(int res);

	Server*				clone() const;
};

#endif