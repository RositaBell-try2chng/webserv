#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <fstream>
# include <sys/time.h>
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

# define DEF_PORT "8080"
# define DEF_HOST "127.0.0.1"

# define BUF_SIZE 4096
# define TIMEOUT 15

#define DEF_RESPONSE "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: close\r\n\r\nHello, world!"

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

//typedef struct	s_use
//{
//	int						resRecv;
//	int 					sockFd;
//	struct sockaddr_storage	addr;
//	socklen_t				addrSize;
//	char					buf[1000];
//} t_use;

#endif