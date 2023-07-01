#include "HTTP_Headers.hpp"

//======	SPECIFIC HEADERS HANDLERS	 ===========================================================

void	ft_hdr_host(std::string &host, std::string &port, std::string value) {

	std::pair<std::string, std::string> full_host = ft_strtopair(value, ':');

	host = full_host.first;
	port = full_host.second;
}

short int	ft_hdr_connection(std::string value) {

	
	if (!value.compare("close"))
		return 0;
	if (!value.compare("keep-alive"))
		return 1;
	if (!value.compare("Upgrade"))
		return 2;
	return 3;
}

void	ft_hdr_content_length(int &content_length, int &body_left, std::string value) {


	std::stringstream strm(value);

	strm >> content_length;

	body_left = content_length;
}

void	ft_hdr_content_type(HTTP_Request::ContentType &type, std::string value) {

	type.media_type = true;

	int	end = value.size() - 1;
	int i;

	for (i = 0; i != end && value[i] != '/'; ++i)
		type.type.push_back(value[i]);
	for (++i; i != end && value[i] != ';'; ++i)
		type.subtype.push_back(value[i]);
	std::string	option;
	for (; i < end; ++i) {
		for (++i; i < end && value[i] != ';'; ++i)
			option.push_back(value[i]);
		type.options.insert(ft_strtopair(option, '='));
		option.clear();
	}
}

void	ft_hdr_date(HTTP_Request::Date &date, std::string str) {

	int	end = str.size() - 1;

	int	i;

	for (i = 0; str[i] != end && str[i] != ','; ++i)
		date.txt_day_of_week.push_back(str[i]);
	
	if (date.txt_day_of_week.compare(MONDAY))
		date.day_of_week = Monday;
	else if (date.txt_day_of_week.compare(TUESDAY))
		date.day_of_week = Tuesday;
	else if (date.txt_day_of_week.compare(WEDNESDAY))
		date.day_of_week = Wednesday;
	else if (date.txt_day_of_week.compare(THURSDAY))
		date.day_of_week = Thursday;
	else if (date.txt_day_of_week.compare(FRIDAY))
		date.day_of_week = Friday;
	else if (date.txt_day_of_week.compare(SATURDAY))
		date.day_of_week = Saturday;
	else if (date.txt_day_of_week.compare(SUNDAY))
		date.day_of_week = Sunday;
	else
		date.day_of_week = Neverday;
	++i;

	std::string tmp;

	for (++i; str[i] != end && str[i] != ' '; ++i)
		tmp.push_back(str[i]);
	std::stringstream strm(tmp);
	tmp.clear();
	strm >> date.day;
	strm.clear();

	if (date.txt_month.compare(JANUARY))
		date.day_of_week = January;
	else if (date.txt_month.compare(FEBRUARY))
		date.day_of_week = February;
	else if (date.txt_month.compare(MARCH))
		date.day_of_week = March;
	else if (date.txt_month.compare(APRIL))
		date.day_of_week = April;
	else if (date.txt_month.compare(MAY))
		date.day_of_week = May;
	else if (date.txt_month.compare(JUNE))
		date.day_of_week = June;
	else if (date.txt_month.compare(JULY))
		date.day_of_week = July;
		else if (date.txt_month.compare(AUGUST))
		date.day_of_week = August;
	else if (date.txt_month.compare(SEPTEMBER))
		date.day_of_week = September;
	else if (date.txt_month.compare(OCTOBER))
		date.day_of_week = October;
	else if (date.txt_month.compare(NOVEMBER))
		date.day_of_week = November;
	else if (date.txt_month.compare(DECEMBER))
		date.day_of_week = December;
	else
		date.day_of_week = Nevermonth;

	for (++i; str[i] != end && str[i] != ' '; ++i)
		tmp.push_back(str[i]);
	strm << tmp;
	tmp.clear();
	strm >> date.year;
	strm.clear();

	for (++i; str[i] != end && str[i] != ':'; ++i)
		tmp.push_back(str[i]);
	strm << tmp;
	tmp.clear();
	strm >> date.hrs;
	strm.clear();

	for (++i; str[i] != end && str[i] != ':'; ++i)
		tmp.push_back(str[i]);
	strm << tmp;
	tmp.clear();
	strm >> date.min;
	strm.clear();

	for (++i; str[i] != end; ++i)
		tmp.push_back(str[i]);
	strm << tmp;
	tmp.clear();
	strm >> date.sec;
	strm.clear();
}

short int ft_hdr_te(std::string te) {

	if (!te.compare("chunked"))
		return 1;		
	return 0;
}

//======	BASIC INFRASTRUCTURE	================================================================

void	ft_headers_parse(HTTP_Request &req) {

	std::string basic_hdrs[] = {	"Host",
									"Connection",
									"Content-Length",
									"Content-Type",
									"Date",
									"Transfer-Encoding",
									"Stop",
									"User-Agent"              	};

	std::map<std::string, std::string>::iterator end = req.base.headers.end();

	for (std::map<std::string, std::string>::iterator it = req.base.headers.begin(); it != end; ++it) {
		switch (ft_if_basic_hdr(it->first, basic_hdrs)) {
			case Host:				{ ft_hdr_host(req.host, req.port, it->second); break ; }
			case Connection:		{ req.flg_cnnctn = ft_hdr_connection(it->second); break ;}
			case Content_Length:	{ ft_hdr_content_length(req.content_lngth, req.body_left, it->second); break ; } //fix me: add req.content_lngth as first param
			case Content_Type:		{ ft_hdr_content_type(req.content_type, it->second); break ; }
			case Date:				{ ft_hdr_date(req.date, it->second); break ; }
			case Transfer_Encoding:	{ req.flg_te = ft_hdr_te(it->second); break ; }

			default: {
				Logger::putMsg("Header " + it->first + " is not supported", FILE_WREQ, WREQ);
				break ; 
			}
		}
	}
	if (req.base.headers.find("Host") == req.base.headers.end()) {
		Logger::putMsg("Request hasn't \"Host\" header", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 0;
	}
	if (!req.base.start_string.method.compare("POST")
			&& req.base.headers.find("Content-Length") == req.base.headers.end()
			&& (req.base.headers.find("Transfer-Encoding") == req.base.headers.end()
			|| req.base.headers.find("Transfer-Encoding")->second.compare("chunked"))) {
		Logger::putMsg("Request must had length but didn\'t", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 11;
	}
	if (req.flg_te == 1) {
		if (req.base.headers.find(basic_hdrs[2]) != req.base.headers.end()) {	//	"Content-Length"
			Logger::putMsg("Request has both \"length\" headers", FILE_WREQ, WREQ);
			req.answ_code[0] = 4;
			req.answ_code[1] = 0;
			req.stage = 53;
		}
	}
}

int		ft_if_basic_hdr(std::string key, std::string basic_hdrs[]) {

	for (int i = 0; ; ++i) {
		if (!key.compare(basic_hdrs[i]))
			return i;
		if (!basic_hdrs[i].compare("Stop"))
			break ;
	}

	return 1000;
}
