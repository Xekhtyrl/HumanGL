NAME = HumanGL

INC = ./Includes
HOME_LIB  = $(HOME)/.local/lib
HOME_INC  = $(HOME)/.local/include
DIR_OBJ = Obj/

IMGUI_DIR = $(INC)/imgui

SRCS =	main.cpp \
		Controls.cpp \
		utils.cpp \
		Shader.cpp \
		stb_image.cpp \
		window.cpp \
		modelMatrices.cpp \
		HierarchicModel.cpp \
		ModelLoadObj.cpp \
		Mesh.cpp \
		rotateNode.cpp \
		LoadAnimation.cpp \
		Animation.cpp \
		$(IMGUI_SRCS)
SRCC = glad.c

OBJ = $(addprefix $(DIR_OBJ), $(SRCS:.cpp=.o))
OBJ += $(addprefix $(DIR_OBJ), $(SRCC:.c=.o))

CXX       := g++
CC        := gcc

CXXFLAGS  = -std=c++20 -Wall -Wextra -Werror -g
CFLAGS    = -Wall -Wextra -Werror -g

INCLUDES  := -I$(INC) \
			 -I$(INC)/imgui \
             -I$(INC)/glad/include \
             -I$(HOME_INC)

LIBS      := -L$(HOME_LIB) \
			 $(IMGUI_DIR)/imgui.a \
             -Wl,-rpath,$(HOME_LIB) \
			 -L$(INC)/glfw-3.4/build/src \
             -lglfw3 -ldl -lGL -lpthread -lX11

all: $(NAME)

$(NAME): $(OBJ)
	make openGL
	$(CXX) $(CXXFLAGS) $(OBJ) $(LIBS) -o $@

# Compile .cpp source files
$(DIR_OBJ)%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile .c source files
$(DIR_OBJ)%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

ifeq ($(wildcard Includes/glfw-3.4/build),)
openGL:
	$(info Creating build folder)
	cd $(INC)/glfw-3.4 && \
	mkdir -p build && \
	cd build && \
	cmake .. -DGLFW_BUILD_WAYLAND=OFF -DGLFW_BUILD_X11=ON -DCMAKE_INSTALL_PREFIX=$(HOME)/.local && \
	make -j && \
	make install
else
openGL:
	$(info build folder already exists)
endif

clean:
	rm -rf $(DIR_OBJ)

fclean: clean
	rm -f $(NAME)
	rm -f imgui.ini
	rm -f err.log

cclean: fclean closeGL

closeGL:
	rm -rf $(INC)/glfw-3.4/build

rebuild: closeGL
	make openGL

re: fclean all

.PHONY: all openGL clean fclean cclean closeGL rebuild re