#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"

//Mesages
#define TOO_MANY_ARGS			"Too many args\nUse only first argument\n"
#define WRONG_EXTENSION_OF_FILE	"Wrong extension of config file: "
#define NOT_PASSED_ARG			"Config file doesn't passed."
#define TRY_DEFAULT				"Trying to use default config file:\n"

#define DEFAULT_CONF "./conf/serv.conf"

# define DELIMITER_START "//////////////////\n\nSTART OF ANOTHER LAUNCH\n\n//////////////////"
# define DELIMITER_END "//////////////////\n\nEND OF ANOTHER LAUNCH\n\n//////////////////"

class Server
{
private:
	Server();
	~Server();
	static std::string	conf;
	static t_listen		*main;
public:
	static void				startServer(); //запускаем сервер
	static bool				checkArgs(int args, char **argv); //проверяет количество аргументов + расширение имени файла
	static bool				parse(); //парсим конфиг
	static void				exitHandler(int sig);
	static const t_listen	&getMain();
};

#endif