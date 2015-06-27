NAME = EasyGameEngine

INCLUDE = assimp freealut glew freeglut Magick++
LIBS = ${shell pkg-config $(INCLUDE) --libs}
FLAGS = -g -pg -std=c++11 -Wall -Wextra ${shell pkg-config $(INCLUDE) --cflags}
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
