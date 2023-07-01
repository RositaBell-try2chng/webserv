#include "HandlerRequest.hpp"

void HandlerRequest::mainHandler(Server &srv)
{
	if (srv.checkTimeOut())
	{
		// std::cout << it->first << " closed by TIMOUT\n";
		srv.Stage = 99;
	}
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
		{ MainClass::setBadStageError(srv); }
	}
}

void HandlerRequest::start(Server &srv)
{
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
		default: { MainClass::setBadStageError(srv); break; }
	}

}

void HandlerRequest::parserRequest(Server &srv)
{
	Logger::putMsg(srv.getRequest(), FILE_REQ, REQ);
	HTTP_Request::ft_strtoreq(*(srv.getReq_struct()), srv.getRequest());
	if (/*srv.getReq_struct()->answ_code[0] == 4 || srv.getReq_struct()->answ_code[0] == 5 || */srv.getReq_struct()->stage == 59)
	{
		srv.Stage = 9;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	else if (srv.getReq_struct()->stage == 54)
	{
		srv.Stage = 98;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	switch (srv.getReq_struct()->stage)
	{
		case 50: { srv.Stage = 1; srv.parseStage = 0; return; }
		case 51: { srv.Stage = 1; srv.parseStage = 1; return; }
		case 52: { srv.parseStage = 2; break; }
		case 53: { srv.parseStage = 3; break; }
		case 59: { HandlerRequest::prepareToSendError(srv); return; }
		default: { MainClass::setBadStageError(srv); return;}
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
	//find serv
	t_serv		*servNode = srv.findServer(srv.getReq_struct()->host);
	//check too large body
	if ((srv.getReq_struct()->flg_te == 0 && srv.getReq_struct()->content_lngth > servNode->limitCLientBodySize) || \
		(srv.getReq_struct()->flg_te == 1 && srv.getReq_struct()->chunk_size > servNode->limitCLientBodySize))
	{
		srv.Stage = 9;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 13;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	//split locName && fileName + find location
	t_loc		*locNode = servNode->locList;
	size_t		i = srv.getReq_struct()->base.start_string.uri.length();
	std::string	locName;
	std::string	fileName;
	std::string realPath(servNode->root);
	while (i != std::string::npos && i != 0)
	{
		locName = srv.getReq_struct()->base.start_string.uri.substr(0, i);
		locNode = srv.findLocation(locName, servNode);
		if (locNode != servNode->locList)
			break;
		i = srv.getReq_struct()->base.start_string.uri.rfind('/', i - 1);
	}

	if (!locNode->root.empty() && locNode->root != "/")
		realPath += locNode->root;
	else if (locNode->location != "/")
		realPath += locNode->location;
	fileName = srv.getReq_struct()->base.start_string.uri.substr(i);
	srv.getCGIptr()->PATH_INFO = std::string("");//srv.getReq_struct()->base.start_string.uri;
	srv.getCGIptr()->SCRIPT_NAME = fileName;
	std::string method = srv.getReq_struct()->base.start_string.method;
	//check exists methods not ALLOWED
	if (method == "CONNECT" || method == "HEAD" || method == "OPTIONS" || method == "PATCH" || method == "PUT" || method == "TRACE")
	{
		srv.Stage = 9;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 5;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	else if (method != "GET" && method != "POST" && method != "DELETE")
	{
		srv.Stage = 9;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 5;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	//check if method allowed
	if ((method == "GET" && !locNode->flgGet) || (method == "POST" && !locNode->flgPost) || (method == "DELETE" && !locNode->flgDelete))
	{
		srv.Stage = 9;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 5;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	//check directory request
	DIR *dirPTR = opendir((realPath + fileName).c_str());
	if (dirPTR != NULL)
	{
		closedir(dirPTR);
		HandlerRequest::handleDirectoryResponse(srv, realPath + fileName, locNode);
		return;
	}
	//try to find file
	bool	CGIflg = false;
	realPath += fileName;
	if (!Server::findFile(srv, realPath, servNode, locNode, CGIflg))
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
		HandlerRequest::GET(srv, servNode, realPath, CGIflg);
	else if (method == "POST")
		HandlerRequest::POST(srv, servNode, locNode, realPath);
	else if (method == "DELETE")
		HandlerRequest::DELETE(srv, servNode, locNode, realPath);
	else
	{
		srv.Stage = 9;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 5;
		HandlerRequest::prepareToSendError(srv);
	}
}

void HandlerRequest::handleDirectoryResponse(Server &srv, std::string realPath, t_loc *locNode)
{
	if (locNode->dirListFlg)
	{
		srv.resp.body = ft_dirlisting(realPath, srv);
		if (srv.Stage == 9)
		{
			HandlerRequest::prepareToSendError(srv);
			return;
		}
		srv.setResponse("HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-Length: " + Size_tToString(srv.resp.body.length(), DEC_BASE) + "\r\n\r\n" + srv.resp.body);
		srv.Stage = 5;
		srv.writeStage = 2;
		return;
	}
	if (!locNode->indexFile.empty())
	{
		srv.getReq_struct()->base.start_string.uri += std::string("/") + locNode->indexFile;
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

void HandlerRequest::GET(Server &srv, t_serv *servNode, std::string &fileName, bool CGIflg)
{
	if (!CGIflg && access(fileName.c_str(), R_OK) == -1)
	{
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 3;
		srv.Stage = 9;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	if (!CGIflg)
	{
		srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>(std::string("FILENAME"), fileName));
		srv.getCGIptr()->PATH_TO_SGI = std::string("/usr/local/bin/python3.9");
		srv.getCGIptr()->PATH_TRANSLATED = std::string(servNode->root + "/CGIs/download.py");
		srv.getCGIptr()->PATH_INFO = std::string("");
		srv.getCGIptr()->SCRIPT_NAME = std::string("download.py");
	}
	else
		srv.getCGIptr()->PATH_TRANSLATED = fileName;
	srv.Stage = 4;
	srv.CGIStage = 0;
	HandlerRequest::CGIHandler(srv);
}

void HandlerRequest::POST(Server &srv, t_serv *servNode, t_loc *locNode, std::string &fileName)
{
	std::string fullPath(servNode->root + "/CGIs/upload.py");
	//check access to upload
	if (access(fullPath.c_str(), X_OK) == -1)
	{
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 3;
		srv.Stage = 9;
		HandlerRequest::prepareToSendError(srv);
		return;
	}
	//set upload path
	std::string uplPath;

	if (servNode->root != "/")
		uplPath = servNode->root;
	else
		uplPath = std::string("./");
	if (locNode->uploadPath != "/")
		uplPath += locNode->uploadPath;
	uplPath += std::string("/");
	srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>("UPLOAD_PATH", uplPath));
	//change body if need / setup params for CGI
	char result = HandlerRequest::changeBodyIfBoundryIsSet(srv.getReq_struct());
	if (fileName == fullPath)
		HandlerRequest::addFileNameEnv(srv.getReq_struct(), result);
	else
	{
		std::string	tmp;
		size_t		i;
		size_t		j;

		std::map<std::string, std::string>::iterator it = srv.getReq_struct()->base.headers.find("Content-Type");
		if (it != srv.getReq_struct()->base.headers.end())
		{
			tmp = it->second;
			i = tmp.rfind('/');
			if (i != std::string::npos)
			{
				tmp.erase(0, i + 1);
				srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>("EXTENSION", tmp));
			}
			else
			{
				i = fileName.rfind('.');
				j = fileName.rfind('/');
				if (i != std::string::npos && i > j)
					srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>("EXTENSION", fileName.substr(i)));
				else
					srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>("EXTENSION", "unknown"));
			}
		}
		else
		{
			i = fileName.rfind('.');
			j = fileName.rfind('/');
			if (i != std::string::npos && i > j)
				srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>("EXTENSION", fileName.substr(i)));
		}
		i = fileName.rfind('.');
		j = fileName.rfind('/');
		tmp = fileName.substr(j + 1, i - j);
		if (!tmp.empty())
			srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>("FILENAME", tmp));
		else
			srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>("FILENAME", "default"));
	}
	srv.getCGIptr()->PATH_TRANSLATED = std::string(servNode->root + "/CGIs/upload.py");
	srv.getCGIptr()->PATH_TO_SGI = std::string("/usr/local/bin/python3.9");
	srv.Stage = 4;
	srv.CGIStage = 0;
	HandlerRequest::CGIHandler(srv);
}

void HandlerRequest::DELETE(Server &srv, t_serv *servNode, t_loc *locNode, std::string &fileName)
{
	std::string fullPath;

	(void) locNode;
	if (access(fileName.c_str(), W_OK) != 0)
	{
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 3;
		srv.Stage = 9;
		HandlerRequest::prepareToSendError(srv);
		return;
	}

	srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>(std::string("FILENAME"), fileName));
	srv.getCGIptr()->PATH_INFO = std::string("");
	srv.getCGIptr()->PATH_TRANSLATED = std::string(servNode->root + "/CGIs/delete.sh");
	srv.getCGIptr()->PATH_TO_SGI = std::string("/bin/bash");
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
	std::string	fullPath(servNode->root + it->second);
	if (access((servNode->root + "/CGIs/download.py").c_str(), X_OK) == -1)
	{
		Logger::putMsg("download.py have no access to execute => error pages can't be open", FILE_ERR, ERR);
		return(false);
	}
	if (access(fullPath.c_str(), R_OK) == -1)
	{
		Logger::putMsg("can't find error " + fullPath + " or FILE have no access to read", FILE_ERR, ERR);
		servNode->errPages.erase(it);
		return(false);
	}
	std::string startStringForError("HTTP/1.1 ");
	switch (code)
	{
		case 400: { startStringForError += std::string("400 Bad Request\r\n"); break; }
		case 403: { startStringForError += std::string("403 Forbidden\r\n"); break; }
		case 404: { startStringForError += std::string("404 Not Found\r\n"); break; }
		case 405: { startStringForError += std::string("405 Method Not Allowed\r\n"); break; }
		case 500: { startStringForError += std::string("500 Internal Server Error\r\n"); break; }
		case 505: { startStringForError += std::string("505 HTTP Version Not Supported\r\n"); break; }
		default: { if (srv.Stage > 90) return (false); }
	}
	srv.getReq_struct()->base.headers.insert(std::pair<std::string, std::string>(std::string("RESPONSE_START_STRING"), startStringForError));
	HandlerRequest::GET(srv, servNode, fullPath, false);
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
	switch (srv.CGIStage)
	{
		case 0: { HandlerRequest::startCGI(srv); break; }
		case 1:
		case 2:
		case 20: { HandlerRequest::prepareToSendCGI(srv); break; }
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
		case 5:
		case 50: { break; } //read from PIPE
		case 6: //end of pipe - clean all
		{
			delete srv.getCGIptr();
			srv.setCGIptr(NULL);
			srv.CGIStage = 0;
			srv.Stage = 5;
			break;
		}
		case 9: { HandlerRequest::CGIerrorManager(srv); break; }
		default: { MainClass::setBadStageError(srv); }
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
	// if (access(srv.getCGIptr()->PATH_TRANSLATED.c_str(), X_OK) == -1)
	// {
	// 	srv.Stage = 9;
	// 	srv.getReq_struct()->answ_code[0] = 4;
	// 	srv.getReq_struct()->answ_code[1] = 3;
	// 	HandlerRequest::prepareToSendError(srv);
	// 	return;
	// }
	srv.CGIStage = srv.getCGIptr()->startCGI();
	srv.CGIStage = srv.getCGIptr()->ForkCGI(srv);
	if (srv.CGIStage == 9)
		HandlerRequest::CGIerrorManager(srv);
	else if (HandlerRequest::isBodyNeed(srv))
		HandlerRequest::prepareToSendCGI(srv);
	else
		srv.CGIStage = 4;
}

void	HandlerRequest::CGIerrorManager(Server &srv)
{
	srv.getCGIptr()->clearCGI();
	srv.Stage = 9;
	srv.CGIStage = 9;
	HandlerRequest::prepareToSendError(srv);
}

void HandlerRequest::prepareToSendCGI(Server &srv)
{
	if (srv.CGIStage == 20)
		return;
	if (srv.getReq_struct()->body.empty() && srv.parseStage < 3)
	{
		srv.Stage = 1;
		return;
	}
	srv.Stage = 4;
	if (srv.parseStage == 3 && srv.getReq_struct()->body.empty())
		srv.CGIStage = 4;
	else if (srv.parseStage == 3)
		srv.CGIStage = 2;
	else
		srv.CGIStage = 1;
}

void HandlerRequest::addFileNameEnv(HTTP_Request *req, char result)
{
	size_t i;
	size_t j;
	std::map<std::string, std::string>::iterator it;
	if (result == 3)
		return;
	//set filename
	if (result == 2 || result == 0)
	{
		it = req->base.headers.find("Content-Disposition");
		if (it == req->base.headers.end())
			req->base.headers.insert(std::pair<std::string, std::string>("FILENAME", "default"));
		else
		{
			i = it->second.find("name=\"") + 6;
			j = it->second.find("\"", i);
			if (i == std::string::npos || j == std::string::npos || j <= i)
				req->base.headers.insert(std::pair<std::string, std::string>("FILENAME", "default"));
			else
			{
				std::string fileName = it->second.substr(i, j - i);
				i = it->second.find(fileName) + fileName.length() + 2;
				j = it->second.find("\"", i);
				if (i == std::string::npos || j == std::string::npos || j <= i)
					req->base.headers.insert(std::pair<std::string, std::string>("FILENAME", "default"));
				fileName = it->second.substr(i, j - i);
				req->base.headers.insert(std::pair<std::string, std::string>("FILENAME", fileName));
			}
		}
	}
	//set ext
	if (result == 1 || result == 0)
	{
		it = req->base.headers.find("Content-Type");
		if (it == req->base.headers.end())
		{
			req->base.headers.insert(std::pair<std::string, std::string>("EXTENSION", "unknown"));
			return;
		}
		i = it->second.find("/");
		if (i == std::string::npos)
		{
			req->base.headers.insert(std::pair<std::string, std::string>("EXTENSION", "unknown"));
			return;
		}
		std::string ext = it->second.substr(i + 1);
		if (ext.empty())
			req->base.headers.insert(std::pair<std::string, std::string>("EXTENSION", "unknown"));
		else
			req->base.headers.insert(std::pair<std::string, std::string>("EXTENSION", std::string(".") + ext));
	}
}

char HandlerRequest::changeBodyIfBoundryIsSet(HTTP_Request *req)
{
	std::map<std::string, std::string>::iterator it = req->base.headers.find("Content-Type");
	if (it == req->base.headers.end())
		return (0);
	size_t i = it->second.find("multipart/form-data;");
	if (i == std::string::npos)
		return (0);
	i = it->second.find("boundary=", i + 19);
	if (i == std::string::npos)
		return (0);
	std::string boundary = it->second.substr(i + 9);
	for (i = 0; i < boundary.length() && boundary[i] == '-'; i++)
		;
	boundary.erase(0, i);
	i = req->body.find(boundary);
	size_t j = req->body.rfind(boundary);
	if (i == std::string::npos || j == i)
		return (0);
	i += boundary.length() + 2; // +\r\n
	for (--j; j > i && req->body[j] == '-'; j--);
	if (req->body[j] == '\n')
		j--;
	if (req->body[j] == '\r')
		j--;
	std::string newBody = req->body.substr(i, j - i);
	i = newBody.find("\r\n\r\n");
	if (i == std::string::npos)
	{
		req->body = newBody;
		return (0);
	}
	std::string subHeads = newBody.substr(0, i);
	newBody.erase(0, i + 4);
	req->body = newBody;
	i = subHeads.find("name=\"");
	j = subHeads.find("\"", i + 6);
	if (i == std::string::npos || j == std::string::npos)
		return (0);
	std::string filename = subHeads.substr(i + 6, j - (i + 6));
	i = subHeads.find(filename, j);
	if (i == std::string::npos)
		return (0);
	i = subHeads.find("\"", i + 1);
	j = subHeads.find("\"", i + 1);
	if (i == std::string::npos || j == std::string::npos)
		return (0);
	filename = subHeads.substr(i + 1, j - (i + 1));
	i = filename.rfind(".");
	std::string ext;
	if (i != std::string::npos)
	{
		ext = filename.substr(i);
		filename.erase(i);
	}
	else
		ext = std::string(".unknown");
	if (!filename.empty() && !ext.empty())
	{
		req->base.headers.insert(std::pair<std::string, std::string>(std::string("FILENAME"), filename));
		req->base.headers.insert(std::pair<std::string, std::string>(std::string("EXTENSION"), ext));
		return (3);
	}
	if (!ext.empty())
	{
		req->base.headers.insert(std::pair<std::string, std::string>(std::string("EXTENSION"), ext));
		return (2);
	}
	req->base.headers.insert(std::pair<std::string, std::string>(std::string("FILENAME"), filename));
	return (1);
}

void HandlerRequest::checkReadyToHandle(Server &srv)
{
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
	if (srv.Stage == 99) //runtime error/FATAL
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
	if (HandlerRequest::haveErrorPage(srv, servNode, code))
		return;
	tmp = HTTP_Answer::ft_reqtoansw(*(srv.getReq_struct()));
	srv.setResponse(HTTP_Answer::ft_answtostr(tmp), true);
	if (srv.Stage > 90)
		srv.writeStage = 3;
	else
		srv.writeStage = 2;
	srv.Stage = 5;
	srv.isChunkedResponse = false;
	srv.getRequest().clear();
}

HandlerRequest::HandlerRequest() {}
HandlerRequest::~HandlerRequest() {}