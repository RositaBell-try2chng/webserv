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
    static bool             flgCreate;

    static void             setServNames(std::string &src, std::vector<std::string> &Names);
	int		 				checkExistsHostPort(std::string const &H, std::string const &P);
	bool					addToConnection(int fd, std::map<std::string, std::string> &S, std::map <std::string, std::map<std::string, std::string> > &L, std::vector<std::string>
	        &SN, std::vector<std::string> &E);
public:
    Servers();
    ~Servers();

    std::map<int, Server *>&    getConnections(bool lstFlg = false);
    Server&                     getServer(int fd);

    void                    addConnection(int fd, Server const &src, bool lstFlg = false);
    void                    removeConnection(int fd, bool lstFlg = false);
    void                    createServer(std::string const &host, std::string const &port, std::map<std::string, std::string> &S, std::map <std::string, std::map<std::string, std::string> > &L, std::vector<std::string> &SN, std::vector<std::string> &E);
    bool                    addServers(std::map<std::string, std::string> &S, std::map <std::string, std::map<std::string, std::string> > &L, std::vector<std::string> &P, std::vector<std::string> &E);
};

#endif