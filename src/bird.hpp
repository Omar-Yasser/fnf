#ifndef bird_H
#define bird_H
#include "definitions.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
using namespace sf;
struct gnu
{
    float x = 150, y = 200, velocity = 0;
    bool isAlive = 1;
    const float gravAcc = 0.010;
    const float jumpAcc = -1;
    Sprite sprite;
    SoundBuffer flapBuffer, collideBuffer;
    SoundBuffer highscoreBuffer, pauseBuffer;
    Sound highscoreSound, pauseSound;
    Sound flap, collide;
    Texture texture;
};

gnu gnu;

bool isColliding(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    if (x1 + w1 >= x2 && x1 <= x2 + w2 && y1 + h1 >= y2 && y1 <= y2 + h2)
        return true;
    return false;
}

bool hitGround(float y)
{
    if (y >= 692.0)
        return true;
    return false;
}

void loadFlappy()
{
    gnu.texture.loadFromFile("assets/gnu.png");
    gnu.sprite.setScale(0.3f, 0.3f);
    gnu.sprite.setTexture(gnu.texture);
    gnu.sprite.setPosition(gnu.x, gnu.y);
    gnu.flapBuffer.loadFromFile("audio/flap.wav");
    gnu.flap.setBuffer(gnu.flapBuffer);
    gnu.flap.setVolume(10);
    gnu.collideBuffer.loadFromFile("audio/gameover.wav");
    gnu.collide.setBuffer(gnu.collideBuffer);
    gnu.highscoreBuffer.loadFromFile("audio/highscore.wav");
    gnu.highscoreSound.setBuffer(gnu.highscoreBuffer);
    gnu.pauseBuffer.loadFromFile("audio/pause.wav");
    gnu.pauseSound.setBuffer(gnu.pauseBuffer);
}
#endif // bird_H