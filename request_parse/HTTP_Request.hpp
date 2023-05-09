#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include "../webserv.hpp"
# include <map>

struct HTTP_Answer {

	private:

	public:

		std::string	version;
		std::string	status_code;
		std::string	reason_phrase;

		std::map<std::string, std::string>	headers;
		
		std::string							body;

		//static int ft_reqtoansw(std::string raw, HTTP_Request *req);
};

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

// Get "raw" string, beginning position of headers and parse into map and body
static int ft_strtohdrs(std::string raw, int i, int end,
			std::map<std::string, std::string> *hdrs,
			std::string *body);

#endif
