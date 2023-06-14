#include "HandlerRequest.hpp"

void HandlerRequest::mainHandler(Server &srv)
{
	switch (src.Stage)
	{
		case 0: //start
		case 1: //need read from socket
		case 2: //need parse request
		case 3: //need handle HTTP_reqStruct
		case 4: { HandlerRequest::CGIHandler(srv); break; } //need CGI
		case 5: //need write to socket
		case 9: //error
	}
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

HandlerRequest::HandlerRequest() {}
HandlerRequest::~HandlerRequest() {}


void HandlerRequest::CGIHandler(Server &srv)
{
    if (!(it->second->CGIStage == 0 || it->second->CGIStage == 6) && it->second->getCGIptr() == NULL)
    {
        std::cout << "BAD stages or ptr in CGI Handler on fd: " << it->first << std::endl;
        return;
    }
    switch (it->second->CGIStage)
    {
        

        



        case 6:  { it->second->CGIStage = it->second->getCGIptr()->endOfCGI(it); break; }//end of pipe riched + clean all
        case 9: { it->second->Stage = 9; MainClass::errorManager(it) break; } //CGI failed + clean
        default:
        {
			if ( )
            std::cout << "bad stage of CGI: " << it->second->CGIStage << std::endl;
        }
    }
}