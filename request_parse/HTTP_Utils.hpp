#ifndef HTTP_UTILS_HPP
# define HTTP_UTILS_HPP

# include "../webserv.hpp"
# include <map>

// Get "raw" string, beginning position of headers and parse into map and body
std::string ft_strtohdrs(std::string raw, int i, int end,
		std::map<std::string, std::string> *hdrs,
		std::string *body);


std::string ft_hdrstostr(std::map<std::string, std::string> headers);

#endif