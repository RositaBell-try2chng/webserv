#include "Server.hpp"
#include "Exceptions.hpp"
#include "Logger.hpp"
#include "ConfParser.hpp"

Server::Server() {};
Server::~Server()
{
	delete this->req_struct;
	delete this->answ_struct;
	if (!this->serv)
		return;
	this->clearServ();
};

Server::Server(const std::string& _host, const std::string& _port)
{
	this->host = _host;
	this->port = _port;
	this->request = std::string();
	this->response = std::string();
	this->serv = NULL;
	this->cntTryingSend = 0;
	this->ptrCGI = NULL;
	this->lastActionTime.tv_sec = time(NULL);
	this->lastActionTime.tv_usec = 0;
	this->Stage = 0;
	this->readStage = 0;
	this->writeStage = 0;
	this->parseStage = 0;
	this->CGIStage = 0;
	this->isChunkedRequest = false;
	this->isChunkedResponse = false;
	this->req_struct = new HTTP_Request();
	this->answ_struct = new HTTP_Answer();
}

void Server::addToReq(std::string src) { this->request += src; }

bool Server::checkCntTryingSend()
{
	this->cntTryingSend++;
	if (this->cntTryingSend < CNT_TRYING)
		return (false);
	return (true);
}

bool Server::checkTimeOut()
{
	if (time(NULL) - this->lastActionTime.tv_sec > TIMEOUT)
		return (true);
	return (false);
}

//finders
t_serv *Server::findServer(std::string const &str)
{
	t_serv *curServ = this->serv;

	if (str == this->host)
		return (curServ);
	while (curServ)
    {
        if (curServ->ServerName == host)
            return (curServ);
		curServ = curServ->next;
    }
    return(this->serv);
}

//finders
t_loc *Server::findLocation(std::string const &str, t_serv *src)
{
	t_loc *cur = src->locList;
	
	while (cur)
	{
		if (cur->location == str)
			return(cur);
		cur = cur->next;
	}
	return (src->locList);
}

//finders
std::string Server::findFile(std::string const &str, t_loc *loc)
{
	std::vector<std::string>::iterator it;
	
	it = find(loc->files.begin(), loc->files.end(), str);
	
	if (it != loc->files.end())
		return (*it);
	return(*loc->files.begin());
}

//for configs
void Server::setServList(std::map<std::string, std::string> &S, std::map <std::string, std::map<std::string, std::string> > &L, std::vector<std::string> &SN, std::vector<std::string> &E)
{
	std::map<std::string, std::string>::iterator itS;
	std::vector<std::string>::iterator it;
	t_serv*		cur;
	ssize_t		limit = -1;
	std::string	root("./www/");

	for (itS = S.begin(); itS != S.end(); itS++)
	{
		if (itS->first == "limitBodySize")
		{
			size_t i;
			std::stringstream ss(itS->second);
			ss >> limit;
			for (i = 0; i < itS->second.length(); i++)
			{
				if (!std::isdigit(itS->second[i]))
					break;
			}
			if (limit < 1 || i < itS->second.length()) {
				Logger::putMsg("limitBodySize is corrupted and not set", FILE_ERR, ERR);
				MainClass::exitHandler(0);
			}
		}
		else if (itS->first == "root")
			root = itS->second;
		else
		{
			Logger::putMsg("unknown parameter ignored:\n" + itS->first, FILE_ERR, ERR);
			MainClass::exitHandler(0);
		}
	}
	if (SN.empty())
		SN.push_back("");
	if (!this->serv)
	{
		this->serv = new t_serv;
		cur = this->serv;
	}
	else
	{
		cur = this->serv;
		while (cur->next)
			cur = cur->next;
		cur->next = new t_serv;
		cur = cur->next;
	}

	it = SN.begin();
	while (it != SN.end())
	{
		cur->ServerName = *it;
		cur->root = root;
		cur->limitCLientBodySize = limit;
		cur->locList = NULL;
		cur->next = NULL;
		//fill err pages map
		this->fillErrorPages(E, cur);
		//set location
		cur->locList = Server::setLocList(L);
		it++;
		if (it == SN.end())
			continue;
		cur->next = new t_serv;
		cur = cur->next;
	}
}

//for configs
void Server::fillErrorPages(std::vector<std::string> &E, t_serv *cur)
{
	std::vector<std::string>::iterator itE;

	for (itE = E.begin(); itE != E.end(); itE++)
	{
		std::string line1;
		std::string line2;

		line1 = *itE;
		ConfParser::splitLine(line1, line2);
		if (line2.empty() || line1.empty())
		{
			Logger::putMsg("BAD CONFIG for 'error page':\n" + line1 + ' ' + line2, FILE_ERR, ERR);
			MainClass::exitHandler(0);
		}
		for (size_t i = 0; i < line1.length(); i++)
		{
			if (!std::isdigit(line1[i]))
			{
				Logger::putMsg("BAD CONFIG for 'error page':\n" + line1 + ' ' + line2, FILE_ERR, ERR);
				MainClass::exitHandler(0);
			}
		}
		{
			std::stringstream	ss(line1);
			int					code;
			ss >> code;

			if (!(code <= 509 && code >= 400))
			{
				Logger::putMsg("BAD ERROR CODE:\n" + line1, FILE_ERR, ERR);
				MainClass::exitHandler(0);
			}
			if (cur->errPages.find(code) != cur->errPages.end())
			{
				Logger::putMsg("DOUBLE ERROR CODE:\n" + line1, FILE_ERR, ERR);
				MainClass::exitHandler(0);
			}
			else
				cur->errPages.insert(std::pair<int, std::string>(code, line2));
		}
	}
}

//for configs
t_loc* Server::setLocList(std::map <std::string, std::map<std::string, std::string> > L)
{
	std::map <std::string, std::map<std::string, std::string> >::iterator it;
	std::map <std::string, std::string>::iterator it2;
	t_loc* res;
	t_loc* cur;

	if (L.empty())
		return (NULL);
	res = new t_loc;
	cur = res;
	it = L.begin();
	while (it != L.end())
	{
		cur->next = NULL;
		cur->location = it->first;
		cur->root = std::string("");
		cur->dirListFlg = false;
		cur->defFileIfDir = std::string("");
		cur->uploadPath = std::string("");
		cur->flgGet = true;
		cur->flgDelete = true;
		cur->flgPost = true;
		for (it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			if (it2->first == "acceptedMethods")
				Server::setMethods(cur, it2->second);
			else if (it2->first == "root")
				cur->root = it2->second;
			else if (it2->first == "dirListOn")
			{
				if (it2->second == "on")
					cur->dirListFlg = true;
				else if (it2->second != "off")
				{
					Logger::putMsg("BAD CONFIG dirListOn:\n" + it2->second, FILE_ERR, ERR);
					MainClass::exitHandler(0);
				}
			}
			else if (it2->first == "defFileIfdir")
				cur->defFileIfDir = it2->second;
			else if (it2->first == "CGIs")
				Server::setCGIs(cur->CGIs, it2->second);
			else if (it2->first == "upload_path")
				cur->uploadPath = std::string(it2->second);
			else if (it2->first == "return")
				Server::setRedirect(cur, it2->second);
			else if (it2->first == "try_files")
				Server::setFiles(cur, it2->second);
			else
			{
				Logger::putMsg("BAD LOCATION CONFIG:\n" + it2->first, FILE_ERR, ERR);
				MainClass::exitHandler(0);
			}
		}
		it++;
		if (it == L.end())
			continue;
		cur->next = new t_loc;
		cur = cur->next;
	}
	return (res);
}

//for configs
void Server::setFiles(t_loc *cur, std::string src)
{
	std::stringstream ss(src);
	std::string	word;

	while (ss >> word)
		cur->files.push_back(word);
}

//for configs
void Server::setRedirect(t_loc *cur, std::string line1)
{
	std::string line2;
	std::string	tmp;
	int 		code;

	if (!cur->redirect.empty())
	{
		Logger::putMsg("BAD CONFIG DOUBLE 'return':\n" + line1 + " " + line2, FILE_ERR, ERR);
		MainClass::exitHandler(0);
	}
	ConfParser::splitLine(line1, line2);
	if (line1.empty() || line2.empty())
	{
		Logger::putMsg("BAD CONFIG 'return':\n" + line1 + " " + line2, FILE_ERR, ERR);
		MainClass::exitHandler(0);
	}
	for (size_t i = 0; i < line1.length(); i++)
	{
		if (!std::isdigit(line1[i]))
		{
			Logger::putMsg("BAD CONFIG 'return':\n" + line1 + " " + line2, FILE_ERR, ERR);
			MainClass::exitHandler(0);
		}
	}
	std::stringstream ss(line1);

	ss >> code;
	tmp.clear();
	ss >> tmp;
	if (!tmp.empty() || !(code >= 301 && code <= 308))
	{
		Logger::putMsg("BAD CONFIG 'return':\n" + line1 + " " + line2, FILE_ERR, ERR);
		MainClass::exitHandler(0);
	}
	cur->redirect.insert(std::pair<int, std::string>(code, line2));
}

//for configs
void Server::setMethods(t_loc *cur, std::string &src)
{
	std::string::size_type i;

	cur->flgGet = false;
	cur->flgDelete = false;
	cur->flgPost = false;

	i = src.find("GET");
	while (i != std::string::npos && !((i == 0 || std::isspace(src[i - 1])) && (i + 3 == src.length() || std::isspace(src[i + 3]))))
		i = src.find("GET", i + 3);
	if (i != std::string::npos)
	{
		cur->flgGet = true;
		src.erase(i, 3);
	}

	i = src.find("POST");
	while (i != std::string::npos && !((i == 0 || std::isspace(src[i - 1])) && (i + 4 == src.length() || std::isspace(src[i + 4]))))
		i = src.find("POST", i + 4);
	if (i != std::string::npos)
	{
		cur->flgPost = true;
		src.erase(i, 4);
	}

	i = src.find("DELETE");
	while (i != std::string::npos && !((i == 0 || std::isspace(src[i - 1])) && (i + 6 == src.length() || std::isspace(src[i + 6]))))
		i = src.find("DELETE", i + 6);
	if (i != std::string::npos)
	{
		cur->flgDelete = true;
		src.erase(i, 6);
	}

	delSpaces(src);
	if (src.empty())
		return;
	Logger::putMsg("BAD CONFIG:\ntoo many symbols at accepted methods:\n" + src, FILE_ERR, ERR);
	MainClass::exitHandler(0);
}

//for configs
void Server::setCGIs(std::set<std::string> &dst, std::string &src)
{
	std::string	line;

	while (!src.empty())
	{
		ConfParser::splitLine(src, line);
		dst.insert(src);
		src = line;
	}
}

//clones
Server*	Server::clone() const
{
	Server *res;

	res = new Server(this->host, this->port);
	res->serv = Server::cloneServList(this->serv);
	res->maxLimitBodiSize = this->maxLimitBodiSize;
	return (res);
}

t_serv* Server::cloneServList(t_serv const *src)
{
	if (!src)
		return (NULL);

	t_serv *res;
	res = new t_serv;

	t_serv	*currRes = res;

	while (src)
	{
		currRes->ServerName = src->ServerName;
		currRes->errPages = std::map<int, std::string>(src->errPages);
		currRes->limitCLientBodySize = src->limitCLientBodySize;
		currRes->root = std::string(src->root);
		currRes->locList = Server::cloneLocList(src->locList);
		currRes->next = NULL;
		src = src->next;
		if (!src)
			continue;
		currRes->next = new t_serv;
		currRes = currRes->next;
	}
	return (res);
}

t_loc* Server::cloneLocList(t_loc const *src)
{
	if (!src)
		return (NULL);

	t_loc *res;
	res = new t_loc;
	t_loc *currRes = res;

	while (src)
	{
		currRes->location = src->location;
		currRes->flgGet = src->flgGet;
		currRes->flgPost = src->flgPost;
		currRes->flgDelete = src->flgDelete;
		currRes->redirect = src->redirect;
		currRes->root = src->root;
		currRes->defFileIfDir = src->defFileIfDir;
		currRes->CGIs = src->CGIs;
		currRes->uploadPath = src->uploadPath;
		currRes->dirListFlg = src->dirListFlg;
		currRes->files = src->files;
		currRes->next = NULL;
		src = src->next;
		if (!src)
			continue;
		currRes->next = new t_loc;
		currRes = currRes->next;
	}
	return res;
}

//clears
void Server::clearAnsw_struct()
{
	this->answ_struct->body.clear();
	this->answ_struct->headers.clear();
	this->answ_struct->reason_phrase.clear();
	this->answ_struct->status_code.clear();
	this->answ_struct->version.clear();
}

//clears
void Server::clearReq_struct()
{
	this->req_struct->base.start_string.version.clear();
	this->req_struct->base.headers.clear();
	this->req_struct->body.clear();
	this->req_struct->answ_code[0] = -1;
	this->req_struct->answ_code[1] = -1;
	this->req_struct->base.start_string.method.clear();
	this->req_struct->base.start_string.uri.clear();
}

//clears
void Server::reqClear()
{
	this->Stage = 0;
	this->readStage = 0;
	this->writeStage = 0;
	this->parseStage = 0;
	this->CGIStage = 0;
	this->isChunkedRequest = false;
	this->isChunkedResponse = false;
	this->response.clear();
	this->CntTryingSendZero();
	if (!this->ptrCGI)
	{
		delete this->ptrCGI;
		this->ptrCGI = NULL;
	}
}

//clears
void Server::resClear()
{
	this->response.clear();
	this->isChunkedResponse = false;
	this->CntTryingSendZero();
	this->Stage = 0;
}

//clears
void    Server::clearLocation(t_loc	**loc)
{
	t_loc*  tmp;
	t_loc*  current;

	current = *loc;
	while (current)
	{
		tmp = current;
		current = current->next;
		delete tmp;
	}
	*loc = NULL;
}

//clears
void    Server::clearServ()
{
	t_serv* tmp;
	t_serv* current;

	current = this->serv;
	while (current)
	{
		tmp = current;
		current = current->next;
		if (tmp->locList)
			clearLocation(&tmp->locList);
		delete tmp;
	}
	this->serv = NULL;
}

//getters
const std::string & Server::getHost() { return (this->host); }
const std::string & Server::getPort() { return (this->port); }
std::string & Server::getRequest() { return (this->request); }
const std::string & Server::getResponse() { return (this->response); }
ssize_t				Server::getMaxBodySize() { return (this->maxLimitBodiSize); }
HTTP_Answer		*Server::getAnsw_struct() { return (this->answ_struct); }
HTTP_Request	*Server::getReq_struct() { return (this->req_struct); }
CGI				*Server::getCGIptr() { return (this->ptrCGI); }

//setters
void	Server::setCGIptr(CGI *src) {this->ptrCGI = src;}

void	Server::setMaxBodySize(ssize_t n) {this->maxLimitBodiSize = n;}

void	Server::setResponse(const std::string &src, bool flgReWrite)
{
	if (!flgReWrite)
		this->response += src;
	else
		this->response = src;
}

void	Server::CntTryingSendZero() {this->cntTryingSend = 0;}

void	Server::addChunkedSizeToResponse()
{
	std::string chunkSize = Size_tToString(this->response.length(), HEX_BASE);

	this->response = chunkSize + std::string("\r\n") + this->response + std::string("\r\n");
}

//copy structs Answ/Req
void Server::setAnsw_struct(HTTP_Answer const &src)
{
	this->answ_struct->body = src.body;
	this->answ_struct->headers = std::map<std::string, std::string>(src.headers);
	this->answ_struct->reason_phrase = src.reason_phrase;
	this->answ_struct->status_code = src.status_code;
	this->answ_struct->version = src.status_code;
}

//copy structs Answ/Req
void Server::setReq_struct(HTTP_Request const &src)
{
	this->req_struct->base.start_string.version = src.base.start_string.version;
	this->req_struct->base.headers = src.base.headers;
	this->req_struct->body = src.body;
	this->req_struct->answ_code[0] = src.answ_code[0];
	this->req_struct->answ_code[1] = src.answ_code[1];
	this->req_struct->base.start_string.method = src.base.start_string.method;
	this->req_struct->base.start_string.uri = src.base.start_string.uri;
}

void Server::updateLastActionTime()
{
	this->lastActionTime.tv_sec = time(NULL);
	this->lastActionTime.tv_usec = 0;
}