#ifndef LOGGER_HPP
# define LOGGER_HPP

# include "webserv.hpp"
# include <ctime>

# define LOGS		"./logs/"

# define DEF_FILE	"info.txt"
# define DEF_TYPE	"INFO"

# define FILE_ERR	"errors.txt"
# define ERR		"ERROR"

# define FILE_REQ	"requests.txt"
# define REQ		"REQUEST"

# define FILE_WREQ	"wrong_requests.txt"
# define WREQ		"WRONG_REQUEST"

# define DELIMITER_START "//////////////////\n\nSTART OF ANOTHER LAUNCH\n\n//////////////////"
# define DELIMITER_END "//////////////////\n\nEND OF ANOTHER LAUNCH\n\n//////////////////"

class Logger
{
private:
	Logger();
	~Logger();
	static std::string	getTime();
public:
//	static int	putErrMsg(std::string const &msg);
//	static void putInfoMsg(std::string const &msg, bool flgError);
//	static void putQuery(t_use &use);
	static void	putMsg(std::string const &msg, std::string const &filename = DEF_FILE, std::string const &typeOfMsg = DEF_TYPE);
	static void	putMsg(std::string const &msg, std::string const &filename = DEF_FILE, std::string const &typeOfMsg = DEF_TYPE, int fd);
};

#endif