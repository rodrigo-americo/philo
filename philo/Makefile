# Nome do programa final
NAME        = philo

# Arquivos fonte C (.c)
SRCS        = ft_clear.c ft_init.c ft_simulation.c ft_utils.c libft.c main.c \
              routine.c philo_actions.c ft_philo_eat.c
# Arquivos objeto (.o)
OBJS        = $(SRCS:.c=.o)

# Compilador e flags
CC          = gcc
CFLAGS      = -Wall -Wextra -Werror -pthread -I. -g
LDFLAGS     = -pthread 

RM          = rm -f

# ----------------- REGRAS -----------------
all: $(NAME)

$(NAME): $(OBJS)
	@echo "Linking $(NAME)..."
	@$(CC) $(OBJS) $(LDFLAGS) -o $(NAME)

%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpeza dos arquivos objeto
clean:
	@echo "Removing object files..."
	@$(RM) $(OBJS)

# Regra para limpeza total (objetos + executável)
fclean: clean
	@echo "Removing executable $(NAME)..."
	@$(RM) $(NAME)

# Regra para recompilar o projeto
re: fclean all

.PHONY: all clean fclean re
