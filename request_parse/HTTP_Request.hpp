#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include "../webserv.hpp"
# include "../Logger.hpp"
# include "HTTP_Utils.hpp"
# include <map>

struct HTTP_Request
{
	private:


	public:

		std::string							method;
		std::string							uri;
		std::string							version;

		std::map<std::string, std::string>	headers;
		
		std::string							body;

		std::string							answ_code;

		~HTTP_Request() {};
		HTTP_Request() : answ_code("200") {};

		// Get "raw" request and parse into a structure
		static void ft_strtoreq(std::string raw, HTTP_Request *req);

};

#endif
