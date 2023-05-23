#include "HTTP_Request.hpp"

// void ft_tokenisation(std::string raw) {

// 	int end = raw.size() - 1;
// 	std::string *strs;
// 	int str_nmbr = 0;
// 	int	letter;

// 	for (int i = 0; raw[i] != end; ++i) {
// 		letter = 0;
// 		while (raw[i + letter] != '\n' && raw[i + letter - 1] != '\r')
// 			letter++;
// 		strs[str_nmbr].append(raw.substr(i, letter - 1));
// 		i += letter;
// 	}
// }

void HTTP_Request::ft_strtoreq(Server &srv, HTTP_Request *req) {

// URL

	int i = 0;
	std::string raw = srv.getReq();
	int end = raw.size() - 1;

//	ft_tokenisation()
	// Method
	for (; raw[i] != ' ' && i != end && i < 15; ++i)
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
	if ((raw.size() - i) > srv.serv->limitCLientBodySize) {
		req->answ_code[0] = 4;
		req->answ_code[0] = 14;
		return ;
	}
	req->answ_code[0] = ft_strtohdrs(raw, i, end, &(req->headers), &(req->body));
	req->answ_code[1] = req->answ_code[0] % 100;
	req->answ_code[0] = req->answ_code[0] / 100;
}
