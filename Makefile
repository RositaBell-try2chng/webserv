SRCS		= main.cpp Server.cpp

OBJS		= ${SRCS:.cpp=.o}

HEADER		= webserv.hpp

NAME		= webserv

CC			= c++

RM			= rm -f

CFLAGS		= -Wall -Wextra -Werror -std=c++98 -fsanitize=address

all:		${NAME}

%.opp:	%.cpp ${HEADER}
		${CC} ${CFLAGS} -c $< -o $@

${NAME}:	${OBJS}
			${CC} ${CFLAGS} ${OBJS} -o ${NAME}

clean:
			${RM} ${OBJS}

fclean:		clean
			${RM} ${NAME}

re:			fclean all

.PHONY:		all clean fclean re run
