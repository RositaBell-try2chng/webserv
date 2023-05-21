#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"

typedef struct s_loc
{
	bool    flgGet;
	bool    flgPost;
	bool    flgDelete;

	std::string redirect;
	std::string root;
	bool        dirListFlg;
	std::string defFileIfDir;
	std::string CGIs;
	std::string uploadPath;
	s_loc*      next;
}   t_loc;

typedef struct s_serv
{
	std::string                 ServerName;
	std::map<int, std::string>  errPages;
	size_t                      limitCLientBodySize;
	std::string                 root;
	t_loc*                      locList;
	s_serv*                     next;
}   t_serv;

class Server
{
private:
	Server();

	//conf
	std::string	host;
	std::string	port;

	//req/resp
	std::string	request;
	std::string response;
	bool		responseReadyFlg;

	//cgi status
	bool		cgiConnectionFlg;
	pid_t		childPid;

	static t_serv*	cloneServList(t_serv const *src);
	static t_loc*	cloneLocList(t_loc *src);
	void			clearLocation();
	void			clearServ();
public:
	Server(std::string const& _host, std::string const& _port);
	~Server();

	t_serv*				serv;//public because need to change in another class
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