#include "Server.hpp"
#include "Logger.hpp"
#include "Exceptions.hpp"
#include "webserv.hpp"
#include "ConfParser.hpp"
#include "MainClass.hpp"
#include "Servers.hpp"
#include "./request_parse/HTTP_Request.hpp"
#include "./request_parse/HTTP_Answer.hpp"


int main(int argc, char **argv)
{
    signal(SIGTERM, MainClass::exitHandler);
    Logger::putMsg(DELIMITER_START);
	MainClass::doIt(argc, argv);
    Logger::putMsg(DELIMITER_END);
    MainClass::exitHandler(0);
}

//
//int main()
//{
//
//	std::string raw =	"GET / HTTP/1.1\r\nHost: 127.0.0.1:8081\r\nConnection: keep-alive\r\nsec-ch-ua: \"Chromium\";v=\"110\", \"Not A(Brand\";v=\"24\", \"Google Chrome\";v=\"110\"\r\nsec-ch-ua-mobile: ?0\r\nsec-ch-ua-platform: \"macOS\"\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: en-US,en;q=0.9\r\nCookie: wp-settings-1=mfold%3Df; wp-settings-time-1=1682074510\r\n";
//
//	std::cout << raw << "\n\n=================================================================\n=================================================================\n\n";
//
//	HTTP_Request req;
//	HTTP_Request::ft_strtoreq(raw, &req);
//
//// ft_strtoreq TESTS
//// ============================================================================================================================================
//	std::cout << "method name:	" << req.method << std::endl;
//	std::cout << "URI:		" << req.uri << std::endl;
//	std::cout << "version:	" << req.version << "\n" << std::endl;
//
//	std::cout << "HEADERS\n" << std::endl;
//	for (std::map<std::string, std::string>::iterator it = req.headers.begin(); it != req.headers.end(); it++)
//        std::cout << (*it).first << ": " << (*it).second << std::endl;
//
//
//
//	std::cout << "body:		" << req.body << std::endl;
//
//	std::cout << raw << "\n\n=================================================================\n=================================================================\n\n";
//
//	std::cout << "Request code: " << "\n" << req.answ_code << std::endl;
//// ============================================================================================================================================
//
//	std::string str_answ;
//	HTTP_Answer	answ;
//	str_answ = HTTP_Answer::ft_reqtoansw(req, &answ);
//
//// ft_reqtoansw TESTS
//// ============================================================================================================================================
//	std::cout << "Answer: " << "\n" << str_answ << std::endl;
//
//// ============================================================================================================================================
//}
