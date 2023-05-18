#ifndef SERVERS_HPP
# define SERVERS_HPP

# include "webserv.hpp"
# include "Server.hpp"
# include "Exceptions.hpp"
# include "Logger.hpp"

class Servers
{
private:
    std::map<int, Server *> lst;
    std::map<int, Server *> connections;
    std::set<int>           fds;
    static bool             flgCreate;
public:
    Servers();
    ~Servers();

    std::map<int, Server *>&    getConnections(bool lstFlg = false);
    std::set<int>&              getFds();
    Server&                     getServer(int fd);

    void                    addConnection(int fd, Server const &src, bool lstFlg = false);
    void                    removeConnection(int fd, bool lstFlg = false);
    void                    createServer(std::string const &host, std::string const &port);
};

#endif