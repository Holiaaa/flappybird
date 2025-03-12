# Install require libs before building!
# You need mingw to build the project!

LIBS := -lmingw32 -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

all:
	@g++ main.cpp -o flappy.exe $(LIBS)

run:
	@g++ main.cpp -o flappy.exe $(LIBS)
	@./main

clean:
	@rm flappy
