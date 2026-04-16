NAME = imgui.a

SRCS = imgui.cpp \
		 imgui_widgets.cpp \
		 imgui_tables.cpp \
		 imgui_impl_opengl3.cpp \
		 imgui_impl_glfw.cpp \
		 imgui_draw.cpp

DIR_OBJ = Obj/
OBJ = $(addprefix $(DIR_OBJ), $(SRCS:.cpp=.o))

CC        := g++
CFLAGS    = -Wall -Wextra -Werror -g -std=c++20

HOME_INC  = $(HOME)/.local/include
HOME_LIB  = $(HOME)/.local/lib
INC	   = ../Includes

INCLUDES  := -I$(INC) \
			 -I$(INC)/imgui \
             -I$(INC)/glad/include \
             -I$(HOME_INC)

all: $(NAME)

$(NAME): $(OBJ)
	@ar rc $(NAME) $(OBJ)

# Compile .c source files
$(DIR_OBJ)%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


clean:
	rm -rf $(DIR_OBJ)

fclean: clean
	rm -f $(NAME)

cclean: fclean

re: fclean all

.PHONY: all clean fclean cclean re