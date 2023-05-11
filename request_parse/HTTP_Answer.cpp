#include "HTTP_Answer.hpp"
#include "Status_Codes.hpp"

std::string	HTTP_Answer::ft_set_rp(std::string status_code) {

	if (status_code[0] == '1')
		return ("OK");
	else if (status_code[0] == '2')
		return ("OK");
	else if (status_code[0] == '3')
		return ("OK");
	else if (status_code[0] == '4')
		return ("NE OK");
	else if (status_code[0] == '5')
		return ("Not implemented");
	else if (status_code[0] == '6')
		return ("NE OK");
	else
		return ("Work on this (watch logs)");
}

void HTTP_Answer::ft_get_answ(HTTP_Request req, HTTP_Answer *answ) {

	answ->body = req.body;
}

// void ft_post_answ(HTTP_Request req, HTTP_Answer *answ) {

// }

// void ft_delete_answ(HTTP_Request req, HTTP_Answer *answ) {


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

	answ->status_code.push_back(static_cast<char>(req.answ_code[0] + 48));
	if (req.answ_code[1] < 10) {
		answ->status_code.push_back('0');
		answ->status_code.push_back(static_cast<char>(req.answ_code[1] + 48));
	}
	else {
		answ->status_code.push_back(static_cast<char>(req.answ_code[1] / 10 + 48));
		answ->status_code.push_back(static_cast<char>(req.answ_code[1] % 10 + 48));
	}
	
	if (!req.method.compare("GET")) 
		ft_get_answ(req, answ);
	// else if (!req.method.compare("POST"))
	// 	ft_post_answ(req, answ);
	// else if (!req.method.compare("DELETE"))
	// 	ft_delete_answ(req, answ);
	else {
		Logger::putMsg("Method: " + req.method + "\nIs Not Implemented" , FILE_WREQ, WREQ);
		answ->status_code = "501";
	}

	answ->reason_phrase = ft_set_rp(answ->status_code);

	return (ft_answtostr(*answ));
};