#include "HTTP_Request.hpp"

//	START STRING	======================================================================================

bool ft_if_method_implemented(std::string method) {

	// put here implemented methods:
	return (method.compare("GET")
			&& method.compare("POST")
			&& method.compare("DELETE")
			&& method.compare("PUT"));
}

int ft_set_method(HTTP_Request &req, std::string url, int end) {

	int	i = 0;

	// set method
	for (; url[i] != ' ' && i < 7 && i < end; ++i)
		req.base.start_string.method.push_back(url[i]);

	std::string method = req.base.start_string.method;

	// checking if method implemented
	if (i < end && ft_if_method_implemented(method)) {
		Logger::putMsg("Method is not implemented", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 5;

		//return 0;
	}

	// other checks if we need (use "method", return 0 if false)

	return i;
}

int	ft_set_uri(HTTP_Request &req, std::string url, int end, int i) {
	
	for (++i; url[i] != ' ' && i < end; ++i)
		req.base.start_string.uri.push_back(url[i]);
	
	end = req.base.start_string.uri.size();

	if (end == 0) {
		Logger::putMsg("Request hasn\'t URI", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 0;
		return 0;
	}

	int	j = 0;
	int	newSizeUri;

	for (++j; req.base.start_string.uri[j] != '?' && j < end ; ++j){}
	newSizeUri = j;
	for (++j; j < end; ++j)
		req.base.start_string.prmtrs.push_back(req.base.start_string.uri[j]);
	
	req.base.start_string.uri.resize(newSizeUri);
	return (i);
}

int	ft_set_version(HTTP_Request &req, std::string url, int end, int i) {

	for (++i; i < end; ++i)
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
		return 0;
	}

	int i = ft_set_method(req, req.left, end);

	if (i > 0)
		i = ft_set_uri(req, req.left, end, i);

	if (i > 0)
		i = ft_set_version(req, req.left, end, i);

	return (i);
}

void	ft_parse_start_string(HTTP_Request &req, std::string &raw, int &end) {

	if (end == 0 || req.stage != 50)
		return ;

	int i;

	for (i = 0; (i + 1 < end) && !(raw[i + 1] == '\n' && raw[i] == '\r'); ++i)
		req.left.push_back(raw[i]);

	if (raw[i + 1] == '\n' && raw[i] == '\r') {
		ft_set_url(req);
			++req.stage;
		req.left.clear();
	}
	else
		req.left.push_back(raw[i]);
	raw.erase(0, i + 2);
	end = raw.size();
}

//	HEADERS	======================================================================================

void ft_set_hdr(HTTP_Request &req) {

	int curr_len = req.left.size();
	req.base.hdrs_total_len += curr_len;

	if (curr_len > HDR_MAX_LEN) {
		Logger::putMsg("Request header is too large", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 31;
	}
	else if (req.base.hdrs_total_len > HDRS_MAX_SUM_LEN) {
		Logger::putMsg("Request headers are too large", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 31;
	}
	else if (req.base.hdrs_total_len + req.base.start_string.len > REQ_MAX_SIZE) {
		Logger::putMsg("Request is too large", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 13;
	}

	std::pair<std::string, std::string>	header = ft_strtopair(req.left, ':');
	req.left.clear();
	delSpaces(header.second);

	if (!header.second.compare("")) {
		Logger::putMsg("Header " + header.first + " Doesn't have a value", FILE_WREQ, WREQ);
		//req.answ_code[0] = 4;
		//req.answ_code[1] = 0;
		return ;
	}
	if (req.base.headers.find(header.first) != req.base.headers.end()){
		Logger::putMsg("Multiple request's header: " + header.first, FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 0;
		return ;
	}

	req.base.headers.insert(header);

	return ;
}

void	ft_parse_headers(HTTP_Request &req, std::string &raw, int &end) {

	if (end == 0 || req.stage != 51)
		return;

	int	i;
	int	letter;

	for (i = 0; i < end; ++i) {
		for (letter = 1;
				i + letter < end && !(raw[i + letter] == '\n' && raw[i + letter - 1] == '\r');
				++letter) {}
		req.left += raw.substr(i, letter);
		if (raw[i + letter] == '\n' && raw[i + letter - 1] == '\r' && !req.base.check) {
			ft_set_hdr(req);
			i += (letter - 1);
		}
		else {
			req.left.clear();
			break ;
		}
		++i;
		if ((i + 2 < end && raw[i + 2] == '\n' && raw[i + 1] == '\r')) {
			ft_headers_parse(req);
			++req.stage;
			i += 3;
			break ;
		}
	}
	raw.erase(0, i);
	end = raw.size();
}

//	BODY	======================================================================================

void	ft_parse_chunked_body(HTTP_Request &req, std::string &raw, int &end) {
	
	int	i = 1;

	std::string	content;

	for (; i < end && raw[i - 1] != '\r' && raw[i] != '\n';) {
		for (i = 1; i < end && !(raw[i - 1] == '\r' && raw[i] == '\n'); ++i){}
		if (raw[i - 1] == '\r' && raw[i] == '\n') {
			req.chunk_size = StringToSize_t(raw.substr(0, i - 1), HEX_BASE, req.flg_ch_sz_crct);
			req.content_lngth += req.chunk_size;
			raw.erase(0, i + 1);
			end = raw.size();
			if (!req.flg_ch_sz_crct) {
				Logger::putMsg("Wrong format of chunk size: ", FILE_WREQ, WREQ);
				req.answ_code[0] = 4;
				req.answ_code[1] = 11;
				break ;
			}
			if (req.chunk_size == 0) {
				++req.stage;
				break ;
			}
			for (i = 0; i < req.chunk_size && i < end; ++i){}
			content = raw.substr(0, i);
			if (i < req.chunk_size) {
				req.left.append(content);
			} else {
				req.body += req.left + content;
				req.left.clear();
			}
			raw.erase(0, i + 1);
			end = raw.size();
			req.chunk_size -= i;
			i = 1;
		}
		else
			req.left.append(raw.substr(0, i + 1));
	}
	raw.erase(0, 2);
}

void	ft_parse_body(HTTP_Request &req, std::string &raw, int &end) {

	if (req.base.hdrs_total_len + req.base.start_string.len + req.content_lngth > REQ_MAX_SIZE) {
		Logger::putMsg("Request is too large", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 13;
		return ;
	}

	if (end == 0 || req.stage != 52)
			return ;
	
	if (req.flg_te == 1)
		ft_parse_chunked_body(req, raw, end);
	else {
		int i;

		for (i = 0; i < end && i < req.body_left; ++i) {}

		std::string	content = raw.substr(0, i);

		if (i < req.body_left)
			req.left += content;
		else {
			req.body = req.left + content;
			++req.stage;
			req.left.clear();
		}
		req.body_left -= i;

		raw.erase(0, i);
	}
}

//	SKIP	======================================================================================

void	ft_skip(HTTP_Request &req, std::string &raw, int end) {

	int i;

	if (req.stage == 50) {
		for (i = 0; i + 1 < end && raw[i] == '\r' && raw[i + 1] == '\n'; ++i){}
		if (i + 1 >= end) {
			raw.erase(0, end);
			return ;
		}
		else {
			raw.erase(0, i + 2);
			req.stage = 51;
		}
	}
	if (req.stage == 51) {
		if (req.base.start_string.method.compare("POST")) {
			for (i = 0; i + 3 < end
					&& !(raw[i + 2] == '\r' && raw[i + 3] == '\n'
					&& raw[i] == '\r' && raw[i + 1] == '\n'); ++i){}
			if (i + 3 >= end) {
				raw.erase(0, end);
				return ;
			}
			else {
				raw.erase(0, i + 4);
				req.stage = 52;
			}
		}
		else
			ft_parse_headers(req, raw, end);
	}
	if (req.stage == 52) {
		if (req.flg_te) {
			ft_parse_chunked_body(req, raw, end);
		}
		else {
			if (end > req.content_lngth)
				raw.erase(0, req.content_lngth);
			else {
				raw.erase(0, end);
				return ;
			}
		}
	}
	
	if (req.stage != 54)
		req.stage = 59;
}

//	======================================================================================

void HTTP_Request::ft_strtoreq(HTTP_Request &req, std::string &raw) {

	int	end = raw.size();

	switch (req.stage) {
		case New: 			{ ft_parse_start_string(req, raw, end); }
		case Start_String:	{ if (req.stage != 51) break ;ft_parse_headers(req, raw, end); }
		case Headers: 		{ if (req.stage != 52) break ;ft_parse_body(req, raw, end); }
		case Ready: 		{ if (req.answ_code[0] < 4) break; }
		case Error: 		{ if (req.stage != 54) ft_skip(req, raw, end); break; }
		default:			{

			Logger::putMsg("Undefined stage 5x!!!", FILE_WREQ, WREQ);
			break;
		}
	}
}
