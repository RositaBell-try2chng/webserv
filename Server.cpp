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

void Server::resizeResponse(int res)
{
	this->response.erase(0, res);
	if (this->response.empty())
		this->setRespReady(false);
}

Server*	Server::clone() const
{
	Server* res = new Server(this->host, this->port);
	res->serv = Server::cloneServList(this->serv);
	return (res);
}

t_serv* Server::cloneServList(t_serv const *src)
{
	if (!src)
		return (NULL);

	t_serv	*res = new t_serv;
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

	t_loc *res = new t_loc;
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