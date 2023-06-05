#ifndef HTTP_HEADERS_HPP
# define HTTP_HEADERS_HPP

# include "../webserv.hpp"
# include "../Logger.hpp"
# include "HTTP_Request.hpp"
# include <map>

enum Basic_Headers {    Host,
                        Something_one,
                        Something_two,
                        Something_three,
                        Something_four,
                        Something_five,
                        Something_six       };

int     ft_if_basic_hdr(std::string key);

void    ft_headers_parse(std::map<std::string, std::string> hdrs, HTTP_Request req);

#endif
