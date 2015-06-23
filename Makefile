NAME = EasyGameEngine

LIBS = -lalut -lopenal -lGLEW -lGLU -lGL -lglut
FLAGS = -g -pg -std=c++11 -Wall -Wextra -I/usr/include/AL
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
