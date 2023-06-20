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
	bool			flgDef;
	std::string	 conf;
	std::string	 fileName;

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
		delSpaces(buf);
		if (!buf.empty())
			conf += buf + '\n';
	}
	in.close();
}

void ConfParser::deepParser(std::string &conf, Servers** allServers)
{
	std::string			 dst;
	bool					endFlg;

	if (conf.empty())
		throw badConfig();
	endFlg = ConfParser::getNextServer(dst, conf);
	if (endFlg)
	{
		Logger::putMsg("BAD CONFIG:\nHAVE NO servers.", FILE_ERR, ERR);
		throw badConfig();
	}
	*allServers = new Servers();
	while (!endFlg)
	{
		endFlg = ConfParser::getNextServer(dst, conf); //cut server config
		delSpaces(conf);
		delSpaces(dst);
		ConfParser::parseForOneServ(dst, *allServers);
	}
	if (!conf.empty())
	{
		Logger::putMsg("BAD CONFIG:\nsomething out of server:\n" + conf, FILE_ERR, ERR);
		throw badConfig();
	}
}

bool ConfParser::getNextServer(std::string &dst, std::string &src)
{
	std::string::size_type i;
	std::string::size_type fromErase;
	std::string::size_type toErase;

	i = src.find("server");
	while (i != std::string::npos && !(i == 0 || (std::isspace(src[i - 1]) && std::isspace(src[i + 6]))))
		i = src.find("server", i + 1);
	if (i == std::string::npos)
		return (true);
	fromErase = i;
	i += 6;
	skipSpaces(src, i);
	if (i == src.length() || src[i] != '{')
	{
		Logger::putMsg("BAD CONFIG:\n" + src, FILE_ERR, ERR);
		throw badConfig();
	}
	i++;
	skipSpaces(src, i);
	toErase = findCloseBracket(src, i, 1) + 1;
	dst = src.substr(i, toErase - i - 1);
	src.erase(fromErase, toErase - fromErase);
	return (false);
}

void ConfParser::parseForOneServ(std::string &src, Servers *allServers)
{
	std::map <std::string, std::string> paramS;
	std::map <std::string, std::string> paramL;
	std::map <std::string, std::map<std::string, std::string> >  locations;
	std::vector<std::string>			ports;
	std::vector<std::string>			errPages;

	while (ConfParser::getLocations(src, paramL)) //отделяем параметры маршрутов location
		delSpaces(src);
	if (src.empty() ) // нет параметров сервера
	{
		Logger::putMsg("BAD CONFIG:\nEmpty server parameters", FILE_ERR, ERR);
		throw badConfig();
	}
	if (paramL.empty()) //нет параметров маршрутизации для сервера
	{
		Logger::putMsg("BAD CONFIG:\nServer have no locations", FILE_ERR, ERR);
		throw badConfig();
	}
	if (!ConfParser::fillServParam(src, paramS, ports, errPages))
		throw badConfig();
	if (!ConfParser::fillLocations(paramL, locations))
		throw badConfig();
	allServers->addServers(paramS, locations, ports, errPages);
}

bool ConfParser::fillLocations(std::map <std::string, std::string> &paramL, std::map <std::string, std::map<std::string, std::string> > &locations)
{
	std::map<std::string, std::string>::iterator	it;
	std::map<std::string, std::string>				part2;
	std::string										line;
	std::string										line2;

	for (it = paramL.begin(); it != paramL.end(); it++)
	{
		part2.clear();
		std::stringstream	sStream(it->second);
		
		while (std::getline(sStream, line))
		{
			ConfParser::splitLine(line, line2);
			if (line.empty() || line2.empty()) {
				Logger::putMsg(std::string("BAD CONFIG:\nline have no enough words:\n").append(line + line2), FILE_ERR, ERR);
				return (false);
			}
			if (line == "root" || line == "upload_path")
			{
				if (line2[0] != '/')
					line2.insert(line2.begin(), '/');
                if (line2[line2.length() - 1] == '/')
                    line2.erase(line2.length() - 1, 1);
			}
			else if (line != "acceptedMethods" && line != "dirListOn" && line != "defFileIfdir" && line != "CGIs" && line != "return" && line != "try_files") {
				Logger::putMsg("BAD PARAM:\n" + line, FILE_ERR, ERR);
				return (false);
			}
			if (part2.find(line) != part2.end())
			{
				Logger::putMsg("DOUBLE PARAM:\n" + line, FILE_ERR, ERR);
				return (false);
			}
			part2.insert(std::pair<std::string, std::string>(line, line2));
		}
		if (part2.empty()) {
			Logger::putMsg("BAD CONFIG OF LOCATION:\n" + it->first, FILE_ERR, ERR);
			return (false);
		}
		locations.insert(std::pair<std::string, std::map<std::string, std::string> >(it->first, part2));
	}
	if (locations.empty())
		return (false);
	return (true);
}

bool ConfParser::fillServParam(std::string &src, std::map <std::string, std::string> &paramS, std::vector <std::string> &ports, std::vector <std::string> &errPages)
{
	std::stringstream	sStream;
	std::string			line;
	std::string			line2;

	sStream << src;
	while (std::getline(sStream, line))
	{
		ConfParser::splitLine(line, line2);
		if (line.empty() || line2.empty()) {
			Logger::putMsg(std::string("BAD CONFIG:\nline have no enough words:\n").append(line + line2), FILE_ERR, ERR);
			return (false);
		}
		if (line == "listen" && ConfParser::checkPort(line2, ports))
			ports.push_back(line2);
		else if (line == "error_page")
			errPages.push_back(line2);
		else if (line == "host" || line == "server_name" || line == "limitBodySize" || line == "root")
		{
			if (line == "root")
            {
                if (line2[0] != '/')
				    line2.insert(line2.begin(), '/');
                if (line2[line2.length() - 1] != '/')
                    line2.erase(line2.length() - 1, 1);
            }
			if (paramS.find(line) != paramS.end())
			{
				Logger::putMsg("DOUBLE PARAMS: " + line, FILE_ERR, ERR);
				return (false);
			}
			if (line == "host" && !checkCorrectHost(line2))
			{
				Logger::putMsg("BAD PARAMS: " + line, FILE_ERR, ERR);
				return (false);
			}
			paramS.insert(std::pair<std::string, std::string>(line, line2));
		}
		else
		{
			Logger::putMsg("BAD CONFIG:\n" + line, FILE_ERR, ERR);
			return (false);
		}
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
	delSpaces(src);
	delSpaces(dst);
}

bool ConfParser::getLocations(std::string &src, std::map<std::string, std::string> &dst)
{
	std::string::size_type  i;
	std::string::size_type  fromErase;
	std::string::size_type  from;
	std::string			 part1;
	std::string			 part2;

	i = src.find("location");
	while (i != std::string::npos && !(i == 0 || (std::isspace(src[i - 1]) && std::isspace(src[i + 8]))))
		i = src.find("location", i + 8);
	if (i == std::string::npos)
		return (false);
	fromErase = i;
	i += 8;
	ConfParser::skipSpaces(src, i);
	from = i;
	while (i < src.length() && src[i] != '{')
		i++;
	if (i == src.length())
	{
		Logger::putMsg("BAD CONFIG:\nHAVE NO { after 'location'.\n" + src.substr(fromErase, i - fromErase), FILE_ERR, ERR);
		throw badConfig();
	}
	part1 = src.substr(from, i - from);
	delSpaces(part1);
    if (part1.empty())
    {
        Logger::putMsg("BAD CONFIG:\nlocation is empty:\n" + src.substr(fromErase, from - fromErase), FILE_ERR, ERR);
		throw badConfig();
    }
	if (part1[0] != '/')
        part1.insert(part1.begin(), '/');
    if (part1.length() > 1 && part1[part1.length() - 1] == '/')
        part1.erase(part1.length() - 1, 1);
	from = i + 1;
	i = ConfParser::findCloseBracket(src, i, 1);
	part2 = src.substr(from, i - from);
	delSpaces(part2);
	if (part2.empty())
	{
		Logger::putMsg("BAD CONFIG:\nlocation config is empty:\n" + src.substr(fromErase, i + 1 - fromErase), FILE_ERR, ERR);
		throw badConfig();
	}
	else if (dst.find(part1) != dst.end())
	{
		Logger::putMsg("BAD CONFIG:\nDOUBLE LOCATION:\n" + part1, FILE_ERR, ERR);
		throw badConfig();
	}
	else
		dst.insert(std::pair<std::string, std::string>(part1, part2));
	src.erase(fromErase, i - fromErase + 1);
	return (true);
}

std::string::size_type ConfParser::findCloseBracket(std::string &src, std::string::size_type i, int cnt)
{
	while (cnt != 0 && ++i < src.length())
	{
		if (src[i] == '{')
			cnt++;
		else if (src[i] == '}')
			cnt--;
	}
	if (i == src.length())
	{
		Logger::putMsg("BAD CONFIG:\nBAD BRACKETS: {}", FILE_ERR, ERR);
		throw badConfig();
	}
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

bool ConfParser::checkPort(std::string Port, std::vector<std::string> &ports)
{
	std::string::size_type  i;

	if (Port.length() > 5)
		return (false);
	if (std::find(ports.begin(), ports.end(), Port) != ports.end())
	{
		Logger::putMsg("BAD CONFIG:\nUse port " + Port + " multiple times", FILE_ERR, ERR);
		throw badConfig();
	}
	for (i = 0; i < Port.length(); i++)
	{
		if (!std::isdigit(Port[i]))
			return (false);
	}
	while (Port.length() < 5)
		Port.insert(Port.begin(), '0');
	if (Port > "65535")
		return (false);
	return (true);
}