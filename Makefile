R_PARS		= ./request_parse/

HTML		= ${R_PARS}HTTP_Request.cpp ${R_PARS}HTTP_Answer.cpp ${R_PARS}HTTP_Utils.cpp

SRCS		= main.cpp Server.cpp Exceptions.cpp Logger.cpp ${HTML}

OBJS		= ${SRCS:.cpp=.o}

HEADER		= webserv.hpp

NAME		= webserv

CC			= c++

RM			= rm -f

CFLAGS		= -Wall -Wextra -Werror -std=c++98 -fsanitize=address

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
