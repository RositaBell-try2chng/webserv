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

#endif
