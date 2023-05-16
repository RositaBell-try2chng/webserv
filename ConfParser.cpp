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

bool ConfParser::parseConf(char *arg, Servers* allServers)
{
    std::ifstream   in;
    bool            flgDef;
    std::string     conf;

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
        throw cannotOpenConfigFile();
    }
    ConfParser::readAll(in, conf);
    return (ConfParser::deepParser(conf, allServers));
}

void ConfParser::readAll(std::ifstream &in, std::string &conf)
{
    std::string buf;

    while (std::getline(in, buf))
        conf += buf;
    in.close();
}

bool ConfParser::deepParser(const std::string &conf, Servers* allServers)
{
    allServers = new Servers();
    if (conf.empty())
        return (ConfParser::gagParser(allServers));
    //fix me: add deep parsing of config file
    std::cout << "deep parsing\n";
    return (true);
}

bool ConfParser::gagParser(Servers* allServers)
{
    allServers->createServer(DEF_HOST, DEF_PORT);
    return (true);
}