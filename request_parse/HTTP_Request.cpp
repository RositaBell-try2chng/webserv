#include "HTTP_Request.hpp"

std::vector<std::string> ft_tokenisation(std::string raw) {

	int end = raw.size() - 1;
	std::vector<std::string> strs;

	int	letter;

	for (int i = 0; raw[i] != end; ++i) {
		letter = 0;
		while (raw[i + letter] != '\n' && raw[i + letter - 1] != '\r')
			letter++;
		strs.push_back(raw.substr(i, letter - 1));
		i += letter;
	}
	return strs;
}

bool ft_if_method_implemented(std::string method) {

	// put here implemented methods:
	return (method.compare("GET")
			&& method.compare("POST")
			&& method.compare("DELETE"));
}

int ft_set_method(HTTP_Request *req, std::string url, int end) {

	int	i = 0;

	// set method
	for (; url[i] != ' ' && i < 7 && i < end; ++i)
		req->method.push_back(url[i]);

	std::string method = req->method;

	// checking if method implemented
	if (ft_if_method_implemented(method)) {
		Logger::putMsg("Method is not implemented", FILE_WREQ, WREQ);
		req->answ_code[0] = 5;
		req->answ_code[0] = 1;
		return 0;
	}

	// other checks if we need (use "method", return 0 if false)

	return i;
}

int	ft_set_uri(HTTP_Request *req, std::string url, int end, int i) {

	for (++i; url[i] != ' ' && i != end ; ++i)
		req->uri.push_back(url[i]);

	if (req->uri.size() == 0) {
		Logger::putMsg("Request hasn\'t URI", FILE_WREQ, WREQ);
		req->answ_code[0] = 4;
		req->answ_code[0] = 0;
		return 0;
	}

	return (i);
}

int	ft_set_version(HTTP_Request *req, std::string url, int end, int i) {

	for (++i; i != end; ++i)
		req->version.push_back(url[i]);

	if (req->version.size() == 0) {
		Logger::putMsg("Request hasn\'t HTTP version", FILE_WREQ, WREQ);
		req->answ_code[0] = 4;
		req->answ_code[0] = 0;
		return 0;
	}

	if (req->version.compare("HTTP/1.1")) {
		Logger::putMsg("Request has wrong HTTP version", FILE_WREQ, WREQ);
		req->answ_code[0] = 5;
		req->answ_code[0] = 5;
		return 0;
	}

	return i;
}

int	ft_set_url(HTTP_Request *req, std::string url) {

	int	end = url.size() - 1;

	int i = ft_set_method(req, url, end);

	if (i > 0)
		i = ft_set_uri(req, url, end, i);

	if (i > 0)
		i = ft_set_version(req, url, end, i);

	return (i);
}

int	ft_make_hdr(HTTP_Request *req, std::string raw) {

	int len = raw.size();

	if (len > 4096) {
		Logger::putMsg("Request header is too large", FILE_WREQ, WREQ);
		req->answ_code[0] = 4;
		req->answ_code[0] = 31;
		return 0;
	}

	int	end = len - 1;

	std::string	key;
	std::string	value;

	int	i = 0;

// Key
	for (; i != end && raw[i] != ':'; ++i) {
		key.push_back(raw[i]);
	}

	if (i == end) {
		Logger::putMsg("Header " + key + "Doesn't have a value", FILE_WREQ, WREQ);
		req->answ_code[0] = 4;
		req->answ_code[0] = 0;
		return 0;
	}

	if (req->headers.find(key) != req->headers.end()){
		Logger::putMsg("Multiple request's header: " + key, FILE_WREQ, WREQ);
		req->answ_code[0] = 4;
		req->answ_code[0] = 0;
		return 0;
	}

// Value
	for (++i; raw[i] == ' '; ++i) {}

	for (; i != end && raw[i] != '\n'; ++i)
		if (raw[i] != '\r')
			value.push_back(raw[i]);

	std::pair<std::string, std::string> hdr = std::make_pair(key, value);
	key.clear();
	value.clear();

	req->headers.insert(hdr);

	return len;
}

bool ft_set_hdrs(HTTP_Request *req, std::vector<std::string> req_str_arr, int end) {

	int i = 1;
	int total_len = 0;
	int curr_len;

// Headers
	for (; req_str_arr[i].compare("\r") && i != end; ++i) {
		curr_len = ft_make_hdr(req, req_str_arr[i]);
		if (!curr_len)
			return ;
		total_len += curr_len;
		if (total_len > 8096) {
			Logger::putMsg("Request headers are too large", FILE_WREQ, WREQ);
			req->answ_code[0] = 4;
			req->answ_code[0] = 31;
			return 0;
		}
	}

	if (req->headers.find("Host") == req->headers.end()) {
		Logger::putMsg("Request hasn't \"Host\" header", FILE_WREQ, WREQ);
		req->answ_code[0] = 4;
		req->answ_code[0] = 0;
		return 0;
	}

	return i;
}

void ft_set_body(HTTP_Request *req, std::vector<std::string> req_str_arr, int i, int end, int limit) {

	int total_size = 0;

	for (; req_str_arr[i] != req_str_arr[end]; ++i) {
		total_size += req_str_arr[i].size();

		if (total_size > limit) {
			Logger::putMsg("Request\'s body is too large", FILE_WREQ, WREQ);
			req->answ_code[0] = 4;
			req->answ_code[0] = 14;
			return ;
		}

		req->body.append(req_str_arr[i]);
	}

	// work with body
}

void HTTP_Request::ft_strtoreq(Server &srv, HTTP_Request *req) {

	std::string	raw = srv.getReq();

	if (raw.size() > 100000) {
		Logger::putMsg("Request is too large", FILE_WREQ, WREQ);
		req->answ_code[0] = 4;
		req->answ_code[0] = 13;
		return ;
	}

	std::vector<std::string> req_str_arr;

// Request to sthrings vector
	req_str_arr = ft_tokenisation(raw);

// URL (Method, URI, HTTP-version)
	if (!ft_set_url(req, req_str_arr[0]))
		return ;

// Headers
	int	end = req_str_arr.size() - 1;
	int i = ft_set_hdrs(req, req_str_arr, end);
	if (!i)
		return ;
		
// Body
	ft_set_body(req, req_str_arr, i, end, srv.serv->limitCLientBodySize);
}
