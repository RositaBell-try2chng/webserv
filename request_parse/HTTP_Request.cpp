#include "HTTP_Request.hpp"
#include "../Logger.hpp"
#include <iostream>

HTTP_Request::HTTP_Request() {};
HTTP_Request::~HTTP_Request() {};

int ft_strtohdrs(std::string raw, int i, int end,
		std::map<std::string, std::string> *hdrs,
		std::string *body) {

	std::string		key;
	std::string		value;

	for (++i; raw[i] != ('\n') && i != end; ) {

		for (; i != end && raw[i] != ':'; ++i) {
			key.push_back(raw[i]);
		}
		if (i == end) {
			Logger::putMsg("Header " + key + "Doesn't have a value", FILE_WREQ, WREQ);
			return 1;
		}
	
		
		for (++i; i != end && raw[i] != '\n'; ++i)
			if (raw[i] != '\r')
				value.push_back(raw[i]);

		
		hdrs->insert(std::make_pair(key, value));
		key.clear();
		value.clear();
		if (i != end) {
			if (raw[i + 1] == '\r')
				i += 2;
			else ++i;
		}
	}

	if (hdrs->find("Host") == hdrs->end()) {
		Logger::putMsg("Request hasn't \"Host\" header", FILE_WREQ, WREQ);
		return 1;
	}

// Body
	if (raw.size() > 0)
		raw.erase(0, ++i);
		*body = raw;

	return (0);
}

int HTTP_Request::ft_strtoreq(std::string raw, HTTP_Request *req) {

// URL

	int i = 0;
	int end = raw.size() - 1;

	// Method
	for (; raw[i] != ' ' && i != end; ++i)
		req->method.push_back(raw[i]);

	if (req->method.size() == 0) {
		Logger::putMsg("Request hasn\'t method", FILE_WREQ, WREQ);
		return 1;
	}
	
	// URI
	for (++i; raw[i] != ' ' && i != end; ++i) {
		req->uri.push_back(raw[i]);
	}
	
	if (req->uri.size() == 0) {
		Logger::putMsg("Request hasn\'t URI" + raw, FILE_WREQ, WREQ);
		return 1;
	}
		
	for (++i; raw[i] != '\n'  && i != end; ++i) {
		if (raw[i] != '\r')
			req->version.push_back(raw[i]);
	}

	if (req->version.size() == 0) {
		Logger::putMsg("Request hasn\'t HTML version", FILE_WREQ, WREQ);
		return 1;
	}

// Headers and Body

	return ft_strtohdrs(raw, i, end, &(req->headers), &(req->body));
}
