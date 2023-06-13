R_PARS		= ./request_parse/

HTTP		= ${R_PARS}HTTP_Request.cpp ${R_PARS}HTTP_Answer.cpp ${R_PARS}HTTP_Utils.cpp ${R_PARS}Specific_Codes.cpp

MAIN		= Server.cpp Exceptions.cpp Logger.cpp ConfParser.cpp MainClass.cpp Servers.cpp

SRCS		= main.cpp ${HTTP}

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

clean:
			${RM} ${OBJS}

fclean:		clean
			${RM} ${NAME}

re:			fclean all

log_clean:
			${RM} logs/errors.txt logs/info.txt logs/requests.txt logs/wrong_requests.txt

stop:
			pkill webserv

.PHONY:		all clean fclean re run log_clean
