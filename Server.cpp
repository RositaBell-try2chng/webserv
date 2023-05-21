#include "Server.hpp"
#include "Exceptions.hpp"
#include "Logger.hpp"

Server::Server() {};
Server::~Server() {};

Server::Server(const std::string& _host, const std::string& _port)
{
	this->host = _host;
	this->port = _port;
	this->request = std::string();
	this->response = std::string();
	this->responseReadyFlg = false;
	this->cgiConnectionFlg = false;
	this->childPid = 0;
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

Server* Server::clone() const
{
	return (new Server(this->host, this->port));
}