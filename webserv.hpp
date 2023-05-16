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