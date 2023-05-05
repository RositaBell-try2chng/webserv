#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "webserv.hpp"
# include <ctime>

# define ERR_LOGS	"./logs/errors.txt"
# define INFO_LOGS	"./logs/info.txt"
# define QUERIES	"./logs/queries.txt"

class Logger
{
private:
	Logger();
	~Logger();
public:
	static int	putErrMsg(std::string const &msg);
	static void putInfoMsg(std::string const &msg, bool flgError);
	static void putQuery(t_use &use);
	static std::string getTime();
};

#endif