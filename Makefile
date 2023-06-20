R_PARS		= ./request_parse/

HTTP		=	${R_PARS}HTTP_Request_new.cpp	${R_PARS}HTTP_Answer.cpp	${R_PARS}HTTP_Headers.cpp \
				${R_PARS}HTTP_Methods.cpp		${R_PARS}HTTP_Utils.cpp		${R_PARS}Specific_Codes.cpp

MAIN		= Server.cpp Exceptions.cpp ConfParser.cpp MainClass.cpp Servers.cpp Logger.cpp CGI.cpp HandlerRequest.cpp

SRCS		= main.cpp utils.cpp ${MAIN} ${HTTP}

OBJS		= ${SRCS:.cpp=.o}

HEADER		= webserv.hpp

NAME		= webserv

CC			= c++

RM			= rm -f

CFLAGS		= -Wall -Wextra -Werror -std=c++98

all:		${NAME}

%.o:	%.cpp ${HEADER}
		${CC} ${CFLAGS} -c $< -o $@

${NAME}:	${OBJS}
			${CC} ${CFLAGS} ${OBJS} -o ${NAME}
			@mkdir -p logs

clean:
			${RM} ${OBJS}

fclean:		clean
			${RM} ${NAME}

re:			fclean all

log_clean:
			${RM} logs/errors.txt logs/info.txt logs/requests.txt logs/wrong_requests.txt

test:	all
			./${NAME} | cat -e

stop:
			pkill webserv

.PHONY:		all clean fclean re run log_clean test
