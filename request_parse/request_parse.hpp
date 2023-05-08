#ifndef REQUEST_PARSE_HPP
# define REQUEST_PARSE_HPP

# include "../webserv.hpp"

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
		HTTP_Request ft_strtoreq(std::string raw);		
};

#endif
