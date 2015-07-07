NAME = EasyGameEngine

PKGDEPS = libalut-dev libmagick++-dev freeglut3-dev libglew-dev libassimp-dev sdl2
INCLUDE = assimp freealut glew Magick++ sdl2
LIBS = ${shell pkg-config $(INCLUDE) --libs} -lglut
WARNS = -Wall -Wextra -Wno-unused-parameter
FLAGS = -g -pg -std=c++11 $(WARNS) ${shell pkg-config $(INCLUDE) --cflags}
BUILD = g++ $(FLAGS) -o $(NAME) main.cpp $(LIBS)
CLEAN = rm gmon.out

DEPS = main.cpp

main: $(DEPS)
	$(BUILD)
run: $(DEPS)
	$(BUILD)
	./$(NAME)
clean:
	$(CLEAN)
