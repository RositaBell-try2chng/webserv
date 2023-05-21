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
	t_serv	*curr = src;

	while (curr)
	{
		currRes->ServerName = curr->ServerName;
		currRes->errPages = std::map(curr->errPages);
		currRes->limitCLientBodySize = curr->limitCLientBodySize;
		currRes->root = std::string(curr->root);
		currRes->locList = Server::cloneLocList(curr->locList);
		currRes->next = NULL;
		curr = curr->next;
		if (!curr)
			continue;
		currRes->next = new t_serv;
		currRes = currRes->next;
	}
	return (res);
}

t_loc* Server::cloneLocList(t_loc *src)
{
	if (!src)
		return (NULL);

	t_loc *res = new t_loc;
	t_loc *currRes = res;
	t_loc *curr = src;

	while (curr)
	{
		currRes->flgGet = curr->flgGet;
		currRes->flgPost = curr->flgPost;
		currRes->flgDelete = curr->flgDelete;
		currRes->redirect = curr->redirect;
		currRes->root = curr->root;
		currRes->defFileIfDir = curr->defFileIfDir;
		currRes->CGIs = curr->CGIs;
		currRes->uploadPath = curr->uploadPath;
		currRes->next = NULL;
		curr = curr->next;
		if (!curr)
			continue;
		currRes->next = new t_serv;
		currRes = currRes->next;
	}
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