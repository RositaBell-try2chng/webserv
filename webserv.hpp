#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <sstream>
# include <vector>
# include <fstream>
# include <ctime>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include "cstring"
# include <csignal>
# include <unistd.h>
# include <set>
# include <map>
# include <fcntl.h>
# include <algorithm>

# define DEF_HOST "127.0.0.1"

# define BUF_SIZE 16384

# define DEF_RESPONSE "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: close\r\n\r\nHello, world!"

# define STANDART_ENV_VARS_CNT 10;

//Mesages
#define TOO_MANY_ARGS			"Too many args\nUse only first argument\n"
#define WRONG_EXTENSION_OF_FILE	"Wrong extension of config file: "
#define NOT_PASSED_ARG			"Config file doesn't passed."
#define TRY_DEFAULT				"Trying to use default config file:\n./conf/serv.conf"

#define DEFAULT_CONF "./conf/serv.conf"

typedef struct	s_listen
{
	int						stts;
	int						sockFd;
	struct addrinfo			hints;
	struct addrinfo			*info;
} t_listen;

class Server;
class Servers;

#endif