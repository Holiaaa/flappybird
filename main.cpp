#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

// made by Téo JAUFFRET (Holiaaa)

class Pipe {
public:
    int posX = 0;
    int posY = 0;
};

int main(int argc, char *argv[]) {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Mon jeu");

    sf::Texture birdTexture;
    if (!birdTexture.loadFromFile("assets/sprites/bird.png")) {
        std::cout << "error while loading bird texture" << std::endl;
        return 1;
    }

    sf::Texture background;
    if(!background.loadFromFile("assets/background.png")) {
        std::cout << "error while loading background texture" << std::endl;
        return 1;
    }

    sf::Texture pipe0;
    sf::Texture pipe1;

    pipe0.loadFromFile("assets/sprites/pipe0.png");
    pipe1.loadFromFile("assets/sprites/pipe1.png");

    sf::Font font;
    if (!font.loadFromFile("assets/font/font.ttf")) {
        std::cout << "error while loading font" << std::endl;
        return 1;
    }

    sf::SoundBuffer sonbuffer;
    sonbuffer.loadFromFile("assets/sounds/jump.ogg");

    sf::Sound son;
    son.setBuffer(sonbuffer);

    int posY = 80;
    double birdSpeed = 0;
    double angle = 0.0;
    double angleSpeed = 0.0;

    int scoreValue = 0;
    char scoreBuffer[20];
    int scoreYPos = 50;
    sprintf(scoreBuffer, "%d", scoreValue);

    int gameoverYPos = -80;

    sf::Text score;
    score.setFont(font);
    score.setString(scoreBuffer);
    score.setCharacterSize(50);
    score.setFillColor(sf::Color::White);
    score.setPosition(395, scoreYPos);

    sf::Text gameoverText;
    gameoverText.setFont(font);
    gameoverText.setString("Game Over!");
    gameoverText.setPosition(sf::Vector2f(300, gameoverYPos));
    gameoverText.setFillColor(sf::Color::White);
    gameoverText.setCharacterSize(50);

    bool spacePressed = false;
    bool collideWithAPipe = false;

    sf::RectangleShape backgroundShape(sf::Vector2f(800, 600));
    backgroundShape.setPosition(sf::Vector2f(0, 0));
    backgroundShape.setTexture(&background);

    sf::Event event;
    sf::RectangleShape rectangle(sf::Vector2f(40, 30));
    rectangle.setOrigin(rectangle.getSize().x / 2, rectangle.getSize().y / 2);
    rectangle.setPosition(sf::Vector2f(100, posY));
    rectangle.setTexture(&birdTexture);

    Pipe groundPipes[4];
    Pipe airPipes[4];

    for (int i = 0; i < 4; i++) {
        groundPipes[i].posX = 300*i + 800;
        groundPipes[i].posY = 320 + (std::rand() % 150);
    }

    for (int i = 0; i < 4; i++) {
        airPipes[i].posX = groundPipes[i].posX;
        airPipes[i].posY = groundPipes[i].posY - 480;
    }

    while(window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (!spacePressed && !collideWithAPipe) {
                birdSpeed = -2.6;
                angleSpeed = 1.5;
                angle = -(rectangle.getPosition().x / 4) * 128;
                spacePressed = true;
                son.play();
            }
        } else {
            spacePressed = false;
        }

        window.clear();
        window.draw(backgroundShape);

        posY += birdSpeed;
        birdSpeed += 0.125;    
        
        if (angle > -60 && angle < 60) {
            angle += angleSpeed;
            angleSpeed += 0.015;
        } else {
            if (angle >= 60) {
                angle = 60;
            } else if (angle <= -60) {
                angle = -59;
            } 
        }

        rectangle.setRotation(angle);
        rectangle.setPosition(sf::Vector2f(100, posY));
        window.draw(rectangle);

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
                //SDL_Rect _pipe = {pipe.posX, pipe.posY, 60, 320};
                //SDL_RenderFillRect(renderer, &_pipe);
                //SDL_RenderCopy(renderer, pipe0Texture, NULL, &_pipe);

                sf::RectangleShape _pipe(sf::Vector2f(60, 320));
                _pipe.setPosition(sf::Vector2f(pipe.posX, pipe.posY));
                _pipe.setTexture(&pipe0);
                window.draw(_pipe);

                if (rectangle.getGlobalBounds().intersects(_pipe.getGlobalBounds())) {
                    collideWithAPipe = true;
                }

                /*if (SDL_HasIntersection(&player, &_pipe)) {
                    collideWithAPipe = true;
                }*/
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
                //SDL_Rect _pipe = {pipe.posX, pipe.posY, 60, 320};
                //SDL_RenderFillRect(renderer, &_pipe);
                //SDL_RenderCopy(renderer, pipe1Texture, NULL, &_pipe);

                sf::RectangleShape _pipe(sf::Vector2f(60, 320));
                _pipe.setPosition(sf::Vector2f(pipe.posX, pipe.posY));
                _pipe.setTexture(&pipe1);
                window.draw(_pipe);

                if (rectangle.getGlobalBounds().intersects(_pipe.getGlobalBounds())) {
                    collideWithAPipe = true;
                }

                /*if (SDL_HasIntersection(&player, &_pipe)) {
                    collideWithAPipe = true;
                }*/

            }
        }

        for (Pipe& pipe : groundPipes) {
            if (pipe.posX-20 == rectangle.getPosition().x) {
                scoreValue++;
                sprintf(scoreBuffer, "%d", scoreValue);
            }
        }

        score.setString(scoreBuffer);

        if (collideWithAPipe) {
            if (scoreYPos < 300) {
                scoreYPos = scoreYPos + 2;
                score.setPosition(sf::Vector2f(score.getPosition().x, scoreYPos));
            }
            if (gameoverYPos < 195) {
                gameoverYPos = gameoverYPos + 2.4;
                gameoverText.setPosition(sf::Vector2f(gameoverText.getPosition().x, gameoverYPos));
                gameoverText.setString("Game Over!");
            }
        }

        window.draw(gameoverText);
        window.draw(score);
        window.display();

        sf::sleep(sf::milliseconds(10));
    }

    return 0;
}
