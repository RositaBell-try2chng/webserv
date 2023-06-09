#include "DirListing.hpp"

std::string	ft_name_cell(const char *name, size_t type) {

	std::string cell;

	cell = "<td>\n<a href=\"";

	cell += name;
	if (type == 4)
		cell += "/";

	cell += "\">";

	cell += name;
	if (type == 4)
		cell += "/";

	cell += "</a>\n</td>\n";

	return cell;
}

std::string ft_mtime(timespec mtime) {

	char time[100];

	strftime(time, sizeof time, "%c", localtime(&mtime.tv_sec));

	return time;	
}

std::string ft_mtime_cell(std::string time) {

	std::string cell;

	cell = "<td>\n";
	cell += time;
	cell += "\n</td>\n";

	return cell;
}

std::string ft_size_cell(long size, size_t type) {

	std::string cell;

	cell = "<td>\n";
	if (type == 8)
		cell += std::to_string(size);
	else
		cell += "-";
	cell += "\n</td>\n";

	return cell;
}

std::string ft_dirlisting(std::string path_str, Server &srv) {

	const char	*path = path_str.c_str();

	std::string	pathToPrint(srv.getReq_struct()->base.start_string.uri);
	if (access(path, F_OK)) {
		Logger::putMsg("Dir \"" + path_str + "\" doesn't exist", FILE_ERR, ERR);
		srv.Stage = 9;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 4;
		return "1";
	}
	if (access(path, R_OK)) {
		Logger::putMsg("Have no permissions to read dir \"" + path_str + "\"", FILE_ERR, ERR);
		srv.Stage = 9;
		srv.getReq_struct()->answ_code[0] = 4;
		srv.getReq_struct()->answ_code[1] = 3;
		return "1";
	}

	std::string page("<html>\n<head>\n<title> Index of /" + pathToPrint + "</title>\n</head>\n<body >\n<h1> Index of " + pathToPrint + "</h1>\n<table style=\"width:80%; font-size: 15px\">\n<hr>\n<th style=\"text-align:left\"> File Name </th>\n<th style=\"text-align:left\"> Last Modification </th>\n<th style=\"text-align:left\"> File Size </th>\n" );

	DIR			*dir;
	dirent		*current;
	struct stat	info;
	size_t		type;
	
	dir = opendir(path);
	current = readdir(dir);

	for (current = readdir(dir); current; current = readdir(dir)) {

		page += "<tr>\n";

		type = static_cast<size_t>(current->d_type);

		page += ft_name_cell(current->d_name, type);

		if (stat((path_str  + current->d_name).c_str(), &info) == -1) {
			Logger::putMsg("Can't do \"stat()\"", FILE_ERR, ERR);
			page += "</tr>\n";
			continue ;
		}
		page +=	ft_mtime_cell(ft_mtime(info.st_mtimespec));
		
		page += ft_size_cell(info.st_size, type);

		page += "</tr>\n";
	}

	closedir(dir);

	page += "</table>\n<hr>\n</body>\n</html>\n";

	return page;
}
