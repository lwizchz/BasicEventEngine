NAME = EasyGameEngine

FLAGS = -g -pg -Wall -Wextra
BUILD = g++ $(FLAGS) -o $(NAME) main.cpp

DEPS = main.cpp

main: $(DEPS)
	$(BUILD)
run: $(DEPS)
	$(BUILD)
	./$(NAME)
