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

all: $(NAME)

$(NAME): $(OBJ)
	@ar rc $(NAME) $(OBJ)

# Compile .c source files
$(DIR_OBJ)%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(DIR_OBJ)

fclean: clean
	rm -f $(NAME)

cclean: fclean

re: fclean all

.PHONY: all clean fclean cclean re