#ifndef HTTP_ANSWER_HPP
# define HTTP_ANSWER_HPP

# include "../webserv.hpp"
# include "../Logger.hpp"
# include "HTTP_Utils.hpp"
# include "HTTP_Request.hpp"
# include "Specific_Codes.hpp"
# include <map>
struct HTTP_Request;

struct HTTP_Answer {

	private:

		// make std::string from int
		static void ft_pars_status_code(int *integ_code, std::string *str_code);

		// Sets answer's reason phrase depends on status code
		static std::string ft_set_rp(int *status_code);

		// Acts depends on request's method (GET, POST, DELETE)
		static void ft_get_answ(HTTP_Request req, HTTP_Answer *answ);
		static void ft_post_answ(HTTP_Request req, HTTP_Answer *answ);
		static void ft_delete_answ(HTTP_Request req, HTTP_Answer *answ);

	public:

		std::string	version;
		std::string	status_code;
		std::string	reason_phrase;

		std::map<std::string, std::string>	headers;
		
		std::string							body;

		~HTTP_Answer() {};
		HTTP_Answer() {};

		// Parse answer to string (concatenation)
		static std::string ft_answtostr(HTTP_Answer answ);

		// Creates answer depends on request
		static HTTP_Answer ft_reqtoansw(HTTP_Request req);
};

#endif
