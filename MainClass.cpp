#include "MainClass.hpp"

//инициализируем static vars
int			MainClass::maxFd = 0;
Servers*	MainClass::allServers = NULL;

//fix me delete this
/*void printAllServ(Servers* src)
{
	std::map<int, Server*>::iterator it;

	for (it = src->getConnections(true).begin(); it != src->getConnections(true).end(); it++)
	{
		std::cout << "fd is: " << it->first << "; config is:\n";
		std::cout << "host:port is: " << it->second->getHost() << ":" << it->second->getPort() << std::endl;
		t_serv* cur;
		cur = it->second->serv;
		while (cur)
		{
			std::cout << "servername is: |" << cur->ServerName << "|" << std::endl;
			std::cout << "limit client body size is: " << cur->limitCLientBodySize << std::endl;
			std::cout << "root is: " << cur->root << std::endl;
			std::cout << "err pages is:\n";
			for (std::map<int, std::string>::iterator it2 = cur->errPages.begin(); it2 != cur->errPages.end(); it2++)
				std::cout << "code is: " << it2->first << "; page is: " << it2->second << std::endl;
			t_loc *cur2 = cur->locList;
			while (cur2)
			{
				std::cout << "location is: |" << cur2->location << "|" << std::endl;
				std::cout << "GET/POST/DELETE is :" << cur2->flgGet << "/" << cur2->flgPost << "/" << cur2->flgDelete << "\n";
				for (std::map<int, std::string>::iterator itR = cur2->redirect.begin(); itR != cur2->redirect.end(); itR++)
					std::cout << "redirect is: " << itR->first << " - " << itR->second << std::endl;
				std::cout << "root is: |" << cur2->root << "|\n";
				std::cout << "dirListFlg is: |" << cur2->dirListFlg << "|\n";
				std::cout << "defFileIfDir is: |" << cur2->defFileIfDir << "|\n";
				std::cout << "uploadPath is: |" << cur2->uploadPath << "|\n";
				for (std::set<std::string>::iterator it3 = cur2->CGIs.begin(); it3 != cur2->CGIs.end(); it3++)
					std::cout << "CGIs extention is: |" << *it3 << "|\n";
				cur2 = cur2->next;
			}
			cur = cur->next;
		}
	}
}*/

void MainClass::doIt(int args, char **argv)
{
	bool		flg;
	char*	   arg;

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
	timeval								timeout = {1, 0};
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
		// std::cout << "handle start in main LOOP\n";
		for (it = allServers->getConnections().begin(); it != allServers->getConnections().end(); it++)
		{
			HandlerRequest::mainHandler(*it->second);
			if (it->second->Stage == 1)
				MainClass::addToSet(it->first, &readFds);
			else if (it->second->Stage == 5)
				MainClass::addToSet(it->first, &writeFds);
			else if (it->second->Stage == 4 && it->second->CGIStage == 4)
				timeout.tv_sec = 1;
			else if (it->second->Stage == 4 && (it->second->CGIStage == 1 || it->second->CGIStage == 2 || it->second->CGIStage == 20))
				MainClass::addToSet(it->second->getCGIptr()->getPipeOutForward(), &writeFds);
			else if (it->second->Stage == 4 && (it->second->CGIStage == 5 || it->second->CGIStage == 50))
				MainClass::addToSet(it->second->getCGIptr()->getPipeInBack(), &readFds);
		}
		// std::cout << "add listen in main LOOP\n";
		//2.1.2 add listen fds
		for (it = allServers->getConnections(true).begin(); it != allServers->getConnections(true).end(); it++)
			MainClass::addToSet(it->first, &readFds);
		//select ловим готовые
		std::cout << "select in main LOOP\nMAX fd = " << MainClass::maxFd << std::endl;
		switch (select(MainClass::maxFd + 1, &readFds, &writeFds, NULL, &timeout))
		{
			case -1: { //select error
				Logger::putMsg(strerror(errno), FILE_ERR, ERR); 
				std::cout << "bad select!!!\n";
				continue; //fix me: need to continue or exit from server?
			}
			case 0: {std::cout << "timeout select\n"; continue;} //timeout. try another select
			default: {std::cout << "goahead select\n"; break;} //have something to do
		}
		// doing write/read depend on stage for all servers
		it = allServers->getConnections().begin();
		// std::cout << "while read/write in main LOOP\n";
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
				{std::cout << "bad stage for server: " << it->first << " stage is: " << it->second->Stage << std::endl;}
			}
		}
		// std::cout << "accept in main LOOP\n";
		acceptConnections(&readFds);
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
			std::cout << "accept from " << it->first << " to ";
			fd = accept(it->first, NULL, NULL);
			std::cout << fd << std::endl;
			if (fd < 0 || fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
				Logger::putMsg(strerror(errno), FILE_ERR, ERR);
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
			Logger::putMsg(std::string("error while recv ") + std::string(strerror(errno)), it->first, FILE_ERR, ERR);
			MainClass::closeConnection(it);
			return;
		}
		case 0: //нечего читать
		{
			Logger::putMsg(std::string("User closed connection ", it->first));
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
	std::string toSend(it->second->getResponse());

	if (toSend.empty()) // nothing to send = ERROR //should not to happen //fix me: test this
	{
		Logger::putMsg(std::string("NOTHING TO SEND"), it->first, FILE_ERR, ERR);
		std::cout << it->first << ": ERROR, NOTHING TO SEND\n";
		MainClass::closeConnection(it);
		return;
	}
	Logger::putMsg(toSend, FILE_REQ, REQ);
	sendRes = send(it->first, toSend.c_str(), toSend.length(), 0);
	switch (sendRes)
	{
		case -1: //error
		{
			Logger::putMsg(std::string("error while send ") + std::string(strerror(errno)), it->first, FILE_ERR, ERR);
			MainClass::closeConnection(it);
			return;
		}
		case 0:
		{
			if (it->second->checkCntTryingSend())
			{
				Logger::putMsg(std::string("error while send:\n3 times zero send in a raw"), it->first, FILE_ERR, ERR);
				MainClass::closeConnection(it);
				return;
			}
			break;
		}
		default:
		{
			it->second->updateLastActionTime();
			it->second->CntTryingSendZero();
			if (static_cast<size_t>(sendRes) == toSend.length())
			{
				if (it->second->writeStage == 3)
				{
					std::cerr << it->first << " has been closed by TIMEOUT\n";
					MainClass::closeConnection(it);
					return;
				}
				it->second->resClear();
				if (!it->second->isChunkedResponse || it->second->writeStage == 2)
					it->second->reqClear();
				else if (it->second->writeStage == 1 || (it->second->writeStage == 0 && it->second->isChunkedResponse))
					it->second->Stage = 4;
				else
					std::cout << it->first << ": bad stages in send:\nStage is: " << it->second->Stage << ". CGIStage is: " << it->second->CGIStage << std::endl;
			}
			else
			{
				toSend.erase(0, sendRes);
				it->second->setResponse(toSend);
			}
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
			if (it->second->CGIStage != 20)
				it->second->Stage = 1;
			break;
		}
		case 2: //2 - last send to pipe
		{
			it->second->CGIStage = it->second->getCGIptr()->sendToPipe(it, writes, true);
			it->second->getCGIptr()->prevStage = 2;
			break;
		}
		case 20: //repeat write
		{
			it->second->CGIStage = it->second->getCGIptr()->sendToPipe(it, writes, it->second->getCGIptr()->prevStage == 2);
			break;
		} 
		case 5: //read from pipe first
		{
			it->second->CGIStage = it->second->getCGIptr()->readFromPipe(it, reads);
			if (it->second->CGIStage == 50) //find body and add chunk size
				it->second->Stage = MainClass::setChunkedResponse(*it->second);
			else if (it->second->CGIStage == 6)
				it->second->setResponse(std::string("\r\n\r\n"));
			// std::cout << "stage is " << it->second->Stage << "  CGIStage = " << it->second->CGIStage << std::endl;
			break;
		}
		case 50: // read from pipe next chunk
		{
			it->second->CGIStage = it->second->getCGIptr()->readFromPipe(it, reads);
			it->second->addChunkedSizeToResponse();
			if (it->second->CGIStage == 6)
				it->second->setResponse(std::string("0\r\n\r\n"));
			break;
		}
		case 9:
		{
			std::cout << "ERROR CGI\n";
			std::cout << strerror(errno) << std::endl;
			it->second->Stage = 9;
			break;
		}
		default: { std:: cout << "BAD Stage CGI: " << it->second->Stage << " - " << it->second->CGIStage << std::endl;}
	}
}

int	MainClass::setChunkedResponse(Server &srv)
{
	std::string	StartStringHeaders;
	std::string	Body(srv.getResponse());
	std::string BodySize;
	size_t		i;

	i = Body.find("\r\n\r\n");
	if (i == std::string::npos)
	{
		srv.CGIStage = 5;
		return (4);
	}
	StartStringHeaders = Body.substr(0, i);
	//erase Content-Length because chunked response + add chunked Header
	i = StartStringHeaders.find("Content-Length");
	if (i != std::string::npos)
	{
		size_t j = StartStringHeaders.find("\r\n", i + 13);
		if (j != std::string::npos)
			StartStringHeaders.erase(i, j + 2 - i);
		else
			StartStringHeaders.erase(i, StartStringHeaders.length() - i);
	}
	StartStringHeaders.append(std::string("Transfer-Encoding: chunked\r\n\r\n"));
	Body.erase(0, i + 4);
	if (!Body.empty())
	{
		BodySize = Size_tToString(Body.length(), HEX_BASE) + std::string("\r\n");
		StartStringHeaders.append(BodySize + Body + std::string("\r\n"));
	}
	srv.setResponse((StartStringHeaders), true);
	return (5);
}

void MainClass::closeConnection(std::map<int, Server *>::iterator &it)
{
	int fd = it->first;

	it++;
	close(fd);
	MainClass::allServers->removeConnection(fd);
	std::cout << "close connection: " << fd << std::endl;
}

void MainClass::addToSet(int fd, fd_set *dst)
{
	std::cout << "add to SET: " << fd << std::endl;
	FD_SET(fd, dst);
	if (fd > MainClass::maxFd)
		MainClass::maxFd = fd;
}

bool MainClass::isCorrectRedirection(std::map<int, Server *>::iterator it)
{
	t_serv		*chNodeServ;
	t_loc		*chNodeLoc;
	std::string	redirectString;
	std::string name;
	std::string ip;
	std::string port;

	for (chNodeServ = it->second->serv; chNodeServ != NULL; chNodeServ = chNodeServ->next)
	{
		for (chNodeLoc = chNodeServ->locList; chNodeLoc != NULL; chNodeLoc = chNodeLoc->next)
		{
			if (chNodeLoc->redirect.empty())
				continue;
			if (!(chNodeLoc->files.empty() && chNodeLoc->root.empty() && chNodeLoc->files.empty() \
					&& !chNodeLoc->dirListFlg && chNodeLoc->defFileIfDir.empty() && chNodeLoc->CGIs.empty() && chNodeLoc->uploadPath.empty()))
			{
				Logger::putMsg("Bad configs with redirection: if 'return is set you should't use other parameters in location" + it->second->getHost() + ":" + it->second->getPort(), FILE_ERR, ERR);
				return (false);
			}
			redirectString = chNodeLoc->redirect.begin()->second;
			if (!MainClass::checkCorrectHostPortInRedirection(redirectString, name, port, ip))
				return (false);
			//find Server *
			for (std::map<int, Server *>::iterator itA = allServers->getConnections(true).begin(); itA != allServers->getConnections(true).end(); itA++)
			{
				if (ip == itA->second->getHost())
				{
					t_serv	*curServ = itA->second->findServer(name);
					t_loc	*curLoc = Server::findLocation(redirectString, curServ);
					if (!curLoc->redirect.empty())
					{
						Logger::putMsg("Two or more redirection in a raw: " + ip + ":" + port, FILE_ERR, ERR);
						return (false);
					}
				}
			}
		}
	}
	return (true);
}

bool MainClass::checkCorrectHostPortInRedirection(std::string &src, std::string &name, std::string &port, std::string &ip)
{
	std::string::size_type	i;
	std::string				tmp;

	i = src.find("http://");
	if (i != 0)
	{
		i = src.find("https://");
		if (i != 0)
			return (false);
		else
		{
			src.erase(0, 8);
			port = "443";
		}
	}
	else
	{
		port = "80";
		src.erase(0, 7);
	}
	i = src.find("www.");
	if (i == 0)
		src.erase(0, 4);
	i = src.find('/');
	tmp = src.substr(0, i);
	src.erase(0, i);
	i = src.rfind('/');
	src.resize(i);
	i = tmp.find(':');
	name = tmp.substr(0, i);
	tmp.erase(0, i + 1);
	if (!tmp.empty())
		port = tmp;
	//check port
	for (i = 0; i < 6 && i < port.length(); i++)
	{
		if (i >= 5 || !std::isdigit(port[i]))
		{
			Logger::putMsg("BAD Port in redirection: " + port, FILE_ERR, ERR);
			return (false);
		}
	}
	std::string portTmp(port);
	while (portTmp.length() < 5)
		portTmp.insert(portTmp.begin(), '0');
	if (portTmp > "65535")
	{
		Logger::putMsg("BAD Port in redirection: " + port, FILE_ERR, ERR);
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
		std::cout << "EXCEPTION exit. check LOGS\n";
	//system("leaks webserv");
	exit(0);
}