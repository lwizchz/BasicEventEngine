NAME = BasicEventEngine

VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_RELEASE = 1

DEPS = main
DEPS_BEE = debug resource_structures util game
DEPS_BEE_GAME = resources room transition display window input draw network
DEPS_BEE_RESOURCE_STRUCTURES = sprite sound background font path timeline object room ext/instancedata ext/collisiontree ext/particle ext/soundgroup
DEPS_BEE_UTIL = real string dates collision sound messagebox files network

RES_OBJS = bee
RES_ROOMS = test

# Generate a unique BEE_GAME_ID
BGI_FILES := ${shell find resources/ -type f -exec grep -Iq . {} \; -and -print} # Find all non-binary resources file
BGI_SUM := ${shell cat $(BGI_FILES) | sha256sum | head -c 8} # Concatenate the files and find the first 8 bytes of their sha256 hash
BEE_GAME_ID := ${shell perl -le 'print hex("$(BGI_SUM)");'} # Convert the 8 bytes into decimal

PKGDEPS = libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libsdl2-net-dev libsdl2-gfx-dev libglew-dev libglm-dev catch
INCLUDE = sdl2 SDL2_image SDL2_ttf SDL2_mixer SDL2_net
LIBS = -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_net -lSDL2 -lGLEW -lGLU -lGL
WARNS = -Wall -Wextra -Wpedantic -Wshadow -Wno-unused-parameter -Wno-unused-variable

DEFINES = -DBEE_GAME_ID=$(BEE_GAME_ID) -DBEE_VERSION_MAJOR=$(VERSION_MAJOR) -DBEE_VERSION_MINOR=$(VERSION_MINOR) -DBEE_VERSION_RELEASE=$(VERSION_RELEASE)
FLAGS = -g -pg -pipe -std=c++11 -flto -Wall -Wextra -Wpedantic -Wshadow -Wno-unused-parameter -Wno-unused-variable -D_REENTRANT -I/usr/include/SDL2 $(DEFINES)

CXX = g++
BUILD = $(CXX) $(FLAGS) -o $(NAME)

CLEAN = rm $(NAME) gmon.out

main: $(foreach var, $(DEPS_BEE), obj/bee/$(var).o) $(foreach var, $(DEPS_BEE_RESOURCE_STRUCTURES), obj/bee/resource_structures/$(var).o) $(foreach var, $(DEPS), obj/$(var).o)
	$(BUILD) $^ $(LIBS)

obj/bee/%.o : bee/%.cpp bee/game.hpp
	$(CXX) $(FLAGS) -c $< -o $@ $(LIBS)
obj/bee/resource_structures/%.o : bee/resource_structures/%.cpp bee/resource_structures/%.hpp bee/game.hpp
	$(CXX) $(FLAGS) -c $< -o $@ $(LIBS)
obj/%.o : %.cpp bee/game.hpp
	$(CXX) $(FLAGS) -c $< -o $@ $(LIBS)
bee/game.hpp: dir obj/bee/game.o obj/bee/util.o resources
obj/bee/game.o: bee/game.cpp $(foreach var, $(DEPS_BEE_GAME), bee/game/$(var).cpp)
obj/bee/util.o: bee/util.cpp bee/util.hpp $(foreach var, $(DEPS_BEE_UTIL), bee/util/$(var).hpp)
	$(CXX) $(FLAGS) -c bee/util.cpp -o obj/bee/util.o $(LIBS)
dir:
	mkdir -p obj/bee/resource_structures/ext
resources: resources/resources.hpp $(foreach var, $(RES_OBJS), resources/objects/$(var).hpp) $(foreach var, $(RES_ROOMS), resources/rooms/$(var).hpp)
	$(CXX) $(FLAGS) -c main.cpp -o obj/main.o $(LIBS)

run: main
	./$(NAME)
debug: main
	gdb ./$(NAME)
clean:
	rm -r obj
	$(CLEAN)
