NAME = BasicEventEngine

DEPS = main.cpp
REQ_FILES = $(NAME)/

PKGDEPS = libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
INCLUDE = sdl2 SDL2_image SDL2_ttf SDL2_mixer
LIBS = ${shell pkg-config --libs $(INCLUDE)}
WARNS = -Wall -Wextra -Wno-unused-parameter
FLAGS = -g -pg -std=c++11 $(WARNS) ${shell pkg-config --cflags $(INCLUDE)}

BUILD = g++ $(FLAGS) -o $(NAME) main.cpp $(LIBS)
CLEAN = rm $(NAME) gmon.out $(NAME).html $(NAME).js

WINLIBS = ${shell i686-w64-mingw32-pkg-config --libs $(INCLUDE)}
WINFLAGS = -g -pg -std=c++11 $(WARNS) ${shell i686-w64-mingw32-pkg-config --cflags $(INCLUDE)} -static-libstdc++
WINBUILD = i686-w64-mingw32-g++ $(WINFLAGS) -o $(NAME).exe main.cpp $(WINLIBS)

EMSCRIPTEN = /usr/lib/emscripten/emc++

main: $(DEPS)
	$(BUILD)
run: $(DEPS)
	$(BUILD)
	./$(NAME)
win: $(DEPS)
	$(WINBUILD)
clean:
	$(CLEAN)

web:
	cd ..
	$(EMSCRIPTEN) $(DEPS) -o $(NAME).html --preload-file $(REQ_FILES)
	cd $(NAME)
