#include "HTTP_Headers.hpp"

//======	SPECIFIC HEADERS HANDLERS	 ===========================================================

void	ft_hdr_host(std::string &host, short int &port, std::string value) {

	std::pair<std::string, std::string> full_host = ft_strtopair(value, ':');

	host = full_host.first;

	std::stringstream strm(full_host.second);
	strm >> port;
}

short int	ft_hdr_connection(std::string value) {

	if (value.compare("close"))
		return 0;
	if (value.compare("keep-alive"))
		return 1;
	if (value.compare("Upgrade"))
		return 2;
	return 3;
}

int	ft_hdr_content_length(std::string value) {

	std::stringstream strm(value);
	int	length;
	
	strm >> length;

	return length;
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

	for (; i != end; ++i) {
		for (++i; i != end && value[i] != ';'; ++i)
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

	if (te.compare("chunked"))
		return 1;
	return 0;
}

//======	BASIC INFRASTRUCTURE	================================================================

void	ft_headers_parse(std::map<std::string, std::string> hdrs, HTTP_Request &req) {

	std::map<std::string, std::string>::iterator end = hdrs.end();

	for (std::map<std::string, std::string>::iterator it = hdrs.begin(); it != end; ++it) {
		switch (ft_if_basic_hdr(it->first)) {
			case Host:				{ ; break; }
			case Connection:		{ req.flg_cnnctn = ft_hdr_connection(it->second); break;}
			case Content_Length:	{ req.content_lngth = ft_hdr_content_length(it->second); break; }
			case Content_Type:		{ ft_hdr_content_type(req.content_type, it->second); break; }
			case Date:				{ ft_hdr_date(req.date, it->second); break; }
			case Transfer_Encoding:	{ req.flg_te = ft_hdr_te(it->second); break; }

			default: {
				Logger::putMsg("Header " + it->first + " is not supported", FILE_WREQ, WREQ);
				break; 
			}
		}
	}
}

int		ft_if_basic_hdr(std::string key) {

	std::string basic_hdrs[] = {	"Host",
									"Connection",
									"Content-Length",
									"Content-Type",
									"Date",
									"Transfer-Encoding",
									"Stop"              	};

	for (int i = 0; ; ++i) {
		if (!key.compare(basic_hdrs[i]))
			return i;
		if (!basic_hdrs[i].compare("Stop"))
			break ;
	}

	return 1000;
}
