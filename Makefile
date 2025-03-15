# use make to build the project
# make sure you have sfml installed!

all:
	@g++ main.cpp -o flappy -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
