#include "HandlerRequest.hpp"

void HandlerRequest::mainHandler(Server &srv)
{
	std::cout << "main handler stage = " << srv.Stage << std::endl;
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
		case 5: {std::cout << srv.getResponse(); break;}
		default:
		{std::cout << "bad stage in mainHandler\n";}
	}
}

void HandlerRequest::start(Server &srv)
{
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

void ParserGAG(HTTP_Request &req, std::string &raw)
{
	raw.clear();
	req.stage = 53;
	req.host = std::string("127.0.0.1");
	req.port = std::string("8081");
	req.flg_cnnctn = 1;
	req.flg_te = 0;
	req.base.start_string.method = std::string("GET");
	req.base.start_string.uri = std::string("/html/index.html");
	req.base.start_string.prmtrs = std::string("");
	req.base.start_string.version = std::string("HTTP/1.1");
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("Connection"), std::string("keep-alive")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("Cache-Control"), std::string("max-age=0")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("sec-ch-ua"), std::string("Chromium;v=110,123")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("sec-ch-ua-mobile"), std::string("?0")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("sec-ch-ua-platform"), std::string("macOS")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("Upgrade-Insecure-Requests"), std::string("1")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("User-Agent"), std::string("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("Accept"), std::string("text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("Sec-Fetch-Site"), std::string("none")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("Sec-Fetch-Mode"), std::string("navigate")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("Sec-Fetch-User"), std::string("?1")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("Sec-Fetch-Dest"), std::string("document")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("Accept-Encoding"), std::string("gzip, deflate, br")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("Accept-Language"), std::string("en-US,en;q=0.9")));
	req.base.headers.insert(std::pair<std::string, std::string>(std::string("Cookie"), std::string("wp-settings-1=mfold%3Df; wp-settings-time-1=1682074510; user=Ahmed160.jpeg")));
// GET /html/index.html HTTP/1.1
// Connection: keep-alive
// Cache-Control: max-age=0
// sec-ch-ua: "Chromium";v="110", "Not A(Brand";v="24", "Google Chrome";v="110"
// sec-ch-ua-mobile: ?0
// sec-ch-ua-platform: "macOS"
// Upgrade-Insecure-Requests: 1
// User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7
// Sec-Fetch-Site: none
// Sec-Fetch-Mode: navigate
// Sec-Fetch-User: ?1
// Sec-Fetch-Dest: document
// Accept-Encoding: gzip, deflate, br
// Accept-Language: en-US,en;q=0.9
// Cookie: wp-settings-1=mfold%3Df; wp-settings-time-1=1682074510; user=Ahmed160.jpeg
}

void HandlerRequest::parserRequest(Server &srv)
{
	// Logger::putMsg(srv.getRequest(), FILE_REQ, REQ);
	//HTTP_Request::ft_strtoreq(*(srv.getReq_struct()), srv.getRequest());
	ParserGAG(*(srv.getReq_struct()), srv.getRequest());
	switch (srv.getReq_struct()->stage)
	{
		case 50: { srv.Stage = 1; srv.parseStage = 0; return; }
		case 51: { srv.Stage = 1; srv.parseStage = 1; return; }
		case 52: {
			srv.parseStage = 2;
			if (!HandlerRequest::isBodyNeed(srv))
			{ srv.getReq_struct()->stage = 53; srv.Stage = 3; srv.isChunkedRequest = false; HandlerRequest::handleRequest(srv); return;}
			else if (srv.getReq_struct()->flg_te == 0 || srv.getReq_struct()->body.empty())
			{ srv.Stage = 1; return; }
			else
				{ srv.Stage = 3; srv.isChunkedRequest = true; HandlerRequest::handleRequest(srv); return;}
		}
		case 53: { srv.Stage = 3; HandlerRequest::handleRequest(srv); return; }
		case 59: { HandlerRequest::prepareToSendError(srv); return; }
		default: {std::cout << "BAD Stage parser: " << srv.getReq_struct()->stage; return;}
	}
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
	t_serv		*servNode = srv.findServer(srv.getReq_struct()->host);
	
	if (!srv.isChunkedRequest && srv.getReq_struct()->content_lngth > servNode->limitCLientBodySize)
	{
			srv.getReq_struct()->answ_code[0] = 4;
			srv.getReq_struct()->answ_code[1] = 13;
			HandlerRequest::prepareToSendError(srv);
			return;
	}

	size_t		i = srv.getReq_struct()->base.start_string.uri.rfind('/');
	std::string	locName = srv.getReq_struct()->base.start_string.uri.substr(0, i);
	t_loc		*locNode = srv.findLocation(locName, servNode);

	//check method
	std::string method = srv.getReq_struct()->base.start_string.method;

	if ((method == "GET" && !locNode->flgGet) || (method == "POST" && !locNode->flgPost) || (method == "DELETE" && !locNode->flgDelete))
	{
		srv.Stage = 9;
		if (HandlerRequest::haveErrorPage(srv, servNode, 405))
			return;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 5;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	//check redirect
	if (!locNode->redirect.empty())
	{	
		HandlerRequest::redirectResponse(srv, locNode);
		return;
	}
	//handle methods
	std::string tmp = srv.getReq_struct()->base.start_string.uri;
	tmp.erase(0, i + 1);

	if (method == "GET")
		HandlerRequest::GET(srv, servNode, locNode, tmp);
	else if (method == "POST")
		HandlerRequest::POST(srv, servNode, locNode, tmp);
	else
		HandlerRequest::DELETE(srv, servNode, locNode, tmp);
}

void HandlerRequest::GET(Server &srv, t_serv *servNode, t_loc *locNode, std::string fileName)
{
	std::string fullPath;

	if (!servNode->root.empty() && servNode->root != "/")
		fullPath = servNode->root;
	else
		fullPath = std::string(".");
	if (locNode->location != "/")
		fullPath += locNode->location;
	fullPath += std::string("/") + fileName; //fix me: need uploadDir???
	if (access(fullPath.c_str(), R_OK) != 0)
	{
		if (HandlerRequest::haveErrorPage(srv, servNode, 403))
			return;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[0] = 3;
		srv.Stage = 9;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>(std::string("FILENAME"), fullPath));
	srv.getReq_struct()->base.start_string.uri = std::string("/CGIs/download.py");
	srv.Stage = 4;
	srv.CGIStage = 0;
	HandlerRequest::CGIHandler(srv);
}

void HandlerRequest::POST(Server &srv, t_serv *servNode, t_loc *locNode, std::string fileName)
{
	//check access to upload
	if (locNode->uploadPath.empty())
	{
		if (HandlerRequest::haveErrorPage(srv, servNode, 403))
			return;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[0] = 3;
		srv.Stage = 9;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	//check CGI
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
	if (HandlerRequest::haveErrorPage(srv, servNode, 501))
			return;
	srv.getReq_struct()->answ_code[0] = 5;
	srv.getReq_struct()->answ_code[0] = 1;
	srv.Stage = 9;
	HandlerRequest::prepareToSendError(srv);
}

void HandlerRequest::DELETE(Server &srv, t_serv *servNode, t_loc *locNode, std::string fileName)
{
	std::string fullPath;

	if (locNode->uploadPath.empty())
	{
		if (HandlerRequest::haveErrorPage(srv, servNode, 403))
			return;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[0] = 3;
		srv.Stage = 9;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	if (!servNode->root.empty())
		fullPath = servNode->root;
	else
		fullPath = std::string(".");
	if (locNode->location != "/")
		fullPath += locNode->location;
	fullPath += locNode->uploadPath + std::string("/") + fileName;
	if (access(fullPath.c_str(), W_OK) != 0)
	{
		if (HandlerRequest::haveErrorPage(srv, servNode, 403))
			return;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[0] = 3;
		srv.Stage = 9;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>(std::string("FILENAME"), fullPath));
	srv.getReq_struct()->base.start_string.uri = std::string("/CGIs/delete.sh");
	srv.Stage = 4;
	srv.CGIStage = 0;
	HandlerRequest::CGIHandler(srv);
}

bool HandlerRequest::haveErrorPage(Server &srv, t_serv *servNode, int code)
{
	std::map<int, std::string>::iterator	it = servNode->errPages.find(code);
	std::cout << "haveErrorPage Start\n";
	if (it == servNode->errPages.end())
		return (false);
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
	std::cout << "CGIhandler Start. stage = " << srv.CGIStage <<std::endl;
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
	std::cout << "startCGI Start\n";
	if (srv.getCGIptr())
	{
		delete srv.getCGIptr();
		srv.setCGIptr(NULL);
	}
	try 
	{
		srv.setCGIptr(new CGI());
		srv.CGIStage = srv.getCGIptr()->startCGI();
	}
	catch (std::exception &e) 
	{
		Logger::putMsg("BAD ALLOC while creating CGI", FILE_ERR, ERR);
		std::cout << e.what() << std::endl; //fix me: delete this
		srv.CGIStage = 9;
	}
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
	std::cout << "prepareToSendError Start\n";
	HTTP_Answer tmp;
	t_serv		*servNode = srv.findServer(srv.getReq_struct()->host);;

	if (srv.Stage == 99)
	{
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 8;
	}
	else if (srv.CGIStage == 9) //CGI error
	{
		srv.getReq_struct()->answ_code[0] = 5;
		srv.getReq_struct()->answ_code[1] = 0;
	}
	int code = srv.getReq_struct()->answ_code[0] * 100 + srv.getReq_struct()->answ_code[1];

	if (code != 408 && HandlerRequest::haveErrorPage(srv, servNode, code))
		return;
	tmp = HTTP_Answer::ft_reqtoansw(*(srv.getReq_struct()));
	srv.setResponse(HTTP_Answer::ft_answtostr(tmp));
	srv.setResponse("\r\n\r\n");
	std::cout << "response is |" << srv.getResponse() << "|"<< std::endl;
	if (srv.Stage == 9)
		srv.writeStage = 0;
	else
		srv.writeStage = 3;
	srv.Stage = 5;
	srv.isChunkedResponse = false;
}

HandlerRequest::HandlerRequest() {}
HandlerRequest::~HandlerRequest() {}