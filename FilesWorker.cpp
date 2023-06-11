#include "FilesWorker.hpp"

FilesWorker::FilesWorker() 
{
	this->fd = -1;
	this->Stage = 0;
}

FilesWorker::~FilesWorker()
{
	if (this-fd != -1)
		close(this->fd);
}

int FilesWorker::getFd() { return this->fd; }
int FilesWorker::getStage() { return this->Stage; }

void FilesWorker::setStage(int n) { this->Stage = n; }

void openFile(Server &thisServer, std::string path)
{
	
}

void FilesWorker::closeFile()
{
	close(this->fd);
	this->fd = -1;
	this->stage = 0;
}