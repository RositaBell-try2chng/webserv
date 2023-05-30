#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"
# include "MainClass.hpp"

class Servers;

typedef struct s_loc
{
	bool    flgGet;
	bool    flgPost;
	bool    flgDelete;

	std::string					location;
	std::map<int, std::string>	redirect;
	std::string					root;
	bool			        	dirListFlg;
	std::string		 			defFileIfDir;
	std::set<std::string>		CGIs; //extensions of file
	std::string					uploadPath;
	s_loc*				    	next;
}   t_loc;

typedef struct s_serv
{
	std::string                 ServerName;
	std::map<int, std::string>  errPages;
	ssize_t                     limitCLientBodySize;
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
	std::string		request;
	HTTP_Request	req_struct;

	std::string		response;
	HTTP_Answer		answ_struct;
	bool			responseReadyFlg;

	//cgi status
	bool		cgiConnectionFlg;
	pid_t		childPid;

	static t_serv*	cloneServList(t_serv const *src);
	static t_loc*	cloneLocList(t_loc const *src);
	static void		clearLocation(t_loc **loc);
	void			clearServ();
	static void		setMethods(t_loc* cur, std::string &src);
	static void		setCGIs(std::set<std::string> &dst, std::string &src);
	static void		fillErrorPages(std::vector<std::string> &E, t_serv *cur);
	static void		setRedirect(t_loc *cur, std::string src);
public:
	Server(std::string const& _host, std::string const& _port);
	~Server();

	t_serv*				serv;//public because need to change in another class
	std::string 	const&	getHost();
	std::string 	const&	getPort();

	std::string		const&	getReq();
	HTTP_Request	const&	getReq_struct();

	std::string		const&	getRes();
	HTTP_Answer		const&	getAnsw_struct();

	bool				respReady(); // get responseReadyFlg
	bool				cgiFlg(); // get cgiConnection
	pid_t				getChPid(); // get childPid
	void				setRespReady(bool flg);

	void				reqClear();
	void				resClear();
	void				addToReq(char const* src);
	void				setResponse(std::string const& src);
	void				setAnsw_struct(HTTP_Answer const& answ);
	void				setReq_struct(HTTP_Request const& req);
	void				resizeResponse(ssize_t res);

	Server*				clone() const;

	void			setServList(std::map<std::string, std::string> &S, std::map <std::string, std::map<std::string, std::string> > &L, std::vector<std::string> &SN, std::vector<std::string> &E);
	static t_loc*	setLocList(t_serv *s, std::map <std::string, std::map<std::string, std::string> > L);
};

#endif