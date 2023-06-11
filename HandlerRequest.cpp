#include "HandlerRequest.hpp"

HandlerRequest::prepareToSend(std::map<int, Server*>::iterator it, int Stage)
{
	switch (Stage)
	{
		case 25: //first chunk from pipe was read -> send
		{
			//make headers
			//make body
			
		}
        case 26: //next chunk from pipe was read -> send
        case 27: //ответ считан полностью после чанка -> send
        case 28: //ответ считан полностью -> send
		case 43: // файл считан полностью -> send
		case 44: // файл считан не полностью ->send
    	case 45: // файл считан не полностью ->send
    	case 46: // файл считан полностью //последний чанк -> send
	}
}

HandlerRequest::HandlerRequest() {}
HandlerRequest::~HandlerRequest() {}