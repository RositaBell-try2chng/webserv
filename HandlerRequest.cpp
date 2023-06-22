#include "HandlerRequest.hpp"

void HandlerRequest::mainHandler(Server &srv)
{
	if (srv.checkTimeOut())
		srv.Stage = 99;
	switch (srv.Stage)
	{
		case 0: { HandlerRequest::start(srv); break; } //start
		case 2: { HandlerRequest::parserRequest(srv); break; } //need parse request
		case 3: { HandlerRequest::handleRequest(srv); break; } //need handle HTTP_reqStruct
		case 4: { HandlerRequest::CGIHandler(srv); break; } //need CGI
		case 9: 
		case 99: { HandlerRequest::prepareToSendError(srv); break; } //error
		case 1: //read/write to socket stage
		case 5: {break;}
		default:
		{ std::cout << "bad stage in mainHandler\n"; } //fix me: delete this or change on something
	}
}

void HandlerRequest::start(Server &srv)
{
	//create CGIptr
	try { srv.setCGIptr(new CGI()); }
	catch (std::exception &e)
	{ 
		srv.Stage = 9;
		srv.getReq_struct()->answ_code[0] = 5;
		srv.getReq_struct()->answ_code[0] = 0;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	//check if somthing else in request
	if (srv.getRequest().empty()) { srv.Stage = 1; return; }
	HandlerRequest::parserRequest(srv);
	switch (srv.parseStage)
	{
		case 0:
		case 1: { srv.Stage = 1; break; }
		case 2: { HandlerRequest::checkReadyToHandle(srv); break; }
		case 3: { srv.Stage = 3;  HandlerRequest::handleRequest(srv); break; }
		case 9: { srv.Stage = 9; HandlerRequest::prepareToSendError(srv); break; }
		default: { std::cout << "BAD parse Stage: " << srv.parseStage << std::endl; break; }
	}
}

void HandlerRequest::parserRequest(Server &srv)
{
	Logger::putMsg(srv.getRequest(), FILE_REQ, REQ);
	HTTP_Request::ft_strtoreq(*(srv.getReq_struct()), srv.getRequest());
	switch (srv.getReq_struct()->stage)
	{
		case 50: { srv.Stage = 1; srv.parseStage = 0; return; }
		case 51: { srv.Stage = 1; srv.parseStage = 1; return; }
		case 52: { srv.parseStage = 2; break; }
		case 53: { srv.parseStage = 3; break; }
		case 59: { HandlerRequest::prepareToSendError(srv); return; }
		default: {std::cout << "BAD Stage parser: " << srv.getReq_struct()->stage; return;}
	}
	//check body need/exists to handle
	if (srv.parseStage == 2 && HandlerRequest::isBodyNeed(srv) && (srv.getReq_struct()->flg_te == 0 || srv.getReq_struct()->body.empty()))
	{
		srv.Stage = 1;
		return;
	}
	if (srv.parseStage == 2 && !HandlerRequest::isBodyNeed(srv))
	{
		srv.getReq_struct()->stage = 3;
		srv.parseStage = 3;
	}
	srv.Stage = 3;
	HandlerRequest::handleRequest(srv);
}

bool HandlerRequest::isBodyNeed(Server &srv)
{
	if (srv.getReq_struct()->base.headers.find("Content-Length") != srv.getReq_struct()->base.headers.end() || \
		srv.getReq_struct()->base.headers.find("Transfer-Encoding") != srv.getReq_struct()->base.headers.end())
		return (true);
	return (false);
}

void HandlerRequest::handleRequest(Server &srv)
{
	std::cout << "handle request\n";
	//find serv
	t_serv		*servNode = srv.findServer(srv.getReq_struct()->host);
	//check too large body
	if ((srv.getReq_struct()->flg_te == 0 && srv.getReq_struct()->content_lngth > servNode->limitCLientBodySize) || \
		(srv.getReq_struct()->flg_te == 1 && srv.getReq_struct()->chunk_size > servNode->limitCLientBodySize))//fix me: check if need to skip some from fd
	{
		srv.Stage = 9;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 13;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	//split locName && fileName + find location
	size_t		i = srv.getReq_struct()->base.start_string.uri.rfind('/');
	std::string	locName = srv.getReq_struct()->base.start_string.uri.substr(0, i);
	std::string	fileName = srv.getReq_struct()->base.start_string.uri;
	fileName.erase(0, i + 1);
	srv.getCGIptr()->PATH_INFO = srv.getReq_struct()->base.start_string.uri;
	srv.getCGIptr()->SCRIPT_NAME = fileName;
	t_loc		*locNode = srv.findLocation(locName, servNode);
	//check if method allowed
	std::string method = srv.getReq_struct()->base.start_string.method;
	if ((method == "GET" && !locNode->flgGet) || (method == "POST" && !locNode->flgPost) || (method == "DELETE" && !locNode->flgDelete))
	{
		srv.Stage = 9;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 5;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	//check directory request
	if (i >= srv.getReq_struct()->base.start_string.uri.length() - 1)
	{
		HandlerRequest::handleDirectoryResponse(srv, locNode);
		return;
	}
	//try to find file
	if (!Server::findFile(fileName, servNode, locNode))
	{
		//check redirect
		if (!locNode->redirect.empty())
			HandlerRequest::redirectResponse(srv, locNode);
		else
		{
			srv.Stage = 9;
			srv.getReq_struct()->answ_code[0] = 4;
			srv.getReq_struct()->answ_code[1] = 4;
			HandlerRequest::prepareToSendError(srv);
		}
		return;
	}
	//start handle methods
	if (method == "GET")
		HandlerRequest::GET(srv, fileName);
	else if (method == "POST")
		HandlerRequest::POST(srv, locNode, fileName);
	else
		HandlerRequest::DELETE(srv, locNode, fileName);
}

void HandlerRequest::handleDirectoryResponse(Server &srv, t_loc *locNode)
{
	std::cout << "handle directory request\n";
	if (locNode->dirListFlg)
	{
		;//do some from Egor's file
		std::cout << "DIR LISTING ON on: " << locNode->location << std::endl;
		return;
	}
	if (!locNode->indexFile.empty())
	{
		srv.getReq_struct()->base.start_string.uri += locNode->indexFile;
		HandlerRequest::handleRequest(srv);
		return;
	}
	if (!locNode->redirect.empty())
	{
		HandlerRequest::redirectResponse(srv, locNode);
		return;
	}
	srv.Stage = 9;
	srv.getReq_struct()->answ_code[0] = 4;
	srv.getReq_struct()->answ_code[1] = 3;
	HandlerRequest::prepareToSendError(srv);
}

void HandlerRequest::GET(Server &srv, std::string &fileName)
{
	if (access(fileName.c_str(), R_OK) != 0)
	{
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[0] = 3;
		srv.Stage = 9;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>(std::string("FILENAME"), fileName));
	srv.getCGIptr()->PATH_TRANSLATED = std::string("./CGIs/download.py");
	srv.getCGIptr()->PATH_INFO = std::string("/CGIs/download.py");
	srv.getCGIptr()->SCRIPT_NAME = std::string("download.py");
	srv.Stage = 4;
	srv.CGIStage = 0;
	HandlerRequest::CGIHandler(srv);
}

void HandlerRequest::POST(Server &srv, t_loc *locNode, std::string &fileName)
{
	//check access to upload
	if (locNode->uploadPath.empty())
	{
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[0] = 3;
		srv.Stage = 9;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	//check extension
	std::string	tmp = fileName;
	size_t		j = tmp.rfind('.');
	if (j != std::string::npos)
	{
		tmp.erase(0, j);
		if (locNode->CGIs.find(tmp) != locNode->CGIs.end())
		{
			srv.Stage = 4;
			srv.CGIStage = 0;
			HandlerRequest::CGIHandler(srv);
			return;
		}
	}
	//can't POST without CGI
	srv.getReq_struct()->answ_code[0] = 5;
	srv.getReq_struct()->answ_code[0] = 1;
	srv.Stage = 9;
	HandlerRequest::prepareToSendError(srv);
}

void HandlerRequest::DELETE(Server &srv, t_loc *locNode, std::string &fileName)
{
	std::string fullPath;

	if (locNode->uploadPath.empty())
	{
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[0] = 3;
		srv.Stage = 9;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	if (access(fileName.c_str(), W_OK) != 0)
	{
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[0] = 3;
		srv.Stage = 9;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>(std::string("FILENAME"), fileName));
	srv.getCGIptr()->PATH_INFO = std::string("/CGIs/delete.sh");
	srv.getCGIptr()->PATH_TRANSLATED = std::string("./CGIs/delete.sh");
	srv.getCGIptr()->SCRIPT_NAME = std::string("delete.sh");
	srv.Stage = 4;
	srv.CGIStage = 0;
	HandlerRequest::CGIHandler(srv);
}

bool HandlerRequest::haveErrorPage(Server &srv, t_serv *servNode, int code)
{
	std::map<int, std::string>::iterator	it = servNode->errPages.find(code);

	if (it == servNode->errPages.end())
		return (false);
	if (access("./CGIs/download.py", X_OK) == -1)
	{
		Logger::putMsg("download.py have no access to execute => error pages can't be open", FILE_ERR, ERR);
		return(false);
	}
	std::string	response("HTTP/1.1 303 See Other\r\nLocation: ");
	response += it->second + std::string("\r\n\r\n");
	srv.Stage = 5;
	return (true);
}

void HandlerRequest::redirectResponse(Server &srv, t_loc *locNode)
{
	std::map<int, std::string>::iterator	it = locNode->redirect.begin();
	int										code = it->first;
	std::string								dst = it->second;
	std::string								response("HTTP/1.1 ");

	switch (code)
	{
		case 301: { response += std::string("301 Moved Permanently"); break; }
		case 302: { response += std::string("302 Found"); break; }
		case 303: { response += std::string("303 See Other"); break; }
		case 304: { response += std::string("304 Not Modified"); break; }
		case 305: { response += std::string("305 Use Proxy"); break; }
		case 306: { response += std::string("306 Switch Proxy"); break; }
		case 307: { response += std::string("307 Temporary Redirect"); break; }
		case 308: { response += std::string("308 Permanent Redirect"); break; }
		default: {std::cerr << "BAD CODE REDIRECT: " << code << std::endl; srv.Stage = 9; return; }
	}
	response += std::string("\r\nLocation: ") + dst + std::string("\r\n\r\n");
	srv.setResponse(response);
	srv.Stage = 5;
	srv.writeStage = 0;
}

void HandlerRequest::CGIHandler(Server &srv)
{
	if (!(srv.CGIStage == 0 || srv.CGIStage == 6) && srv.getCGIptr() == NULL)
    {
        std::cout << "BAD stages or ptr in CGI Handler: " << srv.CGIStage << std::endl;
        return;
    }
	switch (srv.CGIStage)
	{
		case 0: { HandlerRequest::startCGI(srv); break; }
		case 1:
		case 2: { HandlerRequest::prepareToSendCGI(srv); break; }
		case 3: //3 - fork + launch script
		{
			srv.CGIStage = srv.getCGIptr()->ForkCGI(srv);
			if (srv.CGIStage == 9)
				HandlerRequest::CGIerrorManager(srv);
			break;
		}
		case 4: //wait end of pid
		{
			srv.CGIStage = srv.getCGIptr()->waitingCGI();
			if (srv.CGIStage == 9)
				HandlerRequest::CGIerrorManager(srv);
			break;
		}
		case 6: //end of pipe - clean all
		{
			delete srv.getCGIptr();
			srv.setCGIptr(NULL);
			srv.CGIStage = 0;
			srv.Stage = 5;
		}
		case 9: { HandlerRequest::CGIerrorManager(srv); break; }
		default: {std::cout << "bad stage in cgi handler\n";}
	}
}

void HandlerRequest::startCGI(Server &srv)
{
	if (access(srv.getCGIptr()->PATH_TRANSLATED.c_str(), F_OK) == -1)
	{
		srv.Stage = 9;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 4;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	if (access(srv.getCGIptr()->PATH_TRANSLATED.c_str(), X_OK) == -1)
	{
		srv.Stage = 9;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 3;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	srv.CGIStage = srv.getCGIptr()->startCGI();
	if (srv.CGIStage == 9)
		HandlerRequest::CGIerrorManager(srv);
	else
		HandlerRequest::prepareToSendCGI(srv);
}

void	HandlerRequest::CGIerrorManager(Server &srv)
{
	if (srv.getCGIptr())
	{
		delete srv.getCGIptr();
		srv.setCGIptr(NULL);
	}
	srv.Stage = 9;
	srv.CGIStage = 9;
	HandlerRequest::prepareToSendError(srv);
}

void HandlerRequest::prepareToSendCGI(Server &srv)
{
	if (!HandlerRequest::isBodyNeed(srv))
	{
		srv.CGIStage = srv.getCGIptr()->ForkCGI(srv);
		if (srv.CGIStage == 9)
			HandlerRequest::CGIerrorManager(srv);
		return;
	}
	if (srv.getReq_struct()->body.empty())
	{
		srv.Stage = 1;
		return;
	}
	srv.Stage = 4;
	if (srv.parseStage == 3)
		srv.CGIStage = 2;
	else
		srv.CGIStage = 1;
}

void HandlerRequest::checkReadyToHandle(Server &srv)
{
	std::cout << "checkReadyToHandle Start\n";
	if (srv.getReq_struct()->flg_te == 0)
	{
		srv.Stage = 1;
		return;
	}
	srv.isChunkedRequest = true;
	srv.readStage = 1;
	if (!srv.getReq_struct()->body.empty())
	{
		srv.Stage = 3;
		HandlerRequest::handleRequest(srv);
	}
	else
		srv.Stage = 1;
}

void HandlerRequest::prepareToSendError(Server &srv)
{
	HTTP_Answer tmp;
	t_serv		*servNode = srv.findServer(srv.getReq_struct()->host);;

	srv.writeStage = 0;
	if (srv.Stage == 99) //runtime error
	{
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 8;
		srv.writeStage = 3;
	}
	else if (srv.CGIStage == 9) //CGI error
	{
		srv.getReq_struct()->answ_code[0] = 5;
		srv.getReq_struct()->answ_code[1] = 0;
	}
	int code = srv.getReq_struct()->answ_code[0] * 100 + srv.getReq_struct()->answ_code[1];

	std::cout << "prepare to send error start. code = " << code << std::endl;
	if (HandlerRequest::haveErrorPage(srv, servNode, code))
		return;
	tmp = HTTP_Answer::ft_reqtoansw(*(srv.getReq_struct()));
	srv.setResponse(HTTP_Answer::ft_answtostr(tmp), true);
	srv.setResponse("\r\n\r\n");
	srv.Stage = 5;
	srv.isChunkedResponse = false;
}

HandlerRequest::HandlerRequest() {}
HandlerRequest::~HandlerRequest() {}