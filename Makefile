MAKEFLAGS = -j

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++23
LDFLAGS = -std=c++23

NAME = avm
OBJS = avm.o ioperand.o

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean .WAIT all

.PHONY: all clean fclean re
