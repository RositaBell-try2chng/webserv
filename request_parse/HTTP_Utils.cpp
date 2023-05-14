#include "HTTP_Utils.hpp"
#include "../Logger.hpp"

int ft_strtohdrs(std::string raw, int i, int end,
		std::map<std::string, std::string> *hdrs,
		std::string *body) {

	std::string		key;
	std::string		value;

// Headers
	for (++i; raw[i] != ('\n') && i != end; ) {
	// Key
		for (; i != end && raw[i] != ':'; ++i) {
			key.push_back(raw[i]);
		}
		if (i == end) {
			Logger::putMsg("Header " + key + "Doesn't have a value", FILE_WREQ, WREQ);
			return 400;
		}

	// Value
		for (++i; raw[i] == ' '; ++i) {}
	
		for (; i != end && raw[i] != '\n'; ++i)
			if (raw[i] != '\r')
				value.push_back(raw[i]);

		std::pair<std::string, std::string> hdr = std::make_pair(key, value);
		key.clear();
		value.clear();

		if (hdrs->find(hdr.first) != hdrs->end()){
			Logger::putMsg("Multiple request's header: " + hdr.first, FILE_WREQ, WREQ);
			return (400);
		}
		hdrs->insert(hdr);

		if (i != end) {
			++i;
			if (raw[i] == '\r')
				++i;
		}
	}

	if (hdrs->find("Host") == hdrs->end()) {
		Logger::putMsg("Request hasn't \"Host\" header", FILE_WREQ, WREQ);
		return 400;
	}

// Body
	if (raw.size() > 0) {
		raw.erase(0, ++i);
		*body = raw;
	}

	return 200;
}

std::string ft_hdrstostr(std::map<std::string, std::string> headers) {
	
	std::string	concat_hdrs;
	std::string	hdr;
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++) {
        hdr = (*it).first + ": " + (*it).second + "\r\n";
		concat_hdrs.append(hdr);
		hdr.clear();
    }
	return (concat_hdrs);
}
