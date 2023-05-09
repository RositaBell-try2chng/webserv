#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include "../webserv.hpp"
# include "HTTP_Answer.hpp"
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

		~HTTP_Request();
		HTTP_Request();

		// Get "raw" request and parse into a structure
		static int ft_strtoreq(std::string raw, HTTP_Request *req);

};

#endif
