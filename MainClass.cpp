#include "MainClass.hpp"

//инициализируем static vars
int			MainClass::maxFd = 0;
Servers*	MainClass::allServers = NULL;

void MainClass::doIt(int args, char **argv)
{
	bool	flg;
	char*	arg;

	arg = NULL;
	flg = ConfParser::checkArgs(args, argv);
	if (flg)
		arg = argv[1];
	try
	{
		ConfParser::parseConf(arg, &MainClass::allServers);
	}
	catch (std::exception &e)
	{
		std::cerr << "PARSE CONFIG FAILED\n" << e.what() << std::endl;
		return;
	}

	for (std::map<int, Server *>::iterator it = allServers->getConnections(true).begin(); it != allServers->getConnections(true).end(); it++)
	{
		if (!MainClass::isCorrectCGIs(it))
		{
			std::cerr << "BAD CGIs configs, check logs\n";
			return;
		}
		if (!MainClass::isCorrectRedirection(it))
		{
			std::cerr << "BAD redirection, check logs\n";
			return;
		}
	}
	std::cout << "parse config SUCCESS\n";
	if (!MainClass::allServers || MainClass::allServers->getConnections(true).empty())
	{
		std::cerr << "NO SERVER CREATED, CHECK YOUR CONFIG\n";
		return;
	}
	std::cout << "Server has been launched!\n";

	MainClass::mainLoop();
	//printAllServ(MainClass::allServers);
}

void MainClass::mainLoop()
{
	timeval								timeout = {1, 50};
	fd_set								readFds;
	fd_set								writeFds;
	std::map<int, Server*>::iterator	it;

	while (true)
	{
		//2.0 clean sets
		FD_ZERO(&readFds);
		FD_ZERO(&writeFds);
		
		MainClass::maxFd = -1;
		timeout.tv_sec = 15;
		//handle all request until read/write or waiting child
		for (it = allServers->getConnections().begin(); it != allServers->getConnections().end(); it++)
		{
			HandlerRequest::mainHandler(*it->second);
			if (it->second->Stage == 1)
				MainClass::addToSet(it->first, &readFds);
			else if (it->second->Stage == 5)
				MainClass::addToSet(it->first, &writeFds);
			else if (it->second->Stage == 4 && it->second->CGIStage == 4)
			{
				timeout.tv_sec = 0;
				MainClass::addToSet(it->second->getCGIptr()->getPipeInBack(), &readFds);
			}
			else if (it->second->Stage == 4 && (it->second->CGIStage == 1 || it->second->CGIStage == 2 || it->second->CGIStage == 20))
				MainClass::addToSet(it->second->getCGIptr()->getPipeOutForward(), &writeFds);
			else if (it->second->Stage == 4 && (it->second->CGIStage == 5 || it->second->CGIStage == 50))
				MainClass::addToSet(it->second->getCGIptr()->getPipeInBack(), &readFds);
		}
		//2.1.2 add listen fds
		for (it = allServers->getConnections(true).begin(); it != allServers->getConnections(true).end(); it++)
			MainClass::addToSet(it->first, &readFds);
		//select
		switch (select(MainClass::maxFd + 1, &readFds, &writeFds, NULL, &timeout))
		{
			case -1: {
				//Logger::putMsg(strerror(errno), FILE_ERR, ERR); 
				continue;
			}
			case 0: {continue;} //timeout. try another select
			default: {break;} //have something to do
		}
		acceptConnections(&readFds);
		// doing write/read depend on stage for all servers
		it = allServers->getConnections().begin();
		while (it != allServers->getConnections().end())
		{
			switch (it->second->Stage)
			{
				//read from socket
				case 0: { ++it; continue;}
				case 1: { MainClass::readRequest(it, &readFds); break; }
				//CGI
				case 4: { MainClass::CGIHandlerReadWrite(it, &readFds, &writeFds); ++it; break; }
				//write to socket
				case 5: { MainClass::sendResponse(it, &writeFds); break; }
				default://all servers should be in write/read stage or CGI, if not then ERROR
				{ MainClass::setBadStageError(*(it->second)); ++it; }
			}
		}
	}
}


bool MainClass::acceptConnections(fd_set *readFds)
{
	int									fd;
	bool								flgConnection = false;
	std::map<int, Server *>::iterator	it;

	for (it = allServers->getConnections(true).begin(); it != allServers->getConnections(true).end(); it++)
	{
		if (FD_ISSET(it->first, readFds))
		{
			fd = accept(it->first, NULL, NULL);
			if (fd < 0 || fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
				std::cout << "BAD ACCEPT\n";
			else
				MainClass::allServers->addConnection(fd, *(it->second));
			flgConnection = true;
		}
	}
	return (flgConnection);
}

void MainClass::readRequest(std::map<int, Server *>::iterator &it, fd_set *reads)
{
	if (!FD_ISSET(it->first, reads))
	{
		++it;
		return;
	}
	ssize_t	recvRes;
	char	buf[BUF_SIZE];

	recvRes = recv(it->first, buf, BUF_SIZE, 0);
	switch (recvRes)
	{
		case -1: //ошибка чтения
		{
			//Logger::putMsg(std::string("error while recv ") + std::string(strerror(errno)), it->first, FILE_ERR, ERR);
			MainClass::closeConnection(it);
			return;
		}
		case 0: //нечего читать
		{
			//Logger::putMsg(std::string("User closed connection ", it->first));
			MainClass::closeConnection(it);
			return;
		}
		default: {
			it->second->updateLastActionTime();
			it->second->addToReq(std::string(buf, recvRes));
			it->second->Stage = 2;
		}
	}
	it++;
}

void MainClass::sendResponse(std::map<int, Server *>::iterator &it, fd_set *writes)
{
	if (!FD_ISSET(it->first, writes))
	{
		++it;
		return;
	}
	ssize_t sendRes;
	if (it->second->getResponse().empty()) // nothing to send = ERROR
	{
		std::cout << it->first << ": ERROR, NOTHING TO SEND\n";
		MainClass::closeConnection(it);
		return;
	}
	Logger::putMsg(it->second->getResponse(), FILE_REQ, RES);
	sendRes = send(it->first, it->second->getResponse().c_str(), it->second->getResponse().length(), 0);
	switch (sendRes)
	{
		case -1: //error
		{
			//Logger::putMsg(std::string("error while send ") + std::string(strerror(errno)), it->first, FILE_ERR, ERR);
			MainClass::closeConnection(it);
			return;
		}
		case 0:
		{
			if (it->second->checkCntTryingSend())
			{
				//Logger::putMsg(std::string("error while send:\n3 times zero send in a raw"), it->first, FILE_ERR, ERR);
				MainClass::closeConnection(it);
				return;
			}
			break;
		}
		default:
		{
			it->second->updateLastActionTime();
			it->second->CntTryingSendZero();
			if (static_cast<size_t>(sendRes) == it->second->getResponse().length())
			{
				if (it->second->writeStage == 3)
				{
					MainClass::closeConnection(it);
					return;
				}
				if (!it->second->isChunkedResponse || it->second->writeStage == 2)
					it->second->reqClear();
				else if (it->second->writeStage == 1 || (it->second->writeStage == 0 && it->second->isChunkedResponse))
					it->second->Stage = 4;
				else
					MainClass::setBadStageError(*(it->second));// error
			}
			it->second->getResponse().erase(0, sendRes);
			break;
		}
	}
	it++;
}

void MainClass::CGIHandlerReadWrite(std::map<int, Server *>::iterator &it, fd_set *reads, fd_set *writes)
{
	switch (it->second->CGIStage)
	{
		case 1: //1 - send to pipe
		{
			it->second->CGIStage = it->second->getCGIptr()->sendToPipe(it, writes, false);
			it->second->getCGIptr()->prevStage = 1;
			if (it->second->CGIStage == 9)
				it->second->Stage = 9;
			else if (it->second->CGIStage != 20)
				it->second->Stage = 1;
			break;
		}
		case 2: //2 - last send to pipe
		{
			it->second->CGIStage = it->second->getCGIptr()->sendToPipe(it, writes, true);
			it->second->getCGIptr()->prevStage = 2;
			if (it->second->CGIStage == 9)
				it->second->Stage = 9;
			break;
		}
		case 20: //repeat write
		{
			it->second->CGIStage = it->second->getCGIptr()->sendToPipe(it, writes, it->second->getCGIptr()->prevStage == 2);
			if (it->second->CGIStage == 9)
				it->second->Stage = 9;
			break;
		}
		case 4: //pipe is not finished
		{
			if (time(NULL) - it->second->getCGIptr()->timeCGIStarted.tv_sec > 2)
			{
				int res = it->second->getCGIptr()->readFromPipe(it, reads, true);
				if (res != 6)
					break;
				it->second->CGIStage = 6;
				it->second->Stage = MainClass::setContentLengthResponse(*it->second);
			}
			break;
		}
		case 5: //read from pipe first
		{
			it->second->CGIStage = it->second->getCGIptr()->readFromPipe(it, reads);
			if (it->second->CGIStage == 50) //find body and add chunk size
				it->second->Stage = MainClass::setChunkedResponse(*it->second);
			else if (it->second->CGIStage == 6)
			{
				it->second->getCGIptr()->pid = -1;
				it->second->Stage = MainClass::setContentLengthResponse(*it->second);
			}
			break;
		}
		case 50: // read from pipe next chunk
		{
			it->second->CGIStage = it->second->getCGIptr()->readFromPipe(it, reads);
			it->second->addChunkedSizeToResponse();
			if (it->second->CGIStage == 6)
			{
				it->second->getCGIptr()->pid = -1;
				it->second->setResponse(std::string("0\r\n\r\n"));
				it->second->writeStage = 2;
			}
			break;
		}
		case 9:
		{
			std::cout << "ERROR CGI\n";
			std::cout << strerror(errno) << std::endl;
			it->second->Stage = 9;
			break;
		}
		default: { MainClass::setBadStageError(*(it->second)); }
	}
}

void printStages(Server &srv)
{
	std::cout << "BAD STAGES!\n" << "Stage is: " << srv.Stage << "\nParseStage is: " << srv.parseStage << "\nreadStage is: " << srv.readStage;
	std::cout << "\nCGIStage is: " << srv.CGIStage << "\nwriteStage is: " << srv.writeStage << std::endl;
}

void MainClass::setBadStageError(Server &srv)
{
	printStages(srv);
	srv.Stage = 9;
	srv.getReq_struct()->answ_code[0] = 5;
	srv.getReq_struct()->answ_code[1] = 0;
}

int	MainClass::setContentLengthResponse(Server &srv)
{
	std::string	resp(srv.getResponse());
	size_t		i = resp.find("\r\n\r\n");
	if (i == std::string::npos)
	{
		srv.getReq_struct()->answ_code[0] = 5;
		srv.getReq_struct()->answ_code[1] = 0;
		return (9);
	}
	//check if content length exists
	std::string startAndHead = resp.substr(0, i);
	std::stringstream ss(startAndHead);
	std::string tmp;
	size_t	j;
	std::getline(ss, tmp);
	j = tmp.find("HTTP/1.");
	if (j != 0)
	{
		j = startAndHead.find("Status: ");
		tmp = startAndHead.substr(j + 7);
		std::stringstream strstream(tmp);
		int code;
		strstream >> code;
		int answ_code[2];
		answ_code[0] = code / 100;
		answ_code[1] = code % 100;
		resp.insert(0, "HTTP/1.1 " + Size_tToString(code, DEC_BASE) + std::string(" ") + ft_reason_phrase(answ_code) + std::string("\r\n"));
	}
	i = resp.find("\r\n\r\n");
	size_t		bodyLen = resp.length() - (i + 4);
	std::string	bodyLenStr = Size_tToString(bodyLen, DEC_BASE);

	resp.insert(i, std::string("\r\nContent-Length: ") + bodyLenStr);
	srv.setResponse(resp, true);
	return (5);
}

int	MainClass::setChunkedResponse(Server &srv)
{
	std::string	StartStringHeaders;
	std::string	Body(srv.getResponse());
	std::string BodySize;
	size_t		i;

	i = Body.find("HTTP/1.");
	if (i != 0)
	{
		i = Body.find("Status:");
		std::string tmp = Body.substr(i + 7);
		std::stringstream ss(tmp);
		int code;
		ss >> code;
		int answ_code[2];
		answ_code[0] = code / 100;
		answ_code[1] = code % 100;
		Body.insert(0, "HTTP/1.1 " + Size_tToString(code, DEC_BASE) + std::string(" ") + ft_reason_phrase(answ_code) + std::string("\r\n"));
	}
	i = Body.find("\r\n\r\n");
	if (i == std::string::npos)
	{
		srv.CGIStage = 5;
		return (4);
	}
	StartStringHeaders = Body.substr(0, i + 4);
	Body.erase(0, i + 4);
	//erase Content-Length because chunked response + add chunked Header
	i = StartStringHeaders.find("Content-Length");
	if (i != std::string::npos)
	{
		size_t j = StartStringHeaders.find("\r\n", i + 13);
		StartStringHeaders.erase(i, j + 2 - i);
	}
	i = StartStringHeaders.find("\r\n");
	StartStringHeaders.insert(i + 2, std::string("Transfer-Encoding: chunked\r\n"));
	if (!Body.empty())
	{
		BodySize = Size_tToString(Body.length(), HEX_BASE) + std::string("\r\n");
		StartStringHeaders.append(BodySize + Body + std::string("\r\n"));
	}
	srv.setResponse((StartStringHeaders), true);
	srv.isChunkedResponse = true;
	srv.writeStage = 0;
	return (5);
}

void MainClass::closeConnection(std::map<int, Server *>::iterator &it)
{
	int fd = it->first;

	it++;
	close(fd);
	MainClass::allServers->removeConnection(fd);
}

void MainClass::addToSet(int fd, fd_set *dst)
{
	FD_SET(fd, dst);
	if (fd > MainClass::maxFd)
		MainClass::maxFd = fd;
}

bool MainClass::isCorrectCGIs(std::map<int, Server *>::iterator it)
{
	t_serv	*curServ;
	t_loc	*curLoc;

	for (curServ = it->second->serv; curServ != NULL; curServ = curServ->next)
	{
		for (curLoc = curServ->locList; curLoc != NULL; curLoc = curLoc->next)
		{
			if (curLoc->CGIs.size() != curLoc->CGIs_path.size())
			{
				Logger::putMsg("size of CGIs and CGIs_path are diffrent", FILE_ERR, ERR);
				return (false);
			}
			for (size_t i = 0; i < curLoc->CGIs_path.size(); i++)
			{
				if (access(curLoc->CGIs_path[i].c_str(), X_OK) == -1)
				{
					Logger::putMsg(std::string("CGIs_path ") + curLoc->CGIs_path[i] + std::string(" Not found or have no access X"), FILE_ERR, ERR);
					return (false);
				}
			}
		}
	}
	return (true);
}

bool MainClass::isCorrectRedirection(std::map<int, Server *>::iterator it)
{
	t_serv		*chNodeServ;
	t_loc		*chNodeLoc;
	std::string name;
	std::string ip(it->second->getHost());
	std::string port(it->second->getPort());
	std::string location;

	for (chNodeServ = it->second->serv; chNodeServ != NULL; chNodeServ = chNodeServ->next)
	{
		if (chNodeServ->limitCLientBodySize == -1)
			chNodeServ->limitCLientBodySize = 10000000;
		for (chNodeLoc = chNodeServ->locList; chNodeLoc != NULL; chNodeLoc = chNodeLoc->next)
		{
			if (chNodeLoc->redirect.empty())
				continue;
			if (!(chNodeLoc->root.empty() && !chNodeLoc->dirListFlg && chNodeLoc->defFileIfDir.empty() && chNodeLoc->CGIs.empty() && chNodeLoc->uploadPath.empty()))
			{
				//Logger::putMsg("Bad configs with redirection: if 'return' is set you can use only 'try_files' and 'acceptedMethods': " + it->second->getHost() + ":" + it->second->getPort(), FILE_ERR, ERR);
				return (false);
			}
			name = std::string(chNodeServ->ServerName);
			if (!MainClass::checkCorrectHostPortInRedirection(chNodeLoc, name, port, ip, location))
				return (false);
			//find Server *
			for (std::map<int, Server *>::iterator itA = allServers->getConnections(true).begin(); itA != allServers->getConnections(true).end(); itA++)
			{
				if (ip == itA->second->getHost() && port == itA->second->getPort())
				{
					t_serv	*curServ = itA->second->findServer(name);
					t_loc	*curLoc = Server::findLocation(location, curServ);
					if (!curLoc->redirect.empty())
						return (false);
				}
			}
		}
	}
	return (true);
}

bool MainClass::checkCorrectHostPortInRedirection(t_loc *locNode, std::string &name, std::string &port, std::string &ip, std::string &location)
{
	std::string::size_type	i;
	std::string				tmp;
	std::string				src = locNode->redirect.begin()->second;
	std::string				tmpPort;

	i = src.find("http://");
	if (i == 0)
	{
		tmpPort = "80";
		src.erase(0, 7);
	}
	else
	{
		i = src.find("https://");
		if (i == 0)
		{
			tmpPort = "443";
			src.erase(0, 8);
		}
	}
	i = src.find("www.");
	if (i == 0)
		src.erase(0, 4);
	//separate host:port = tmp && location = src
	i = src.find('/');
	tmp = src.substr(0, i);
	src.erase(0, i);
	i = src.rfind('/');
	src.resize(i);
	location = src;
	if (!tmp.empty())
	{
		i = tmp.find(':');
		name = tmp.substr(0, i);
		tmp.erase(0, i + 1);
		if (!tmp.empty())
			port = tmp;
		else if (!tmpPort.empty())
			port = tmpPort;
	}
	//check port
	for (i = 0; i < 6 && i < port.length(); i++)
	{
		if (i >= 5 || !std::isdigit(port[i]))
		{
			//Logger::putMsg("BAD Port in redirection: " + port, FILE_ERR, ERR);
			return (false);
		}
	}
	std::string portTmp(port);
	while (portTmp.length() < 5)
		portTmp.insert(portTmp.begin(), '0');
	if (portTmp > "65535")
	{
		//Logger::putMsg("BAD Port in redirection: " + port, FILE_ERR, ERR);
		return (false);
	}
	//check host/name
	if (checkCorrectHost(name))
		ip = name;
	if (name == "localhost")
		ip = "127.0.0.1";
	return (true);
}

void MainClass::exitHandler(int sig)
{
	if (sig != SIGTERM && sig != 0)
		return;
	if (sig == SIGTERM)
		std::cout << "ExitHandler: SIGTERM received\n";
	else
		std::cout << "EXCEPTION exit.\n";
	//system("leaks webserv");
	exit(0);
}