#include "HTTP_Request.hpp"

void HTTP_Request::ft_strtoreq(std::string raw, HTTP_Request *req) {

// URL

	int i = 0;
	int end = raw.size() - 1;

	
	// Method
	for (; raw[i] != ' ' && i != end; ++i)
		req->method.push_back(raw[i]);

	if (req->method.size() == 0) {
		Logger::putMsg("Request hasn\'t method", FILE_WREQ, WREQ);
		req->answ_code[0] = 4;
		req->answ_code[0] = 0;
		return ;
	}
	
	// URI
	for (++i; raw[i] != ' ' && i != end ; ++i) {
		req->uri.push_back(raw[i]);
	}
	
	if (req->uri.size() == 0) {
		Logger::putMsg("Request hasn\'t URI" + raw, FILE_WREQ, WREQ);
		req->answ_code[0] = 4;
		req->answ_code[0] = 0;
		return ;
	}
	
	// Version
	for (++i; raw[i] != '\n'  && i != end; ++i) {
		if (raw[i] != '\r')
			req->version.push_back(raw[i]);
	}

	if (req->version.size() == 0) {
		Logger::putMsg("Request hasn\'t HTML version", FILE_WREQ, WREQ);
		req->answ_code[0] = 4;
		req->answ_code[0] = 0;
		return ;
	}

// Headers and Body				HTTP_Utils.hpp
	req->answ_code[0] = ft_strtohdrs(raw, i, end, &(req->headers), &(req->body));
	req->answ_code[1] = req->answ_code[0] % 100;
	req->answ_code[0] = req->answ_code[0] / 100;
}
