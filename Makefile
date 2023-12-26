NAME=webserve
CXX=c++
CXXFLAGS= -Wall -Wextra -Werror -std=c++98  -fsanitize=address -g3
FILE=pars_config/server_config.cpp workshop.cpp \
pars_config/location.cpp pars_config/servers.cpp \
multiplexing/multiplexing.cpp multiplexing/post.cpp  multiplexing/request.cpp \
multiplexing/Get_request.cpp multiplexing/Get_response.cpp	\
multiplexing/Content_type.cpp	multiplexing/root_page.cpp	\

OBJ=$(FILE:.cpp=.o)
INCLUDE=pars_config/server_config.hpp pars_config/location.hpp \
pars_config/servers.hpp multiplexing/multiplexing.hpp \

all: $(NAME)
$(NAME):$(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
$(OBJ):$(INCLUDE)

clean:
	rm -rf $(OBJ) 
fclean:clean
	rm -rf $(NAME)
re:fclean all