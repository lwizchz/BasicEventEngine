NAME = BasicEventEngine

DEPS = main
DEPS_BEE = debug resource_structures util game
DEPS_BEE_GAME = resources room transition display window input draw
DEPS_BEE_RESOURCE_STRUCTURES = sprite sound background font path object room ext/instancedata ext/particles
DEPS_BEE_UTIL = real string dates collision sound messagebox files script

PKGDEPS = libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev chaiscript
INCLUDE = sdl2 SDL2_image SDL2_ttf SDL2_mixer
LIBS = ${shell pkg-config --libs $(INCLUDE)}
WARNS = -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable

FLAGS = -g -pg -pipe -std=c++11 $(WARNS) ${shell pkg-config --cflags $(INCLUDE)}
SCRIPTING = ${shell which chai 2>&1 > /dev/null; echo $$?}
ifeq ($(SCRIPTING),0)
LIBS +=-ldl
FLAGS +=-D_BEE_SCRIPTING -I/usr/include/chaiscript
endif

BUILD = g++ $(FLAGS) -o $(NAME) $(LIBS)

CLEAN = rm $(NAME) gmon.out $(NAME).html $(NAME).js

WINLIBS = ${shell i686-w64-mingw32-pkg-config --libs $(INCLUDE)}
WINFLAGS = -g -pg -std=c++11 $(WARNS) ${shell i686-w64-mingw32-pkg-config --cflags $(INCLUDE)} -static-libstdc++
WINBUILD = i686-w64-mingw32-g++ $(WINFLAGS) -o $(NAME).exe main.cpp $(WINLIBS)

EMSCRIPTEN = /usr/lib/emscripten/emc++

main: $(foreach var, $(DEPS_BEE), obj/bee/$(var).o) $(foreach var, $(DEPS_BEE_RESOURCE_STRUCTURES), obj/bee/resource_structures/$(var).o) $(foreach var, $(DEPS), obj/$(var).o)
	$(BUILD) $^

obj/bee/%.o : bee/%.cpp bee/game.hpp
	g++ $(FLAGS) -c $< -o $@
obj/bee/resource_structures/%.o : bee/resource_structures/%.cpp bee/game.hpp
	g++ $(FLAGS) -c $< -o $@
obj/%.o : %.cpp bee/game.hpp
	g++ $(FLAGS) -c $< -o $@
bee/game.hpp: obj/bee/util.o resources/resources.hpp $(foreach var, $(DEPS_BEE_GAME), bee/game/$(var).cpp) dir
obj/bee/util.o: bee/util.cpp bee/util.hpp $(foreach var, $(DEPS_BEE_UTIL), bee/util/$(var).hpp)
	g++ $(FLAGS) -c bee/util.cpp -o obj/bee/util.o
dir:
	mkdir -p obj/bee/resource_structures/ext
resources/resources.hpp: resources/objects/bee.hpp resources/rooms/test.hpp
	g++ $(FLAGS) -c main.cpp -o obj/main.o

run: main
	./$(NAME)
win: $(DEPS)
	$(WINBUILD)
clean:
	$(CLEAN)
	rm -r obj

web:
	cd ..
	$(EMSCRIPTEN) $(DEPS) -o $(NAME).html --preload-file $(NAME)/
	cd $(NAME)
