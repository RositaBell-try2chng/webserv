#include "ConfParser.hpp"

ConfParser::ConfParser() {};
ConfParser::~ConfParser() {};

bool ConfParser::checkArgs(int args, char **argv)
{
    std::string res;

    Logger::putMsg("start parsing arguments");
    if (args == 1)
    {
        Logger::putMsg(std::string(NOT_PASSED_ARG), FILE_ERR, ERR);
        Logger::putMsg(std::string(TRY_DEFAULT));
        return (false);
    }
    res = std::string(argv[1]);
    if (args > 2)
        Logger::putMsg(std::string(TOO_MANY_ARGS) + res, FILE_ERR, ERR);
    if (res.rfind(".conf") != res.length() - 5)
    {
        Logger::putMsg(std::string(WRONG_EXTENSION_OF_FILE) + res, FILE_ERR, ERR);
        Logger::putMsg(std::string(TRY_DEFAULT));
        return (false);
    }
    Logger::putMsg("parsing arguments SUCCESS");
    return (true);
}

bool ConfParser::parseConf(char *arg)
{
    //fix me: add deep parsing of config file
    std::ifstream   in;
    bool            flgDef;

    flgDef = false;
    if (!arg)
    {
        arg = DEFAULT_CONF;
        flgDef = true;
    }
    in.open(arg);
    if (!flgDef && !in.is_open())
    {
        Logger::putMsg(std::string(strerror(errno)) + std::string("\n") + std::string(arg), FILE_ERR, ERR);
        Logger::putMsg(std::string("TRY to open same file in 'conf' directory\n"));
        arg = (std::string("./conf/") + std::string(arg)).c_str();
        in.open(arg);
    }
    if (!in.is_open())
    {
        Logger::putMsg(std::string(strerror(errno)) + std::string("\n") + std::string(arg), FILE_ERR, ERR);
        return (false);
    }
    in.close();
    Logger::putMsg("parsing config file SUCCESS");
    return (true);
}