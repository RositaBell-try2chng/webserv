#include "webserv.hpp"

void delSpaces(std::string &str)
{
    int i = 0;

    while (i <= static_cast<int>(str.length()) && std::isspace(str[i]))
        i++;
    str.erase(0, i);
    if (str.empty())
        return;
    i = static_cast<int>(str.length() - 1);
    while (i >= 0 && std::isspace(str[i]))
        i--;
    if (i != static_cast<int>(str.length() - 1))
        str.erase(i + 1, str.length());
}

bool checkCorrectHost(std::string &Host)
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

ssize_t strToSSize_t(std::string const &src, ssize_t limit)
{
    size_t              i;
    std::stringstream   ss(src);
    ssize_t             res;

    for (i = 0; i < src.length(); i++)
    {
        if (!std::isdigit(src[i]))
        {
            Logger::putMsg("BAD STR in strToSSize_t:\n" + src, FILE_ERR, ERR);
            return (-1);
        }
    }
    ss >> res;
    if (res > limit)
    {
        Logger::putMsg("BAD STR in strToSSize_t:\ntoo large value:\n" + src, FILE_ERR, ERR);
        return (-1);
    }
    return (res);
}

std::string Size_tToString(std::string::size_type src, std::string base)
{
    char c;

    if (src < base.length())
    {
        c = base[src];
        return (std::string(&c, 1));
    }
    c = base[src % base.length()];
    return(Size_tToString(src / base.length(), base) + std::string(&c, 1));
}

std::string::size_type StringToSize_t(std::string src, std::string base, bool &flgCorrect)
{
    std::string::size_type j;
    std::string::size_type res = 0;

    delSpaces(src);
    for (size_t i = 0; i < src.length(); i++)
    {
        j = base.find(src[i]);
        if (j == std::string::npos)
        {
            flgCorrect = false;
            return (0);
        }
        res *= base.length();
        res += j;
    }
    return (res);
}