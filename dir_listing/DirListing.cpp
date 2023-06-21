//	opendir()		https://www.opennet.ru/man.shtml?topic=opendir&russian=0&category=&submit=%F0%CF%CB%C1%DA%C1%D4%D8+man
//	readdir()		https://www.opennet.ru/man.shtml?topic=readdir&category=3&russian=0
//	closedir()		https://www.opennet.ru/man.shtml?topic=closedir&russian=0&category=&submit=%F0%CF%CB%C1%DA%C1%D4%D8+man
//	struct dirent	https://www.opennet.ru/man.shtml?topic=dirent&russian=0&category=&submit=%F0%CF%CB%C1%DA%C1%D4%D8+man
//	stat()			https://www.opennet.ru/man.shtml?topic=stat&category=2&russian=0

#include "DirListing.hpp"

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
	struct stat	info;
	
	dir = opendir(dir_nm);

	for (current = readdir(dir); current; current = readdir(dir)) {
		std::cout << current->d_ino << "	" << current->d_name << '\n' << std::endl;

		if (stat(current->d_name, &info) == -1) {
			Logger::putMsg("Can't do \"stat()\"", FILE_ERR, ERR);
			return 1;
		}
		else {
			std::cout << "st_atimespec.tv_nsec:		" << info.st_atimespec.tv_nsec << std::endl;
			std::cout << "st_atimespec.tv_sec:		" << info.st_atimespec.tv_sec << std::endl;
			std::cout << "st_birthtimespec.tv_nsec:	" << info.st_birthtimespec.tv_nsec << std::endl;
			std::cout << "st_birthtimespec.tv_sec:	" << info.st_birthtimespec.tv_sec << std::endl;
			std::cout << "st_blksize:			" << info.st_blksize << std::endl;
			std::cout << "st_blocks:			" << info.st_blocks << std::endl;
			std::cout << "st_ctimespec.tv_nsec:		" << info.st_ctimespec.tv_nsec << std::endl;
			std::cout << "st_ctimespec.tv_sec:		" << info.st_ctimespec.tv_sec << std::endl;
			std::cout << "st_dev:				" << info.st_dev << std::endl;
			std::cout << "st_flags:			" << info.st_flags << std::endl;
			std::cout << "st_gen:				" << info.st_gen << std::endl;
			std::cout << "st_gid:				" << info.st_gid << std::endl;
			std::cout << "st_ino:				" << info.st_ino << std::endl;
			std::cout << "st_lspare:			" << info.st_lspare << std::endl;
			std::cout << "st_mode:			" << info.st_mode << std::endl;
			std::cout << "st_mtimespec.tv_nsec:		" << info.st_mtimespec.tv_nsec << std::endl;
			std::cout << "st_mtimespec.tv_sec:		" << info.st_mtimespec.tv_sec << std::endl;
			std::cout << "st_nlink:			" << info.st_nlink << std::endl;
			std::cout << "st_qspare:			" << info.st_qspare << std::endl;
			std::cout << "st_rdev:			" << info.st_rdev << std::endl;
			std::cout << "st_size:			" << info.st_size << std::endl;
			std::cout << "st_uid:				" << info.st_uid << std::endl;
		}

		std::cout << "	Length of filename:	" << current->d_namlen << std::endl;
		std::cout << "	Length of record:	" << current->d_reclen << std::endl;
		std::cout << "	seekof:			" << current->d_seekoff << std::endl;
		std::cout << "	Type of file:		" << static_cast<size_t>(current->d_type) << std::endl;
		std::cout << "=======================================================" << std::endl;
	}
	
	closedir(dir);

	return 0;
}
