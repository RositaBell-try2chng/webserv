#include "HTTP_Headers.hpp"

//======	SPECIFIC HEADERS HAMDLERS	 ===========================================================

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

short int	ft_hdr_content_type(HTTP_Request::ContentType &type, std::string value) {

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



//======	BASIC INFRASTRUCTURE	================================================================

void	ft_headers_parse(std::map<std::string, std::string> hdrs, HTTP_Request &req) {

	std::map<std::string, std::string>::iterator end = hdrs.end();

	for (std::map<std::string, std::string>::iterator it = hdrs.begin(); it != end; ++it) {
		switch (ft_if_basic_hdr(it->first)) {
			case Host:			{ ; break; }
			case Connection:	{ req.flg_connection = ft_hdr_connection(it->second); break;}
			case Content_Length:{ req.content_lngth = ft_hdr_content_length(it->second); break; }
			case Content_Type:	{ ft_hdr_content_type(req.content_type, it->second); break; }
			case Date:			{ ; break; }

			default: { break; }
		}
	}
}

int		ft_if_basic_hdr(std::string key) {

	std::string basic_hdrs[] = {	"Host",
									"Connection",
									"Content-Length",
									"Content-Type",
									"Something_four",
									"Stop"              };

	for (int i = 0; ; ++i) {
		if (!key.compare(basic_hdrs[i]))
			return i;
		if (!basic_hdrs[i].compare("Stop"))
			break ;
	}

	return 1000;
}
