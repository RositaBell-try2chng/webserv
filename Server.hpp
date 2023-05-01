#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"

//Mesages
#define TOO_MANY_ARGS			"Too many args\nUse only first argument\n"
#define WRONG_EXTENSION_OF_FILE	"Wrong extension of config file"
#define NOT_PASSED_ARG			"Config file doesn't passed.\n"
#define TRY_DEFAULT				"Trying to use default config file:\n"

class Server
{
private:
	Server();
	~Server();

	static std::string	configFile;
	static int			socketFd;
	static std::string	errorMsg;
public:
	static void	startServer();

	//getters
	static std::string const &getErrMsg();
	//parse arguments
	static bool	checkArgs(int args, char **argv); //проверяет количество аргументов + расширение имени файла
	static bool	parse(bool flgSecond);
	//вывод сообщений
	static int	putErrMsg(std::string msg = Server::errorMsg);
	static int	putMsg(std::string msg);
};

#endif