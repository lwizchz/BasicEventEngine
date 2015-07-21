NAME = BasicEventEngine

DEPS = main.cpp
REQ_FILES = $(NAME)/

PKGDEPS = libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
INCLUDE = sdl2 SDL2_image SDL2_ttf SDL2_mixer
LIBS = ${shell pkg-config $(INCLUDE) --libs}
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
