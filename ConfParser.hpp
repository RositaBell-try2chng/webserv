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
    static void                     deepParser(std::string& conf, Servers** allServers);
    static void                     readAll(std::ifstream& in, std::string& conf);
    static void                     delComment(std::string &str, char c);
    static bool                     getNextServer(std::string &dst, std::string &src);
    static void                     skipSpaces(std::string &src, std::string::size_type &i);
    static std::string::size_type   findCloseBracket(std::string &src, std::string::size_type i, int cnt);

    static bool                     parseForOneServ(std::string &dst, Servers *allServers);

    static bool                     getLocations(std::string &src, std::map<std::string, std::string> &dst);
    static bool                     fillServParam(std::string &src, std::map<std::string, std::string> &paramS, std::vector<std::string> &ports, std::vector<std::string> &errPages);
    static bool                     fillLocations(std::map <std::string, std::string> &paramL, std::map <std::string, std::map<std::string, std::string> >  &locations);

    static bool                     checkCorrectHost(std::string &Host);
    static bool                     checkPort(std::string &Port);
public:
    static bool checkArgs(int args, char **argv);
    static void parseConf(char* arg, Servers** allServers);
    static void splitLine(std::string &src, std::string &dst);
    static void delSpaces(std::string &str);
};

#endif