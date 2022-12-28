# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mfirdous <mfirdous@student.42abudhabi.a    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/12/22 14:10:32 by mfirdous          #+#    #+#              #
#    Updated: 2022/12/28 20:17:47 by mfirdous         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	philo

SRCS	=	srcs/main.c srcs/utils.c

OBJS	=	${SRCS:.c=.o}

HEADER	=	include

CC		=	gcc

# CFLAGS	=	 -Wall -Wextra -Werror -I ${HEADER} 
CFLAGS	=	 -Wall -Wextra -Werror -I ${HEADER} -Ofast -pthread

.c.o:
			${CC} ${CFLAGS} -c $< -o ${<:.c=.o} 

${NAME}:	${OBJS}
			${CC} ${CFLAGS} ${OBJS} -o ${NAME}

all:		${NAME}
#			mv ${NAME} srcs
clean:		
			rm -f ${OBJS}

fclean:		clean
			rm -f ${NAME} 

re:			fclean all

.PHONY:		all clean fclean re bonus