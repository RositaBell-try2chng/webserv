#include "Request_parse.hpp"
#include "../Logger.hpp"

Request_parse::HTTP_Request() {};
Request_parse::~HTTP_Request() {};

Request_parse::HTTP_Request Request_parse::ft_strtorec(raw, req) {

	HTTP_Request	req;
	std::string		key;
	std::string		value;

// Start

	// Method
	for (int i = 0; raw[i] != ' '; ++i)
		req->method.push_back(raw[i]);
	raw.erase(0, i);
	if (req->method.size == 0) {
		Logger::putMsg("Request haven\'t method", FILE_WREQ, WREQ);
		return NULL;
	}
	
	// URI
	for (int i = 0; raw[i] != ' '; ++i)
		req->uri.push_back(raw[i]);
	raw.erase(0, i)
	if (req->uri.size == 0) {
		Logger::putMsg("Request haven\'t URI" + raw, FILE_WREQ, WREQ);
		return NULL;
	}

	// Version
	if (raw.substr(0, 5).compare("HTML/")) {
		Logger::putMsg("Request's HTML version incorrect", FILE_WREQ, WREQ);
		return NULL;
	}
	else
		raw.erase(0, 5);
	
	for (int i = 0; raw[i] != '\n' ; ++i)
		if (raw[i] != '\r')
			req->version.push_back(raw[i]);
	raw.erase(0, i);

	if (req->version.size == 0) {
		Logger::putMsg("Request haven\'t HTML version", FILE_WREQ, WREQ);
		return NULL;
	}

// Headers
	while (!raw.substr(0, 2).compare("\r\n") && raw[0]!=("\n") && raw.size() > 0) {
		i = 0;

		while (raw[i] != ':')
			key.push_back(raw[i]);
		++i;

		while (raw[i] != '\n')
			if (raw[i] != '\r')
				value.push_back(raw[i])

		map.insert(key, value);
		key.clean;
		value.clean;
		raw.erase(0, i);
	}

	if (headers.find("Host") == headers.end()) {
		Logger::putMsg("Request haven\'t \"Host\" header", FILE_WREQ, WREQ);
		return NULL;
	}


// Body
	if (raw.size > 0)
		body = raw;

	return (req);
}
