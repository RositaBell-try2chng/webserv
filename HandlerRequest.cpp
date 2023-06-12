#include "HandlerRequest.hpp"

void HandlerRequest::mainHandler(Server &srv, int Stage)
{
	switch (Stage)
	{
		// 4 - после считки запроса что-то осталось в string
		case 4: {//send to parsing
				break;
		}
		case 20:
		{
			CGI *newCGI = new CGI();

		}
		//20 - первичная работа с CGI(создание PIPES / CGI)
		//21 - форк + запуск скрипта
		//22 - ожидаем ответ скрипта(+ таймер на досрочный выход)
		//23 - читаем ответ из PipeInBack
		//24 - отправляем запрос в PipeOutForward (меняем когда отправим последний chunk)
		//25 - CGI chunked response (из пайпа считано не все) первичка
		//26 - CGI chunked response очередной
		//27 - ответ считан полностью после чанка
		//28 - ответ считан полностью
		//29 - CGI failed
		//50 - start
		//51 - start string was fully parsed
		//52 - headers was fully parsed
		//53 - body was fully parsed
		//59 - error
		// 99 - runtime error
	}
}

HandlerRequest::HandlerRequest() {}
HandlerRequest::~HandlerRequest() {}