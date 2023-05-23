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

void ConfParser::parseConf(char *arg, Servers** allServers)
{
    std::ifstream   in;
    bool            flgDef;
    std::string     conf;
    std::string     fileName;

    flgDef = false;
    if (!arg)
    {
        fileName = std::string(DEFAULT_CONF);
        flgDef = true;
    }
    else
        fileName = std::string(arg);
    in.open(fileName.c_str());
    if (!flgDef && !in.is_open())
    {
        Logger::putMsg(std::string(strerror(errno)) + std::string("\n") + fileName, FILE_ERR, ERR);
        Logger::putMsg(std::string("TRY to open same file in 'conf' directory\n"));
        fileName = (std::string("./conf/") + fileName);
        in.open(fileName.c_str());
    }
    if (!in.is_open())
    {
        Logger::putMsg(std::string(strerror(errno)) + std::string("\n") + fileName, FILE_ERR, ERR);
        throw cannotOpenConfigFile();
    }
    ConfParser::readAll(in, conf);
    ConfParser::deepParser(conf, allServers);
}

void ConfParser::readAll(std::ifstream &in, std::string &conf)
{
    std::string buf;

    while (std::getline(in, buf))
    {
        ConfParser::delComment(buf, '#');
        ConfParser::delSpaces(buf);
        if (!buf.empty())
            conf += buf + '\n';
    }
    in.close();
}

void ConfParser::deepParser(std::string &conf, Servers** allServers)
{
    std::string             dst;
    bool                    endFlg = false;
    bool                    correctFlg = false;

    *allServers = new Servers();
    if (conf.empty())
        throw badConfig();
    endFlg = ConfParser::getNextServer(dst, conf);
    if (endFlg)
        throw badConfig();
    while (!endFlg)
    {

        endFlg = ConfParser::getNextServer(dst, conf); //cut server config
        ConfParser::delSpaces(conf);
        if (ConfParser::parseForOneServ(dst, *allServers))
            correctFlg = true;
        else
            Logger::putMsg(std::string("BAD CONFIG:\n") + dst, FILE_ERR, ERR);
    }
    if (!correctFlg)
        throw badConfig();
}

bool ConfParser::getNextServer(std::string &dst, std::string &src)
{
    std::string::size_type i;
    std::string::size_type fromErase;
    std::string::size_type toErase;

    i = src.find("server");
    if (i == std::string::npos)
        return (true);
    fromErase = i;
    i += 6;
    skipSpaces(src, i);
    if (i == src.length() || src[i] != '{')
        throw badConfig();
    i++;
    skipSpaces(src, i);
    toErase = findCloseBracket(src, i, 1) + 1;
    dst = src.substr(i, toErase - i - 1);
    src.erase(fromErase, toErase - fromErase);
    return (false);
}

bool ConfParser::parseForOneServ(std::string &src, Servers *allServers)
{
    std::map <std::string, std::string> paramS;
    std::map <std::string, std::string> paramL;
    std::map <std::string, std::map<std::string, std::string>>  locations;
    std::vector<std::string>            ports;
    std::vector<std::string>            errPages;

    while (ConfParser::getLocations(src, paramL))
        ConfParser::delSpaces(src);
    if (src.empty() || paramL.empty())
        return (false);
    if (!ConfParser::fillServParam(paramS, ports, errPages))
        return (false);
    if (!ConfParser::fillLocations(paramL, locations))
        return (false);
    return (allServers->addServers(paramS, locations, ports, errPages));
}

bool ConfParser::fillLocations(std::map <string, string> &paramL, std::map <std::string, std::map<std::string, std::string>> &locations)
{
    std::map<std::string, std::string>::iterator    it;
    std::map<std::string, std::string>              part2;
    std::stringstream                               sStream;
    std::string                                     line;
    std::string                                     line2;
    bool                                            flgCorrect;

    for (it = paramL.begin(); it != paramL.end(); it++)
    {
        flgCorrect = true;
        part2.clear();
        sStream << it->second;
        while (std::getline(sStream, line))
        {
            ConfParser::splitLine(line, line2);
            if (line.empty() || line2.empty()) {
                Logger::putMsg("BAD CONFIG IGNORED:\n" + line + ' ' + line2, FILE_ERR, ERR);
                continue;
            }
            if (line != "acceptedMethods" && line != "root" && line != "dirListOn" && line != "defFileIfdir" && line != "CGIs" && line != "upload_path" && line != "return") {
                Logger::putMsg("BAD PARAM of Location IGNORED:\n" + line, FILE_ERR, ERR);
                continue;
            }
            if (part2.find(line) != part2.end()) {
                flgCorrect = false;
                break;
            }
            part2.insert(std::pair<std::string, std::string>(line, line2));
        }
        if (!flgCorrect || part2.empty()) {
            Logger::putMsg("BAD CONFIG OF LOCATION:\n" + it->first, FILE_ERR, ERR);
            continue;
        }
        locations.insert(std::pair<std::string, std::map<std::string, std::string>>(it->first, part2));
    }
    if (locations.empty())
        return (false);
}

bool ConfParser::fillServParam(std::map <std::string, std::string> &paramS, std::vector <std::string> &ports, std::vector <std::string> &errPages)
{
    std::stringstream   sStream;
    std::string         line;
    std::string         line2;

    sStream << src;
    while (std::getline(sStream, line))
    {
        ConfParser::splitLine(line, line2);
        if (line.empty() || line2.empty()) {
            Logger::putMsg("BAD CONFIG IGNORED:\n" + line + ' ' + line2, FILE_ERR, ERR);
            continue;
        }
        if (line == "listen" && ConfParser::checkPort(line2))
            ports.push_back(line2);
        else if (line == "error_page")
            errPages.push_back(line2);
        else if (line == "host" || line == "server_name" || line == "limitBodySize" || line == "root")
        {
            if (paramS.find(line) != paramS.end())
            {
                Logger::putMsg("DOUBLE " + line + " PARAM: " + line2, FILE_ERR, ERR);
                return (false);
            }
            if (line == "host" && !ConfParser::checkCorrectHost(line2))
            {
                Logger::putMsg("BAD " + line + " PARAM: " + line2, FILE_ERR, ERR);
                return (false);
            }
            paramS.insert(std::pair<std::string, std::string>(line, line2));
        }
        else
            Logger::putMsg("BAD CONFIG IGNORED:\n" + line + ' ' + line2, FILE_ERR, ERR);
    }
    return (true);
}

void ConfParser::splitLine(std::string &src, std::string &dst)
{
    std::string::size_type i = 0;
    std::string::size_type len;

    len = src.length();
    while (i < len && !std::isspace(src[i]))
        i++;
    dst = src.substr(i, len - i);
    src.erase(i, len - i);
    ConfParser::delSpaces(src);
    ConfParser::delSpaces(dst);
}

bool ConfParser::getLocations(std::string &src, std::map<std::string, std::string> &dst)
{
    std::string::size_type  i;
    std::string::size_type  fromErase;
    std::string::size_type  from;
    std::string             part1;
    std::string             part2;

    i = src.find("location");
    if (i == std::string::npos)
        return (false);
    fromErase = i;
    i += 8;
    ConfParser::skipSpaces(src, i);
    from = i;
    while (i < src.length() && src[i] != '{')
        i++;
    part1 = src.substr(from, i - from);
    ConfParser::delSpaces(part1);
    from = i + 1;
    i = ConfParser::findCloseBracket(src, i, 1);
    part2 = src.substr(from, i - from);
    ConfParser::delSpaces(part2);
    if (part1.empty() || part2.empty())
        Logger::putMsg("BAD CONFIG IGNORED:\n" + part1 + '\n' + part2, FILE_ERR, ERR);
    else if (dst.find(part1) != dst.end())
        Logger::putMsg("DOUBLE LOCATION IGNORED:\n" + part1, FILE_ERR, ERR);
    else
        dst.insert(std::pair<std::string, std::string>(part1, part2));
    src.erase(fromErase, i - fromErase + 1);
    return (true);
}

std::string::size_type ConfParser::findCloseBracket(std::string &src, std::string::size_type i, int cnt)
{
    //i--;
    while (cnt != 0 && ++i < src.length())
    {
        if (src[i] == '{')
            cnt++;
        else if (src[i] == '}')
            cnt--;
    }
    if (i == src.length())
        throw badConfig();
    return (i);
}

void ConfParser::skipSpaces(std::string &src, std::string::size_type &i)
{
    while (i < src.length() && std::isspace(src[i]))
        i++;
}

void ConfParser::delComment(std::string &str, char c)
{
    std::string::size_type i;

    i = str.find(c);
    if (i != std::string::npos)
        str.erase(i, str.length());
}

void ConfParser::delSpaces(std::string &str)
{
    std::string::size_type i = 0;

    while (i <= str.length() && std::isspace(str[i]))
        i++;
    str.erase(0, i);
    if (str.empty())
        return;
    i = str.length() - 1;
    while (i >= 0 && std::isspace(str[i]))
        i--;
    if (i != str.length() - 1)
        str.erase(i + 1, str.length());
}

bool ConfParser::checkCorrectHost(std::string &Host)
{
    int                     cnt = 0;
    std::string::size_type  i = 0;
    std::string::size_type  len = Host.length();
    std::string             part;

    while (i < len && std::isdigit(Host[i])) {
        part += Host[i];
        i++;
    }
    if (part.empty() || part.length() > 3)
        return (false);
    while (part.length() < 3)
        part = '0' + part;
    if (part > std::string("255"))
        return (false);
    while (i < len)
    {
        part.clear();
        if (Host[i++] != '.')
            return (false);
        cnt++;
        while (i < len && std::isdigit(Host[i])) {
            part += Host[i];
            i++;
        }
        if (part.empty() || part.length() > 3)
            return (false);
        while (part.length() < 3)
            part = '0' + part;
        if (part > std::string("255"))
            return (false);
    }
    if (cnt != 3)
        return (false);
    return (true);
}

bool ConfParser::checkPort(std::string &Port)
{
    std::string::size_type  i;

    if (Port.length() > 5)
        return (false);
    for (i = 0; i < Port.length(); i++)
    {
        if (!std::isdigit(Port[i]))
            return (false);
    }
    while (Port.length() < 5)
        Port = '0' + Port;
    if (Port > "65535")
        return (false);
    return (true);
}