#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <random>
#include <time.h>
#include <stdlib.h>

// made by Téo JAUFFRET (Holiaaa)

class Pipe {
public:
    int posX = 0;
    int posY = 0;
};

void renderScore(SDL_Renderer *renderer, const char *scoreText, SDL_Rect scoreRect) {
    int fontSize = 48;
    TTF_Font *font = NULL;
    int textW = 0, textH = 0;

    while (fontSize > 6) {
        if (font) TTF_CloseFont(font);
        font = TTF_OpenFont("./assets/font/font.ttf", fontSize);
        if (!font) {
            SDL_Log("Font error: %s", TTF_GetError());
            return;
        }

        TTF_SizeText(font, scoreText, &textW, &textH);
        if (textW <= scoreRect.w && textH <= scoreRect.h) {
            break;
        }
        fontSize--;
    }

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, scoreText, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect destRect = {
        scoreRect.x + (scoreRect.w - textW) / 2,
        scoreRect.y + (scoreRect.h - textH) / 2,
        textW,
        textH
    };

    SDL_RenderCopy(renderer, textTexture, NULL, &destRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) > 0) {
        std::cout << "Error while loading sdl." << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cout << "Error while initializing SDL_ttf. error : " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        std::cout << "Error while initializing SDL_image. error : " << IMG_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    
    if (Mix_Init(MIX_INIT_OGG) == 0) {
        std::cout << "Error while initializing SDL_image. error : " << IMG_GetError() << std::endl;
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
        std::cout << "Error while initializing Mix_OpenAudio. error : " << Mix_GetError() << std::endl;
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Error while creating the window. error : " << SDL_GetError() << std::endl;
        Mix_Quit();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Error while creating the renderer. error : " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        Mix_Quit();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;

    const Uint8 *state = SDL_GetKeyboardState(NULL);

    int playerYpos = 100;
    float playerSpeed = 0;

    Pipe groundPipes[4];
    Pipe airPipes[4];

    SDL_Texture *backgroundTexture = IMG_LoadTexture(renderer, "./assets/background.png");
    if (!backgroundTexture) {
        std::cout << "Error while creating texture for background. error : " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_Quit();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
    }

    SDL_Texture *pipe0Texture = IMG_LoadTexture(renderer, "./assets/sprites/pipe0.png");
    if (!pipe0Texture) {
        std::cout << "Error while creating texture for pipe. error : " << IMG_GetError() << std::endl;
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_Quit();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
    }

    SDL_Texture *pipe1Texture = IMG_LoadTexture(renderer, "./assets/sprites/pipe1.png");
    if (!pipe1Texture) {
        std::cout << "Error while creating texture for pipe. error : " << IMG_GetError() << std::endl;
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(pipe0Texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_Quit();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
    }

    SDL_Texture *birdTexture = IMG_LoadTexture(renderer, "./assets/sprites/bird.png");
    if (!pipe1Texture) {
        std::cout << "Error while creating texture for bird. error : " << IMG_GetError() << std::endl;
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(pipe0Texture);
        SDL_DestroyTexture(pipe1Texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_Quit();
        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
    }

    Mix_Chunk* jumpsound = Mix_LoadWAV("./assets/sounds/jump.ogg");
    if (jumpsound == NULL) {
        printf("Mix_LoadMUS failed: %s\n", Mix_GetError());
        return -1;
    }

    Mix_AllocateChannels(16);

    int score = 0;
    char scoreBuffer[20];
    int scoreYpos = 50;
    sprintf(scoreBuffer, "%d", score);

    int gameoverYpos = -80;

    double angle = 0.0;
    double angleSpeed = 0;
    bool collideWithAPipe = false;

    int spacePressed = 0;

    SDL_Color white = {255, 255, 255};

    for (int i = 0; i < 4; i++) {
        groundPipes[i].posX = 300*i + 800;
        groundPipes[i].posY = 320 + (std::rand() % 150);
    }

    for (int i = 0; i < 4; i++) {
        airPipes[i].posX = groundPipes[i].posX;
        airPipes[i].posY = groundPipes[i].posY - 480;
    }
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);

        // loop start here

        SDL_Rect player = {100, playerYpos, 50, 40};

        playerYpos += playerSpeed;
        playerSpeed += 0.175;

        if (state[SDL_SCANCODE_SPACE]) {
            if (!spacePressed && !collideWithAPipe) {
                playerSpeed = -3.6;
                angleSpeed = 1.5;
                angle = -(player.x / 4) * 128;
                if (Mix_PlayChannel(-1, jumpsound, 0) == -1) {
                    printf("Mix_PlayChannel failed: %s\n", Mix_GetError());
                    return -1;
                }
        
                spacePressed = 1;
            }
        } else {
            spacePressed = 0;
        }

        if (player.y > 600) {
            collideWithAPipe = true;
        }

        if (angle > -60 && angle < 60) {
            angle += angleSpeed;
            angleSpeed += 0.015;
        } else {
            if (angle >= 60) {
                angle = 60;
            }
            else if (angle <= -60) {
                angle = -59;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_Point center = {player.w /2, player.h/2};
        SDL_RenderCopyEx(renderer, birdTexture, NULL, &player, angle, &center, SDL_FLIP_NONE);
        //SDL_RenderFillRect(renderer, &player);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        int hauteur_y = 0;
        for (Pipe& pipe : groundPipes) {
            if (pipe.posX < -50) {
                pipe.posX = 350 + 800;
                pipe.posY = 350 + (std::rand() % 150);
                hauteur_y = pipe.posY;
            } else {
                if (!collideWithAPipe) {
                    pipe.posX -= 2;
                }
                SDL_Rect _pipe = {pipe.posX, pipe.posY, 60, 320};
                //SDL_RenderFillRect(renderer, &_pipe);
                SDL_RenderCopy(renderer, pipe0Texture, NULL, &_pipe);

                if (SDL_HasIntersection(&player, &_pipe)) {
                    collideWithAPipe = true;
                }
            }
        }

        for (Pipe& pipe : airPipes) {
            if (pipe.posX < -50) {
                pipe.posX = 350 + 800;
                pipe.posY = hauteur_y - 480;
            } else {
                if (!collideWithAPipe) {
                    pipe.posX -= 2;
                }
                SDL_Rect _pipe = {pipe.posX, pipe.posY, 60, 320};
                //SDL_RenderFillRect(renderer, &_pipe);
                SDL_RenderCopy(renderer, pipe1Texture, NULL, &_pipe);

                if (SDL_HasIntersection(&player, &_pipe)) {
                    collideWithAPipe = true;
                }
            }
        }

        for (Pipe& pipe : groundPipes) {
            if (pipe.posX-20 == player.x) {
                score++;
                sprintf(scoreBuffer, "%d", score);
            }
        }

        SDL_Rect scoreRect = {385, scoreYpos, 50, 50};
        if (collideWithAPipe) {
            if (scoreYpos < 300) {
                scoreYpos = scoreYpos + 2;
            }
            SDL_Rect gameoverRect = {310, gameoverYpos, 200, 100};
            if (collideWithAPipe) {
                if (gameoverYpos < 195) {
                    gameoverYpos = gameoverYpos + 2.4;
                }
            }
            renderScore(renderer, "Game Over", gameoverRect);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        renderScore(renderer, scoreBuffer, scoreRect);

        SDL_RenderPresent(renderer);
        SDL_Delay(7);
    }

    Mix_FreeChunk(jumpsound);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(pipe0Texture);
    SDL_DestroyTexture(pipe1Texture);
    SDL_DestroyTexture(birdTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}