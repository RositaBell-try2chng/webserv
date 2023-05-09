#ifndef HTTP_UTILS_HPP
# define HTTP_UTILS_HPP

# include "../webserv.hpp"
# include <map>

// Get "raw" string, beginning position of headers and parse into map and body
static int ft_strtohdrs(std::string raw, int i, int end,
			std::map<std::string, std::string> *hdrs,
			std::string *body);

#endif