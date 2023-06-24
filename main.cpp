#include "Server.hpp"
#include "Logger.hpp"
#include "Exceptions.hpp"
#include "webserv.hpp"
#include "ConfParser.hpp"
#include "MainClass.hpp"
#include "Servers.hpp"
#include "./request_parse/HTTP_Request.hpp"
#include "./request_parse/HTTP_Answer.hpp"
#include "./request_parse/HTTP_Headers.hpp"
#include "./request_parse/HTTP_Methods.hpp"
#include "./dir_listing/DirListing.hpp"


 int main(int argc, char **argv)					// MAIN main
 {
 	signal(SIGTERM, MainClass::exitHandler);
 	Logger::putMsg(DELIMITER_START);
 	MainClass::doIt(argc, argv);
 	Logger::putMsg(DELIMITER_END);
 	MainClass::exitHandler(0);

 	return 0;
 }

//int main(void)										// Directories listing Tests
//{
//	ft_dir_out(".");
//
//	return 0;
//}0

// int main( void )									// Parser Tests
// {

// 	std::string raw =	"GET / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nHost: 127.0.0.1:8081\r\nConnection: keep-alive\r\nsec-ch-ua: \"Chromium\";v=\"110\", \"Not A(Brand\";v=\"24\", \"Google Chrome\";v=\"110\"\r\nsec-ch-ua-mobile: ?0\r\nsec-ch-ua-platform: \"macOS\"\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: en-US,en;q=0.9\r\nCookie: wp-settings-1=mfold%3Df; wp-settings-time-1=1682074510\r\n\r\n7\r\nMozilla\r\n9\r\nDeveloper\r\n1\r\nO\r\n0\r\n\r\n";
// 	//std::string raw1 =	"1\r\nO\r\n0\r\n\r\n";
// 	// std::string raw2 =	"222";

// 	std::cout << raw << "\n\n=================================================================\n=================================================================\n\n";

// 	HTTP_Request req = HTTP_Request();
// 	HTTP_Request::ft_strtoreq(req, raw);
// 	//HTTP_Request::ft_strtoreq(req, raw1);
// 	// HTTP_Request::ft_strtoreq(req, raw2);

// // ft_strtoreq TESTS
// // ============================================================================================================================================
// 	std::cout << "=================================================================\n" << std::endl;

// 	std::cout << "START STRING\n" << std::endl;

// 	std::cout << "	method name:	" << req.base.start_string.method << std::endl;
// 	std::cout << "	URI:		" << req.base.start_string.uri << std::endl;
// 	std::cout << "	URI_param:	" << req.base.start_string.prmtrs << std::endl;
// 	std::cout << "	version:	" << req.base.start_string.version << "\n" << std::endl;

// 	std::cout << "START STRING END\n" << std::endl;

// 	std::cout << "=================================================================\n" << std::endl;

// 	std::cout << "HEADERS\n" << std::endl;

// 	for (std::map<std::string, std::string>::iterator it = req.base.headers.begin(); it != req.base.headers.end(); it++)
//        std::cout << "	" << (*it).first << ": " << (*it).second << std::endl;

// 	std::cout << "\nHEADERS END\n" << std::endl;

// 	std::cout << "=================================================================\n" << std::endl;

// 	std::cout << "PARSED HEADERS\n" << std::endl;

// 	std::cout << "	Left:			" << req.left << "\n" << std::endl;
// 	std::cout << "	Stage:			" << req.stage << "\n" << std::endl;
// 	std::cout << "	Host:			" << req.host << std::endl;
// 	std::cout << "	Port:			" << req.port << std::endl;
// 	std::cout << "	Connection:		" << req.flg_cnnctn << std::endl;
// 	std::cout << std::boolalpha << "	Content Type:		" << req.content_type.media_type << std::endl;
// 	std::cout << "	Date:			" << req.date.txt_day_of_week << " " << req.date.day << " " << req.date.txt_month << " " << req.date.year << " " << req.date.sec << " " << req.date.min << " " << req.date.hrs << std::endl;
// 	std::cout << "	Transfer encoding:	" << req.flg_te << std::endl;
// 	std::cout << "	Content-Length:		" << req.content_lngth << std::endl;
// 	std::cout << "\n	raw:			" << raw << "\n" << std::endl;

// 	std::cout << "\nPARSED HEADERS END\n" << std::endl;

// 	std::cout << "=================================================================\n" << std::endl;

// 	std::cout << "BODY:		" << req.body << std::endl;

// 	//std::cout << raw << "\n\n=================================================================\n=================================================================\n\n";

// 	std::cout << "Request code: " << "\n" << req.answ_code[0] * 100 + req.answ_code[1] << std::endl;
// // ============================================================================================================================================

// 	HTTP_Answer str_answ;

// 	str_answ = HTTP_Answer::ft_reqtoansw(req);

// // ft_reqtoansw TESTS
// // ============================================================================================================================================
// 	//std::cout << "Answer: " << "\n" << str_answ << std::endl;

// // ============================================================================================================================================
// }
