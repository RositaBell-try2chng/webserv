#include "HTTP_Utils.hpp"
#include "../Logger.hpp"

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
