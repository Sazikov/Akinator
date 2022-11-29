FLAGS=-Wall -Wextra -Wpedantic
SRC = main.cpp src\tree.cpp akinator.cpp src\Onegin.cpp src\stack.cpp

all:
	g++  $(SRC) -o main