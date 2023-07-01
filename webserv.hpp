#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <sstream>
# include <vector>
# include <fstream>
# include <ctime>
# include <cerrno>
# include <cstring>
# include <string>
# include <csignal>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <netdb.h>
# include <unistd.h>
# include <set>
# include <map>
# include <fcntl.h>
# include <algorithm>
# include <dirent.h>


# define DEF_HOST "127.0.0.1"

# define BUF_SIZE 25000
# define BUF_SIZE_PIPE 16000

# define DEF_RESPONSE "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: close\r\n\r\nHello, world!"

# define STANDART_ENV_VARS_CNT 11;

# define BINARY_BASE std::string("01", 2)
# define OCT_BASE  std::string("01234567", 8)
# define DEC_BASE  std::string("0123456789", 10)
# define HEX_BASE  std::string("0123456789ABCDEF", 16)

# define LAST_CHUNK std::string("0\r\n\r\n", 5);

# define CNT_TRYING 3
# define TIMEOUT 15 //in sec

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

void delSpaces(std::string &str);
bool checkCorrectHost(std::string &Host);
ssize_t strToSSize_t(std::string const &src, ssize_t limit);
std::string Size_tToString(std::string::size_type src, std::string const &base);
std::string::size_type StringToSize_t(std::string src, std::string const &base, bool &flgCorrect);

#endif