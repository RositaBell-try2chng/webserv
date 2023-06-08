#include "HTTP_Methods.hpp"

void	t_get_answ(HTTP_Request &req, HTTP_Answer &answ) {

	FILE	*file;

	// checking if recourse exists
	file = std::fopen(req.uri.c_str(), "r");
	if (file) {
		std::ifstream in(req.uri);
		if (in.is_open()) {
			std::string line;
			while (std::getline(in, line))
				answ.body.append(line);
		}
	}
	else {
		Logger::putMsg("Page not found: 404", FILE_WREQ, WREQ);
		req.answ_code[0] = 4;
		req.answ_code[1] = 4;
		return ;
	}

}

// void ft_post_answ(HTTP_Request req, HTTP_Answer *answ) {

// }

// void ft_delete_answ(HTTP_Request req, HTTP_Answer *answ) {


// }
