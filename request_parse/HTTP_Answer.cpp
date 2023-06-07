#include "HTTP_Answer.hpp"

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

HTTP_Answer HTTP_Answer::ft_reqtoansw(HTTP_Request req) {

	HTTP_Answer	answ;
	
	//version equal to request.version
	answ.version = req.version;

	if (req.answ_code[0] < 4) {
		if (!req.method.compare("GET")) 
			ft_get_answ(req, &answ);
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

	ft_pars_status_code(req.answ_code, &(answ.status_code));
	answ.reason_phrase = ::ft_reason_phrase(req.answ_code);

	return (answ);
};
