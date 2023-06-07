#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include "../webserv.hpp"
# include "../Logger.hpp"
# include "HTTP_Utils.hpp"
# include <cstring>
# include <map>

struct HTTP_Request
{
	private:

		

	public:

		struct ContentType
		{
			bool media_type = false;

			std::string 						type;
			std::string 						subtype;
			std::map<std::string, std::string>	options;
		};

		struct Date
		{
			std::string	txt_day_of_week;
			short int	day_of_week;

			short int	day;
			std::string	txt_month;
			short int	month;
			short int	year;

			short int	sec;
			short int	min;
			short int	hrs;
		};

		std::string							method;
		std::string							uri;
		std::string							version;

		std::map<std::string, std::string>	headers;

		std::string	host;
		std::string	port;
		short int	flg_cnnctn = 1;	// 0 - close, 1 - keep alive, 2 - upgrade HTTP version
		int			content_lngth;	// length of request body in bytes						
		ContentType	content_type;	// media type of the body of the request
		Date		date;			// the date and time at which the message was originated
		short int	flg_te = 0;		// 0 - full, 1 - chunked

		std::string							body;

		int									answ_code[2];

		~HTTP_Request() {};
		HTTP_Request() {

			answ_code[0] = 2;
			answ_code[0] = 0;
		};

		// Get "raw" request and parse into a structure
		static HTTP_Request ft_strtoreq(std::string raw, int limitCLientBodySize);

};

#endif
