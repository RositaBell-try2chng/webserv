#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include "../webserv.hpp"
# include "../Server.hpp"
# include "../Logger.hpp"
# include "HTTP_Utils.hpp"
# include <cstring>
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

		int									answ_code[2];

		~HTTP_Request() {};
		HTTP_Request() {

			answ_code[0] = 2;
			answ_code[0] = 0;
		};

		// Get "raw" request and parse into a structure
		static HTTP_Request ft_strtoreq(Server &srv);

};

#endif
