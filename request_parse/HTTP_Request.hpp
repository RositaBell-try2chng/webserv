#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include "../webserv.hpp"
# include "../Logger.hpp"
# include "HTTP_Utils.hpp"
# include "HTTP_Headers.hpp"
# include <cstring>
# include <map>

# define REQ_MAX_SIZE		100000000
# define HDR_MAX_LEN		4096
# define HDRS_MAX_SUM_LEN	8192

enum Request_Parse_Stage	{	New = 50,
								Start_String,
								Headers,
								Ready,
								Error				};

struct HTTP_Request {
	
	private:

		

	public:

		struct ContentType {

			bool media_type;

			std::string 						type;
			std::string 						subtype;
			std::map<std::string, std::string>	options;

			ContentType() : media_type(false) {};
		};

		struct Date {

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

		struct Base {

			struct StartString {

				std::string	method;
				std::string	uri;
				std::string	prmtrs;
				std::string	version;

				int	len;
			};

			StartString							start_string;

			std::map<std::string, std::string>	headers;
			int									hdrs_total_len;

			Base() : hdrs_total_len(0){}
		};

		std::string	left;	//	unparsed part of request

		int			stage;	//	50 - start
							//	51 - start string was fully parsed
							//	52 - headers was fully parsed
							//	53 - body was fully parsed
							//	54 - CLOSE
							//	59 - errorHTTP_Request

		Base		base;

		std::string	host;
		std::string	port;
		short int	flg_cnnctn;		// 0 - close, 1 - keep alive, 2 - upgrade HTTP version
		int			content_lngth;	// length of request body in bytes
		int			chunk_size;	
		int			body_left;		// left from body					
		ContentType	content_type;	// media type of the body of the request
		Date		date;			// the date and time at which the message was originated
		short int	flg_te;			// 0 - full, 1 - chunked
		bool		flg_ch_sz_crct;	// 0 - wrong format of chunk size, 1 - correct format of chunk size

		std::string							body;

		int									answ_code[2];

		~HTTP_Request() {};
		HTTP_Request() : stage(50), flg_cnnctn(1), content_lngth(0), body_left(0), flg_te(0), flg_ch_sz_crct(true) {

			this->content_type = ContentType();
			this->base = Base();
			answ_code[0] = 2;
			answ_code[1] = 0;
		};

		// Get "raw" request and parse into a structure
		static void ft_strtoreq(HTTP_Request &req, std::string &raw);

};

#endif
