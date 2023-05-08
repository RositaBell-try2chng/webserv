#include "HTTP_Request.hpp"
#include "../Logger.hpp"
#include <iostream>

HTTP_Request::HTTP_Request() {};
HTTP_Request::~HTTP_Request() {};

int HTTP_Request::ft_strtoreq(std::string raw, HTTP_Request req) {

	std::string		key;
	std::string		value;

// URL

	int i = 0;
	// Method
	for (; raw[i] != ' '; ++i)
		req.method.push_back(raw[i]);

	raw.erase(0, i);
	i = 0;
	if (req.method.size() == 0) {
		Logger::putMsg("Request hasn\'t method", FILE_WREQ, WREQ);
		return 1;
	}
	
	// URI
	for (; raw[i] != ' '; ++i)
		req.uri.push_back(raw[i]);
	
	raw.erase(0, i);
	i = 0;
	if (req.uri.size() == 0) {
		Logger::putMsg("Request hasn\'t URI" + raw, FILE_WREQ, WREQ);
		return 1;
	}

	// Version
	if (raw.substr(0, 5).compare("HTML/")) {
		Logger::putMsg("Request's HTML version incorrect", FILE_WREQ, WREQ);
		return 1;
	}
	else
		raw.erase(0, 5);
	
	for (int i = 0; raw[i] != '\n' ; ++i)
		if (raw[i] != '\r')
			req.version.push_back(raw[i]);
	raw.erase(0, i);

	if (req.version.size() == 0) {
		Logger::putMsg("Request hasn\'t HTML version", FILE_WREQ, WREQ);
		return 1;
	}

// Headers
	while (raw[0]!=('\n') && raw.size() > 0) {
		i = 0;

		while (raw[i] != ':')
			key.push_back(raw[i]);
		++i;

		while (raw[i] != '\n')
			if (raw[i] != '\r')
				value.push_back(raw[i]);

		
		req.headers.insert(std::make_pair(key, value));
		key.clear();
		value.clear();
		if (raw[0] == '\r')
			++i;
		raw.erase(0, i);
	}

	if (req.headers.find("Host") == req.headers.end()) {
		Logger::putMsg("Request hasn't \"Host\" header", FILE_WREQ, WREQ);
		return 1;
	}

// Body
	if (raw.size() > 0)
		req.body = raw;

	return (0);
}
