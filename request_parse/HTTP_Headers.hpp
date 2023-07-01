#ifndef HTTP_HEADERS_HPP
# define HTTP_HEADERS_HPP

# include "../webserv.hpp"
# include "../Logger.hpp"
# include "HTTP_Utils.hpp"
# include "HTTP_Request.hpp"
# include <map>


# define MONDAY		"Mon"
# define TUESDAY	"Tue"
# define WEDNESDAY	"Wed"
# define THURSDAY	"Thu"
# define FRIDAY		"Fri"
# define SATURDAY	"Sat"
# define SUNDAY		"Sun"

enum Weekday	{	Monday = 1,
					Tuesday,
					Wednesday,
					Thursday,
					Friday,
					Saturday,
					Sunday,
					Neverday	};


# define JANUARY	"Jan"
# define FEBRUARY	"Feb"
# define MARCH		"Mar"
# define APRIL		"Apr"
# define MAY		"May"
# define JUNE		"Jun"
# define JULY		"Jul"
# define AUGUST		"Aug"
# define SEPTEMBER	"Sep"
# define OCTOBER	"Oct"
# define NOVEMBER	"Nov"
# define DECEMBER	"Dec"

enum Month	{	January = 1,
				February,
				March,
				April,
				May,
				June,
				July,
				August,
				September,
				October,
				November,
				December,
				Nevermonth	};


enum Basic_Headers	{	Host,
						Connection,
						Content_Length,
						Content_Type,
						Date,
						Transfer_Encoding,
						Accept_Encoding,
						User_Agent,
						Something_else	};

struct HTTP_Request;

int     ft_if_basic_hdr(std::string key, std::string basic_hdrs[]);
void    ft_headers_parse(HTTP_Request &req);

#endif
