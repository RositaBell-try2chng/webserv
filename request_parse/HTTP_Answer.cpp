#include "HTTP_Answer.hpp"

std::string	HTTP_Answer::ft_set_rp(std::string status_code) {

	if (!status_code.compare("200"))
		return ("OK");
	else
		return ("NE OK");
}

// int ft_get_answ(HTTP_Request req, HTTP_Answer *answ) {

// }

// int ft_post_answ(HTTP_Request req, HTTP_Answer *answ) {

// }

// int ft_delete_answ(HTTP_Request req, HTTP_Answer *answ) {


// }

std::string HTTP_Answer::ft_answtostr(HTTP_Answer answ) {

	std::string answer_str;
	std::string headers = ft_hdrstostr(answ.headers);


	answer_str = answ.version + " " + answ.status_code + " " + answ.reason_phrase +
			"\r\n" + headers +
			"\r\n" + answ.body;

	return (answer_str);
}

std::string HTTP_Answer::ft_reqtoansw(HTTP_Request req, HTTP_Answer *answ) {

	answ->version = req.version;
	answ->status_code = req.answ_code;

	// if (!req.method.compare("GET")) 
	// 	ft_get_answ(req, answ);
	// else if (!req.method.compare("POST"))
	// 	ft_post_answ(req, answ);
	// else if (!req.method.compare("DELETE"))
	// 	ft_delete_answ(req, answ);
	// else {
	// 	Logger::putMsg("Method: " + req.method + "\nIs Not Implemented" , FILE_WREQ, WREQ);
	// 	answ->status_code = "501";
	// }

	answ->reason_phrase = ft_set_rp(answ->status_code);

	return (ft_answtostr(*answ));
};