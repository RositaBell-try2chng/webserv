#ifndef FILESWORKER_HPP
# define FILESWORKER_HPP

# include "webserv.hpp"
# include "Server.hpp"

class FilesWorker
{
private:
	int fd;
	int Stage;	//0 - только создан / обнулен
				//1 - файл открыт(готов к чтению)
				//2 - считали все
				//3 - считали часть
				//9 - ошибка открытия / чтения
public:
	FilesWorker();
	~FilesWorker();

	int		getFd();
	int		getStage();
	void	setStage(int n);

	void	openFile(Server &thisServer, std::string path);
	void	closeFile();
}
#endif