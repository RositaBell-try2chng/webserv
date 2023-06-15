#include "HandlerRequest.hpp"

void HandlerRequest::mainHandler(Server &srv)
{
	switch (src.Stage)
	{
		case 0: { HandlerRequest::start(srv) break; } //start
		//case 1: //read from socket //nothing to do //server ready to read
		case 2: { HandlerRequest::parserRequest(srv); break; } //need parse request
		case 3: { HandlerRequest::handleRequest(srv); break; } //need handle HTTP_reqStruct
		case 4: { HandlerRequest::CGIHandler(srv); break; } //need CGI
		//case 5: //write to socket //nothing to do //server ready to write
		case 9: { HandlerRequest::prepareToSendError(srv); break; } //error
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
		case 2: { HandlerRequest::checkHeaders(srv); break; }
		case 3: { srv.Stage = 3; HandlerRequest::handleRequest(srv); break; }
		case 9: { srv.Stage = 9; HandlerRequest::prepareToSendError(srv); break; }
		default: { std::cout << "BAD parse Stage: " << srv.parseStage << std::endl; break; }
	}
}

void HandlerRequest::handleRequest(Server &srv)
{
	t_serv		*servNode = srv.findServer(srv.getReq_struct()->host);

	if (srv.getReq_struct()->flg_te == 0 && srv.getReq_struct()->content_lngth > servNode->limitCLientBodySize)
		;//fix me: add error limiting

	size_t		i = srv.getReq_struct()->uri.rfind('/');
	std::string	locName = srv.getReq_struct()->uri.substr(0, i + 1);
	t_loc		*locNode = srv.findLocation(locName, servNode);

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
				HandlerRequest::CGIerrorManager(srv);//fix me: implement
			break;
		}
		case 4: //wait end of pid
		{
			srv.CGIStage = srv.getCGIptr()->waitingCGI(it);
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
	}
}

void HandlerRequest::prepareToSendCGI(Server &srv)
{
	if (!srv.getReq_struct().body.empty())
		return;
	if (srv.parseStage == 3)
	{
		srv.getReq_struct().body = std::string("\r\n\r\n");
		srv.CGIStage = 2;
	}
	srv.Stage = 2;
	//fix me: add send to Parse;
	if (srv.parseStage == 3)
		srv.CGIStage = 2;
}

void HandlerRequest::startCGI(Server &srv)
{
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
		MainClass::CGIerrorManager(srv);
	else
		HandlerRequest::prepareToSendCGI(srv);
}

void HandlerRequest::checkHeaders(Server &srv)
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

HandlerRequest::HandlerRequest() {}
HandlerRequest::~HandlerRequest() {}