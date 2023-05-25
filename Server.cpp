#include "Server.hpp"
#include "Exceptions.hpp"
#include "Logger.hpp"
#include "ConfParser.hpp"

Server::Server() {};
Server::~Server()
{
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
	this->responseReadyFlg = false;
	this->cgiConnectionFlg = false;
	this->childPid = 0;
	this->serv = NULL;
	this->serv->locList = NULL;
}

//getters
const std::string & Server::getHost() { return (this->host); }
const std::string & Server::getPort() { return (this->port); }
const std::string & Server::getReq() { return (this->request); }
const std::string & Server::getRes() { return (this->response); }
bool Server::respReady() { return (this->responseReadyFlg); }
bool Server::cgiFlg() { return (this->cgiConnectionFlg); }
pid_t Server::getChPid() { return (this->childPid); }

//setters
void Server::setRespReady(bool flg) {this->responseReadyFlg = flg;}

void Server::reqClear() { this->request.clear(); }
void Server::resClear()
{
	this->response.clear();
	this->responseReadyFlg = false;
}

void Server::addToReq(const char *src)
{
	this->request += std::string(src);
}

void Server::setResponse(const std::string &src)
{
	this->response = src;
	this->setRespReady(true);
}

void Server::resizeResponse(ssize_t res)
{
	this->response.erase(0, res);
	if (this->response.empty())
		this->setRespReady(false);
}

Server*	Server::clone() const
{
	Server *res;

	res = new Server(this->host, this->port);
	res->serv = Server::cloneServList(this->serv);
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
		currRes->flgGet = src->flgGet;
		currRes->flgPost = src->flgPost;
		currRes->flgDelete = src->flgDelete;
		currRes->redirect = src->redirect;
		currRes->root = src->root;
		currRes->defFileIfDir = src->defFileIfDir;
		currRes->CGIs = src->CGIs;
		currRes->uploadPath = src->uploadPath;
		currRes->next = NULL;
		src = src->next;
		if (!src)
			continue;
		currRes->next = new t_loc;
		currRes = currRes->next;
	}
	return res;
}

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

void Server::setServList(std::map<std::string, std::string> &S, std::map <std::string, std::map<std::string, std::string> > &L, std::vector<std::string> &SN, std::vector<std::string> &E)
{
	std::map<std::string, std::string>::iterator itS;
	std::vector<std::string>::iterator it;
	std::vector<std::string>::iterator itE;
	t_serv*		cur;
	size_t		limit = -1;
	std::string	root = std::string("./www/");
	char		c;

	for (itS = S.begin(); itS != S.end(); it++)
	{
		if (itS->first == "limitBodySize")
		{
			std::stringstream ss(itS->second);
			ss >> limit;
			if (ss.get(c)) {
				Logger::putMsg("limitBodySize is corrupted and not set", FILE_ERR, ERR);
				limit = -1;
			}
		}
		else if (itS->first == "root")
			root = itS->second;
		else
			Logger::putMsg("unknown parameter ignored:\n" + itS->first, FILE_ERR, ERR);
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
		cur->next = NULL;
		//fill err pages map
		for (itE = E.begin(); itE != E.end(); it++)
		{
			std::string line1;
			std::string line2;

			line1 = *itE;
			ConfParser::splitLine(line1, line2);
			if (line1.empty() || line2.empty())
				continue;
			{
				std::stringstream	ss;
				char 				c2;
				int					code;

				ss << line1;
				ss >> code;
				if (ss.get(c2))
					Logger::putMsg("BAD ERROR CODE:\n" + line1, FILE_ERR, ERR);
				else if (cur->errPages.find(code) != cur->errPages.end())
					Logger::putMsg("DOUBLE ERROR CODE:\n" + line1, FILE_ERR, ERR);
				else
					cur->errPages.insert(std::pair<int, std::string>(code, line2));
			}
		}
		//set location
		cur->locList = Server::setLocList(cur, L);
		it++;
		if (it == SN.end())
			continue;
		cur->next = new t_serv;
		cur = cur->next;
	}
}

t_loc* Server::setLocList(t_serv* s, std::map <std::string, std::map<std::string, std::string> > &L)
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
		cur->root = s->root;
		cur->dirListFlg = false;
		cur->defFileIfDir = std::string("");
		cur->uploadPath = std::string("");
		cur->redirect = std::string("");
		cur->flgGet = true;
		cur->flgDelete = true;
		cur->flgPost = true;
		for (it2 = it->second.begin(); it2 != it->second.end(); it++)
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
					Logger::putMsg("BAD CONFIG dirListOn:\n" + it2->second, FILE_ERR, ERR);
			}
			else if (it2->first == "defFileIfdir")
				cur->defFileIfDir = it2->second;
			else if (it2->first == "CGIs")
				Server::setCGIs(cur->CGIs, it2->second);
			else if (it2->first == "upload_path")
				cur->uploadPath = std::string(it2->second);
			else if (it2->first == "return")
				cur->redirect = std::string(it2->second);
			else
				Logger::putMsg("BAD LOCATION CONFIG:\n" + it2->first, FILE_ERR, ERR);
		}
		it++;
		if (it == L.end())
			continue;
		cur->next = new t_loc;
		cur = cur->next;
	}
	return (res);
}

void Server::setMethods(t_loc *cur, std::string &src)
{
	std::string::size_type i;

	cur->flgGet = false;
	cur->flgDelete = false;
	cur->flgPost = false;

	i = src.find("GET");
	while (i != std::string::npos && !((i == 0 || std::isspace(src[i - 1])) && std::isspace(src[i + 3])))
		i = src.find("GET");
	if (i != std::string::npos)
		cur->flgGet = true;

	i = src.find("POST");
	while (i != std::string::npos && !((i == 0 || std::isspace(src[i - 1])) && std::isspace(src[i + 4])))
		i = src.find("POST");
	if (i != std::string::npos)
		cur->flgPost = true;

	i = src.find("DELETE");
	while (i != std::string::npos && !((i == 0 || std::isspace(src[i - 1])) && std::isspace(src[i + 6])))
		i = src.find("DELETE");
	if (i != std::string::npos)
		cur->flgDelete = true;
}

void Server::setCGIs(std::set<std::string> &dst, std::string &src)
{
	std::string	line;

	while (!src.empty())
	{
		ConfParser::splitLine(src, line);
		dst.insert(line);
	}
}