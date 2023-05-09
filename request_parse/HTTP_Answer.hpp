#ifndef HTTP_ANSWER_HPP
# define HTTP_ANSWER_HPP

# include "../webserv.hpp"
# include "../Logger.hpp"
# include "HTTP_Utils.hpp"
# include "HTTP_Request.hpp"
# include <map>

struct HTTP_Answer {

	private:

	public:

		std::string	version;
		std::string	status_code;
		std::string	reason_phrase;

		std::map<std::string, std::string>	headers;
		
		std::string							body;

		~HTTP_Answer();
		HTTP_Answer();

		// Get request and make answer for it
		static int ft_reqtoansw(HTTP_Request req, HTTP_Answer *answ);
};

#endif
