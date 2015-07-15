NAME = EasyGameEngine

DEPS = main.cpp
REQ_FILES = $(NAME)/

PKGDEPS = libalut-dev libmagick++-dev freeglut3-dev libglew-dev libassimp-dev libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
INCLUDE = assimp freealut glew Magick++ sdl2 SDL2_image SDL2_ttf
LIBS = ${shell pkg-config $(INCLUDE) --libs} -lglut
WARNS = -Wall -Wextra -Wno-unused-parameter
FLAGS = -g -pg -std=c++11 $(WARNS) ${shell pkg-config $(INCLUDE) --cflags}

BUILD = g++ $(FLAGS) -o $(NAME) main.cpp $(LIBS)
CLEAN = rm $(NAME) gmon.out $(NAME).html $(NAME).js

EMSCRIPTEN = /usr/lib/emscripten/emc++

main: $(DEPS)
	$(BUILD)
run: $(DEPS)
	$(BUILD)
	./$(NAME)
clean:
	$(CLEAN)

web:
	cd ..
	$(EMSCRIPTEN) $(DEPS) -o $(NAME).html --preload-file $(REQ_FILES)
	cd $(NAME)
