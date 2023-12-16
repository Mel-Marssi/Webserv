NAME=webserve
CXX=c++
CXXFLAGS= -Wall -Wextra -Werror -std=c++98 #-fsanitize=address -g3
FILE=pars_config/server_config.cpp workshop.cpp \
pars_config/location.cpp pars_config/servers.cpp
OBJ=$(FILE:.cpp=.o)
INCLUDE=pars_config/server_config.hpp pars_config/location.hpp \
pars_config/servers.hpp

all: $(NAME)
$(NAME):$(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
$(OBJ):$(INCLUDE)

clean:
	rm -rf $(OBJ)
fclean:clean
	rm -rf $(NAME)
re:fclean all