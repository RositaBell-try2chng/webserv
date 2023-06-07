void	t_get_answ(HTTP_Request &req, HTTP_Answer &answ) {

	std::FILE	file;

	// checking if recourse exists
	file = std::fopen(req.uri)
	if (file) {

	}
	else {
		Logger::putMsg("Page not found: 404", FILE_WREQ, WREQ);
		req->answ_code[0] = 4;
		req->answ_code[1] = 4;
		return ;
	}

}

// void ft_post_answ(HTTP_Request req, HTTP_Answer *answ) {

// }

// void ft_delete_answ(HTTP_Request req, HTTP_Answer *answ) {


// }
