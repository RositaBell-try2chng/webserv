//	opendir()		https://www.opennet.ru/man.shtml?topic=opendir&russian=0&category=&submit=%F0%CF%CB%C1%DA%C1%D4%D8+man
//	readdir()		https://www.opennet.ru/man.shtml?topic=readdir&category=3&russian=0
//	closedir()		https://www.opennet.ru/man.shtml?topic=closedir&russian=0&category=&submit=%F0%CF%CB%C1%DA%C1%D4%D8+man
//	struct dirent	https://www.opennet.ru/man.shtml?topic=dirent&russian=0&category=&submit=%F0%CF%CB%C1%DA%C1%D4%D8+man
//	stat()			https://www.opennet.ru/man.shtml?topic=stat&category=2&russian=0

#include "DirListing.hpp"

const char	*ft_make_name(const char *dir_nm, char *f_name) {

	std::string full = dir_nm;
	full.push_back('/');
	full += f_name;
	std::cout << full << std::endl;
	return full.c_str();
}

int ft_dir_out(std::string dir_nm_str) {

	const char *dir_nm = dir_nm_str.c_str();

	if (access(dir_nm, F_OK)) {
		Logger::putMsg("Dir \"" + dir_nm_str + "\" doesn't exist", FILE_ERR, ERR);
		return 1;
	}
	if (access(dir_nm, R_OK)) {
		Logger::putMsg("Have no permissions to read dir \"" + dir_nm_str + "\"", FILE_ERR, ERR);
		return 1;
	}

	DIR			*dir;
	dirent		*current;
	struct stat	*info = NULL;
	
	dir = opendir(dir_nm);

	for (current = readdir(dir); current; current = readdir(dir)) {
		std::cout << current->d_ino << "	" << current->d_name << std::endl;
		if (stat(ft_make_name(dir_nm, current->d_name), info) == -1) {
			Logger::putMsg("Can't do \"stat()\"", FILE_ERR, ERR);
			//return 1;
		}
		else {
			std::cout << info->st_atimespec.tv_nsec << std::endl;
		}
		std::cout << "	Length of filename:	" << current->d_namlen << std::endl;
		std::cout << "	Length of record:	" << current->d_reclen << std::endl;
		std::cout << "	seekof:		" << current->d_seekoff << std::endl;
		std::cout << "	Type of file:		" << static_cast<size_t>(current->d_type) << std::endl;
		std::cout << "=======================================================" << std::endl;
	}
	
	closedir(dir);
	return 0;
}