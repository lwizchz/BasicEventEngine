NAME = EasyGameEngine

FLAGS = -g -pg -Wall -Wextra -std=c++11
BUILD = g++ $(FLAGS) -o $(NAME) main.cpp

DEPS = main.cpp

main: $(DEPS)
	$(BUILD)
run: $(DEPS)
	$(BUILD)
	./$(NAME)
