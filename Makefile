LIBS := -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

all:
	@g++ main.cpp -o main $(LIBS)

run:
	@g++ main.cpp -o main $(LIBS)
	@./main

clean:
	@rm main