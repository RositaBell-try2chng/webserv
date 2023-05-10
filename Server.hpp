#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"

#define BUF_SIZE 4096

//Mesages
#define TOO_MANY_ARGS			"Too many args\nUse only first argument\n"
#define WRONG_EXTENSION_OF_FILE	"Wrong extension of config file: "
#define NOT_PASSED_ARG			"Config file doesn't passed."
#define TRY_DEFAULT				"Trying to use default config file:\n"

#define DEFAULT_CONF "./conf/serv.conf"

# define DELIMITER_START "//////////////////\n\nSTART OF ANOTHER LAUNCH\n\n//////////////////"
# define DELIMITER_END "//////////////////\n\nEND OF ANOTHER LAUNCH\n\n//////////////////"

#define ANSWER "HTTP/1.1 200 OK"
#define ANSWER_LEN 15

class Server
{
private:
	Server();
	~Server();

	static std::string		conf;
	static t_listen			*main;

	static void	prepareServ();
	static void	fillHints();
	static void	mainLoop();
	static void	recvAll(std::set<int>::iterator &it, std::set<int> &clients, std::string &res);
	static void	acceptNewConnection(std::set<int> &clients);
	static void	communicate(std::set<int> &clients, std::string &request, fd_set *readFds);
public:
	static void				startServer(); //запускаем сервер
	static bool				checkArgs(int args, char **argv); //проверяет количество аргументов + расширение имени файла
	static bool				parse(); //парсим конфиг
	static void				exitHandler(int sig);
	static const t_listen	&getMain();
};

#endif