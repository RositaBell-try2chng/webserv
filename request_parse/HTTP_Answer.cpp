#include "HTTP_Answer.hpp"
#include "Specific_Codes.hpp"

std::string	HTTP_Answer::ft_set_rp(int *status_code) {

	Status_Categories category;
	category = static_cast<Status_Categories>(status_code[0]);

	if (category == informational_responses)
		return (reason_phrase<Informational_responses>(status_code[1]));
	else if (category == successful_responses)
		return ("OK");
	else if (category == redirection_messages)
		return ("OK");
	else if (category == client_error_responses)
		return ("NE OK");
	else if (category == server_error_responses)
		return ("Not implemented");
	else if (category == security_events)
		return ("NE OK");
	else
		return ("Work on this, you need actual code (watch logs)");

	
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

void HTTP_Answer::ft_pars_status_code(int *integ_code, std::string *str_code) {

	// code category cast
	str_code->push_back(static_cast<char>(integ_code[0] + 48));

	// specific code cast
	if (integ_code[1] < 10)
		str_code->push_back('0');
	else
		str_code->push_back(static_cast<char>(integ_code[1] / 10 + 48));
	str_code->push_back(static_cast<char>(integ_code[1] % 10 + 48));
}

std::string HTTP_Answer::ft_reqtoansw(HTTP_Request req, HTTP_Answer *answ) {

	//version equal to request.version
	answ->version = req.version;

	if (req.answ_code[0] < 4) {
		if (!req.method.compare("GET")) 
			ft_get_answ(req, answ);
		// else if (!req.method.compare("POST"))
		// 	ft_post_answ(req, answ);
		// else if (!req.method.compare("DELETE"))
		// 	ft_delete_answ(req, answ);
		else {
			Logger::putMsg("Method: " + req.method + "\nIs Not Implemented" , FILE_WREQ, WREQ);
			req.answ_code[0] = 5;
			req.answ_code[1] = 1;
		}
	}

	ft_pars_status_code(req.answ_code, &(answ->status_code));
	answ->reason_phrase = ft_set_rp(req.answ_code);

	return (ft_answtostr(*answ));
};