#ifndef CONFPARSER_HPP
# define CONFPARSER_HPP

# include "webserv.hpp"
# include "Logger.hpp"

//Mesages
#define TOO_MANY_ARGS			"Too many args\nUse only first argument\n"
#define WRONG_EXTENSION_OF_FILE	"Wrong extension of config file: "
#define NOT_PASSED_ARG			"Config file doesn't passed."
#define TRY_DEFAULT				"Trying to use default config file:\n./conf/serv.conf"

#define DEFAULT_CONF "./conf/serv.conf"

class ConfParser
{
private:
    ConfParser();
    ~ConfParser();
public:
    static bool checkArgs(int args, char **argv);
    static bool parseConf(char* arg);
};

#endif