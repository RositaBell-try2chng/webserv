#include "Request_parse.hpp"
#include "Logger.hpp"

Request_parse::HTTP_Request() {};
Request_parse::~HTTP_Request() {};

HTTP_Request Request_parse::strtoreq(std::string raw) {

	HTTP_Request req(); 

	for (int i == 0; raw[i] != ' '; ++i)
		req->method.push_back(raw[i]);
	raw.erase(0, i);
	
	for (int i == 0; raw[i] != ' '; ++i)
		req->uri.push_back(raw[i]);
	raw.erase(0, i);

	if (raw.substr(0, 5).compare("HTML/"))
		raw.erase(0, 5);
	else
		
	
	for (int i == 0; raw[i] != '\n'; ++i)
		if (raw[i] != '\r')
			req->uri.push_back(raw[i]);
	raw.erase(0, i);


};
