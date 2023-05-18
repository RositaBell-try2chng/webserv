#ifndef CONFPARSER_HPP
# define CONFPARSER_HPP

# include "webserv.hpp"
# include "Logger.hpp"
# include "Servers.hpp"
# include "Exceptions.hpp"

class ConfParser
{
private:
    ConfParser();
    ~ConfParser();
    static bool gagParser(Servers* allServers);
    static bool deepParser(std::string const& conf, Servers** allServers);
    static void readAll(std::ifstream& in, std::string& conf);
public:
    static bool checkArgs(int args, char **argv);
    static bool parseConf(char* arg, Servers** allServers);
};

#endif