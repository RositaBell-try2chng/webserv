#include "HTTP_Request.hpp"

bool ft_if_method_implemented(std::string method) {

	// put here implemented methods:
	return (method.compare("GET")
			&& method.compare("POST")
			&& method.compare("DELETE"));
}

int ft_set_method(HTTP_Request &req, std::string url, int end) {

	int	i = 0;

	// set method
	for (; url[i] != ' ' && i < 7 && i < end; ++i)
		req.base.start_string.method.push_back(url[i]);

	std::string method = req.base.start_string.method;

	// checking if method implemented
	if (ft_if_method_implemented(method)) {
		Logger::putMsg("Method is not implemented", FILE_WREQ, WREQ);
		req.answ_code[0] = 5;
		req.answ_code[1] = 1;

		return 0;
	}

	// other checks if we need (use "method", return 0 if false)

	return i;
}

int	ft_set_uri(HTTP_Request &req, std::string url, int end, int i) {

	for (++i; url[i] != ' ' && i != end ; ++i)
		req.base.start_string.uri.push_back(url[i]);

	end = req.base.start_string.uri.size();

	if (end == 0) {
		Logger::putMsg("Request hasn\'t URI", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 0;
		return 0;
	}

	int	j = 0;

	for (++j; req.base.start_string.uri[j] != '?' && j < end ; ++j){}
	for (++j; j < end; ++j)
		req.base.start_string.prmtrs.push_back(req.base.start_string.uri[j]);

	return (i);
}

int	ft_set_version(HTTP_Request &req, std::string url, int end, int i) {

	for (++i; i != end; ++i)
		req.base.start_string.version.push_back(url[i]);

	if (req.base.start_string.version.size() == 0) {
		Logger::putMsg("Request hasn\'t HTTP version", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 0;
		return 0;
	}

	if (req.base.start_string.version.compare("HTTP/1.1")) {
		Logger::putMsg("Request has wrong HTTP version", FILE_WREQ, WREQ);
		req.answ_code[0] = 5;
		req.answ_code[1] = 5;
		return 0;
	}

	return i;
}

int	ft_set_url(HTTP_Request &req) {

	int	end = req.left.size();

	if (end > REQ_MAX_SIZE) {
		Logger::putMsg("Request is too large", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 13;
		return (0);
	}

	--end;

	int i = ft_set_method(req, req.left, end);

	if (i > 0)
		i = ft_set_uri(req, req.left, end, i);

	if (i > 0)
		i = ft_set_version(req, req.left, end, i);

	return (i);
}

bool ft_set_hdr(HTTP_Request &req) {

	int curr_len = req.left.size();

	if (curr_len > HDR_MAX_LEN) {
		Logger::putMsg("Request header is too large", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 31;
		return false;
	}

	req.base.hdrs_total_len += curr_len;

	if (req.base.hdrs_total_len > HDRS_MAX_SUM_LEN) {
		Logger::putMsg("Request headers are too large", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 31;
		return false;
	}

	if (req.base.hdrs_total_len + req.base.start_string.len > REQ_MAX_SIZE) {
		Logger::putMsg("Request is too large", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 13;
		return (0);
	}

	std::pair<std::string, std::string>	header = ft_strtopair(req.left, ':');
	req.left.clear();

	if (req.base.headers.find(header.first) != req.base.headers.end()){
		Logger::putMsg("Multiple request's header: " + header.first, FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 0;
		return false;
	}
	else if (!header.first.compare("")) {
		Logger::putMsg("Header " + header.first + "Doesn't have a value", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 0;
		return 0;
	}
	else
		req.base.headers.insert(header);

	return true;

	
}

void ft_set_body(HTTP_Request *req, std::vector<std::string> req_str_arr, int i, int end, int limit) {

	int total_size = 0;

	for (; req_str_arr[i] != req_str_arr[end]; ++i) {
		total_size += req_str_arr[i].size();

		if (total_size > limit) {
			Logger::putMsg("Request\'s body is too large", FILE_WREQ, WREQ);
			req->answ_code[0] = 4;
			req->answ_code[1] = 14;
			return ;
		}

		req->body.append(req_str_arr[i]);
	}

	// work with body
}

void	ft_parse_body(HTTP_Request &req, std::string &raw, int &end) {

	if (req.base.hdrs_total_len + req.base.start_string.len + req.content_lngth > REQ_MAX_SIZE) {
		Logger::putMsg("Request is too large", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 13;
		return ;
	}

	if (end == 0 || req.stage != 52 || req.answ_code[0] > 3)
		return ;

	int i;

	for (i = 0; i < end && i < req.content_lngth; ++i) {}

	std::string	content = raw.substr(0, i);

	if (i < req.content_lngth)
		req.left = content;
	else {
		req.body = req.left + content;
		++req.stage;
		req.left.clear();
	}

	raw.erase(0, i);
}

void	ft_parse_headers(HTTP_Request &req, std::string &raw, int &end) {

	if (end == 0 || req.stage != 51 || req.answ_code[0] > 3)
		return;

	int	i;
	int	letter;

	for (i = 0; i < end; ++i) {
		for (letter = 1;
				i + letter < end && raw[i + letter] != '\n' && raw[i + letter - 1] != '\r';
				++letter) {}
		req.left += raw.substr(i, letter - 1);
		if (raw[i + letter] != '\n' && raw[i + letter - 1] != '\r') {
			ft_set_hdr(req);
			i += (letter + 1);
		}
		else
			break ;
		if (i + 1 < end && raw[i + 1] == '\n' && raw[i] == '\r') {
			ft_headers_parse(req);
			++req.stage;
			i += 2;
			break ;
		}
	}
	raw.erase(0, i);
	end = raw.size();
}

void	ft_parse_start_string(HTTP_Request &req, std::string &raw, int &end) {

	if (end == 0 || req.stage != 50 || req.answ_code[0] > 3)
		return ;

	int i;

	for (i = 0; (i < end) && (raw[i] != '\n' && raw[i - 1] != '\r'); ++i)
		req.left.push_back(raw[i]);
struct HTTP_Request;
	if (raw[i] == '\n' && raw[i - 1] == '\r') {
		if (ft_set_url(req) != 0)
			++req.stage;
		req.left.clear();
	}

	raw.erase(0, i);
	end = raw.size();
}

void HTTP_Request::ft_strtoreq(HTTP_Request &req, std::string &raw) {

	int	end = raw.size();

	switch (req.stage) {
		case New: 			{ ft_parse_start_string(req, raw, end); __attribute__ ((fallthrough));}
		case Start_String:	{ ft_parse_headers(req, raw, end); __attribute__ ((fallthrough));}
		case Headers: 		{ ft_parse_body(req, raw, end); __attribute__ ((fallthrough));}
		case Ready: 		{ if (req.answ_code[0] < 4) break; __attribute__ ((fallthrough));}
		case Error: 		{ req.stage = 59; break; }
		default:			{ 

			Logger::putMsg("Undefined stage 5x!!!", FILE_WREQ, WREQ);
			break; 
		}
	}
}
