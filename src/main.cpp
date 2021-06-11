#include "bird.hpp"
#include "definitions.hpp"
#include "displayingEvents.hpp"
#include "highscore.hpp"
#include "pause.hpp"
#include "pipe.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
using namespace sf;
using namespace std;

int main()
{

    // Create a window
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "fnf", Style::Close);
    window.setFramerateLimit(360);
    Texture backgroundImage;
    backgroundImage.loadFromFile("assets/background.png");
    Sprite background(backgroundImage);

    // map used in calculating scores
    map<int, bool> markedPipes;

    // load things
    loadFlappy();
    loadPipe();
    txtHighscore();

    // Game Loop
    while (window.isOpen())
    {
        Event event;
        handleEvent(window, event, isPaused, isGameover);

        // Startscreen or if esc is pressed
        if (gameLevel == gameStates[3])
        {

            // reset to default
            pipeRate = 150, gap = 225, gnu.isAlive = 1, frames = 0;
            isPaused = false, pauseSoundPlayed = false, highscoreSoundPlayed = false, checkedHighscore = false, firstTime = false;
            gnu.x = 150, gnu.y = 200, gnu.velocity = 0;
            gnu.sprite.setPosition(gnu.x, gnu.y);
            pipes.clear();
            markedPipes.clear();

            // Displays Game Over Screen if isGameover flag is true
            if (isGameover)
            {
                displayGameover(window, score, userHighScore, isHighscore);
                if (event.key.code == Keyboard::Key::Escape)
                {
                    //stop sounds while playing if the user pressed the escape button
                    gnu.collide.stop();
                    isGameover = false;
                    isHighscore = false;
                }
            }
            // Displays Main Menu Screen if isGameover flag is false
            else
            {
                displayMenu(window, score);
            }
        }
        else
        {
            // Start Game if isPaused flag is false
            if (!isPaused)
            {
                // stop sound
                gnu.collide.stop();
                gnu.pauseSound.stop();
                pauseSoundPlayed = false;

                // easy
                if (gameLevel == gameStates[0])
                {
                    pipeRate = 200;
                    gap = 250;
                }
                // medium
                if (gameLevel == gameStates[1])
                {
                    pipeRate = 150;
                    gap = 225;
                }
                // hard
                if (gameLevel == gameStates[2])
                {
                    pipeRate = 120;
                    gap = 200;
                }

                // generate pipes
                if (frames % pipeRate == 0)
                {
                    // dark magic
                    int random = rand() % 150 + 200;

                    // set position of new pipes
                    positionPipe(random, gap);

                    // push pipes to the vector
                    pipes.push_back(pipeDown.sprite);
                    pipes.push_back(pipeUp.sprite);
                }

                // simulate moving
                for (auto itr = pipes.begin(); itr != pipes.end(); itr++)
                {
                    (*itr).move(-3, 0);
                }

                // remove pipes offscreen
                if (frames % 100 == 0)
                {
                    auto startIterator = pipes.begin();
                    auto endIterator = pipes.begin();

                    for (; endIterator != pipes.end(); endIterator++)
                    {
                        if ((*endIterator).getPosition().x > -104)
                        {
                            break;
                        }
                        // clear map for scoring
                        markedPipes[endIterator - startIterator] = 0;
                    }
                    pipes.erase(startIterator, endIterator);
                }

                // test collision
                for (auto i : pipes)
                {
                    float px, py, pw, ph;
                    float fx = gnu.sprite.getPosition().x, fy = gnu.sprite.getPosition().y;
                    float fw = 250.0 * gnu.sprite.getScale().x, fh = 253.0 * gnu.sprite.getScale().y; // bird width and high scaled

                    px = i.getPosition().x;
                    pw = 150 * i.getScale().x; // pipe width scaled
                    if (i.getScale().y > 0)
                    {
                        py = i.getPosition().y;
                        ph = 800 * i.getScale().y; // pipe height scaled
                    }
                    else
                    {
                        ph = -800 * i.getScale().y; // pipe height scaled
                        py = i.getPosition().y - ph;
                    }

                    // check colliding and ground
                    if (isColliding(fx, fy, fw, fh, px, py, pw, ph) || hitGround(fy))
                    {
                        gnu.isAlive = false;
                        // Save Highscore before closing
                        Highscore(score, userHighScore, gameLevel, isHighscore);
                    }
                }

                // stop game and return to main menu
                if (!gnu.isAlive)
                {
                    gameLevel = gameStates[3]; // Over
                    gnu.isAlive = 0;
                    gnu.highscoreSound.stop(); // dont mix sounds
                    gnu.flap.stop();
                    gnu.collide.play();
                    isGameover = true;
                    continue;
                }

                // gravity effect
                if (gnu.isAlive)
                {
                    gnu.velocity += gnu.gravAcc;
                    gnu.sprite.move(0, gnu.velocity);
                }

                // draw bird and pipes

                window.clear();
                window.draw(background);
                window.draw(gnu.sprite);

                for (auto i : pipes)
                {
                    window.draw(i);
                }
                displayScore(window, score);
                window.display();

                // score
                // check if it is first time for user
                if (!checkedHighscore)
                {
                    Highscore(score, userHighScore, gameLevel, isHighscore);
                    checkedHighscore = true;
                    if (userHighScore == 0)
                        firstTime = true;
                }
                // check if achieved highscore
                if (score > userHighScore)
                {
                    userHighScore = score;
                    isHighscore = true;
                }
                //  play highscore sound
                if (isHighscore && !highscoreSoundPlayed && !firstTime)
                {
                    gnu.highscoreSound.play();
                    highscoreSoundPlayed = true;
                }
                // calculate score
                for (int i = 0; i < pipes.size(); i++)
                {
                    // check if bird position exceeds pipe position + its scaled width to increment score
                    if (gnu.sprite.getPosition().x + 34.0 * gnu.sprite.getScale().x > pipes[i].getPosition().x + 150.0 * pipes[i].getScale().x && !markedPipes[i] && !markedPipes[i + 1])
                    {
                        score++;
                        markedPipes[i] = 1, markedPipes[i + 1] = 1; // mark pipes
                        break;
                    }
                }

                // increment frames
                frames++;
            }
            // Pause Game if isPaused flag is true
            else
            {
                // Pause sound when gamae is paused
                if (gnu.highscoreSound.getStatus() == SoundSource::Playing)
                    gnu.highscoreSound.pause();

                if (!pauseSoundPlayed)
                {
                    gnu.pauseSound.play();
                    pauseSoundPlayed = 1;
                }
                // Pause
                window.clear();
                window.draw(background);
                displayScore(window, score);
                window.draw(gnu.sprite);
                for (auto i : pipes)
                {
                    window.draw(i);
                }
                displayPause(window);
                window.display();
            }
        }
    }

    return 0;
}
