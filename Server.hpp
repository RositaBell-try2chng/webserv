#ifndef SERVER_HPP
# define SERVER_HPP

# include "webserv.hpp"
# include "MainClass.hpp"
# include "./request_parse/HTTP_Request.hpp"
# include "./request_parse/HTTP_Answer.hpp"
# include "CGI.hpp"

class Servers;
class CGI;

typedef struct s_loc
{
	bool    flgGet;
	bool    flgPost;
	bool    flgDelete;

	std::string					location;
	std::map<int, std::string>	redirect;
	std::vector<std::string>	files;
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
	ssize_t     maxLimitBodiSize;

	//req/resp
	std::string		request;
	HTTP_Request	req_struct;

	std::string		response;
	std::string		chunkToSend;
	HTTP_Answer		answ_struct;

	int				Stage;

	char cntTryingRecv;
	char cntTryingSend;

	CGI				*ptrCGI;

	static void		setMethods(t_loc* cur, std::string &src);
	static void		setCGIs(std::set<std::string> &dst, std::string &src);
	static void		fillErrorPages(std::vector<std::string> &E, t_serv *cur);
	static void		setRedirect(t_loc *cur, std::string line1);
	static void		setFiles(t_loc *cur, std::string src);


	static ssize_t	cutSize(std::string &src);
//clears
	static t_serv*	cloneServList(t_serv const *src);
	static t_loc*	cloneLocList(t_loc const *src);
	static void		clearLocation(t_loc **loc);
	void			clearServ();
public:
	Server(std::string const& _host, std::string const& _port);
	~Server();

	t_serv*					serv;//public because need to change in another class

	std::string 	const&	getHost();
	std::string 	const&	getPort();

	std::string		const&	getRequest();
	HTTP_Request	&		getReq_struct();

	std::string		const&	getResponse();
	HTTP_Answer		&		getAnsw_struct();
	std::string		const&	getChunkToSend();

	int					getStage();
	ssize_t				getMaxBodySize();
	CGI					*getCGIptr();

	t_loc*				findLocation(std::string const &str, t_serv *src);
	static t_serv*		findServer(std::string const &str);
	static std::string	findFile(std::string const &str, t_loc *loc);

	void				setSGIptr(CGI *src);
	void				setStage(int n);
	void				setAnsw_struct(HTTP_Answer const &src);
	void				setReq_struct(HTTP_Request const &src);
	void				setMaxBodySize(ssize_t n);
	void				setChunkToSend(std::string const &src);
	bool				addToChunk(std::string src);

//clears
	void				reqClear();
	void				resClear();
	void				clearAnsw_struct();
	void				clearReq_struct();

	void				addToReq(std::string src);
	bool				addToChunk(std::string src);
	void				setResponse(std::string const src);
	void				resizeResponse(ssize_t res);

	bool 				checkCntTryingRecv();
	bool 				checkCntTryingSend();
	void				CntTryingRecvZero();
	void				CntTryingSendZero();

	Server*				clone() const;

	void			setServList(std::map<std::string, std::string> &S, std::map <std::string, std::map<std::string, std::string> > &L, std::vector<std::string> &SN, std::vector<std::string> &E);
	static t_loc*	setLocList(t_serv *s, std::map <std::string, std::map<std::string, std::string> > L);
};

#endif