#ifndef REQUEST_PARSE_HPP
# define REQUEST_PARSE_HPP

# include "../webserv.hpp"

struct HTTP_Request
{
	private:

		HTTP_Request();
		~HTTP_Request();

	public:

		std::string							method;
		std::string							uri;
		std::string							version;

		std::map<std::string, std::string>	headers;
		
		std::string							body;

	// Get "raw" request and parse into structure
		HTTP_Request ft_strtoreq(std::string raw);

		
		
};



#endif
