#include <SFML/Graphics.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <deque>
#include "Ball.hpp"

////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
float main() {
    // Define some constants
    const float gameWidth = 800;
    const float gameHeight = 600;
    const float gravityAccel = 0;

    int i, j;
    bool paused = false;

    // Create the window of the application
    sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "Physics Test",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    // Create the clock and entities
    sf::Clock clock;
    //std::deque<Entity> entities;
    std::deque<Ball *> balls; // Use a deque so that the list is not constantly copied around while resizing, losing the ability to play sound or something
    std::map<Ball, std::set<Ball *> *> closeEntities;
    //Ball ball = Ball(200, 200, 150, 120, 0, gravityAccel, 20, clock, 1);
    balls.push_back(new Ball(0, 200, 200, 150, 120, 0, gravityAccel, 20, clock, 1));
    balls.push_back(new Ball(1, 100, 200, 50, 0, 0, gravityAccel, 50, clock, 0.75, sf::Color::Red));
    balls.push_back(new Ball(2, 400, 100, -800, 100, 0, gravityAccel, 5, clock, 1.5, sf::Color::Black));
    balls.push_back(new Ball(3, 500, 200, 50, 0, 0, gravityAccel, 20, clock, 1, sf::Color::Yellow));
    balls.push_back(new Ball(4, 700, 200, -50, 0, 0, gravityAccel, 20, clock, 1, sf::Color::Green));
    balls.push_back(new Ball(5, 600, 350, 0, 0, 0, gravityAccel, 30, clock, 0.5, sf::Color::Magenta));

    std::set<Ball *> *ballSet = new std::set<Ball *>();
    for (i = 0; i < balls.size(); i++) {
        ballSet->insert(balls[i]);
    }

    // Load the text font
    sf::Font font;
    if (!font.loadFromFile("resources/sansation.ttf"))
        return EXIT_FAILURE;

    // Initialize the text
    sf::Text info;
    info.setFont(font);
    info.setCharacterSize(18);
    info.setFillColor(sf::Color::Black);

    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            // Window closed or escape key pressed: exit
            if ((event.type == sf::Event::Closed) ||
               ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
                window.close();
                break;
            }
            // Pause on click
            if (event.type == sf::Event::MouseButtonPressed) {
                paused = !paused;
            }
        }

        if (!paused) {
            // Clear the window
            window.clear(sf::Color::White);

            /* Optimization to determine close entities for collision-handling (https://youtu.be/eED4bSkYCB8?t=949)
            for (i = 0; i < balls.size(); i++) {
                for (j = 0; j < balls.size(); j++) {
                    if (j == i) continue;
                    // Check "x-range" of each to see if it is within the "x-range" of another
                    if ((balls[i].getPosition().x - balls[i].getRadius() < balls[j].getPosition().x + balls[j].getRadius() &&
                         balls[i].getPosition().x - balls[i].getRadius() > balls[j].getPosition().x - balls[j].getRadius())
                         ||
                        (balls[i].getPosition().x + balls[i].getRadius() < balls[j].getPosition().x + balls[j].getRadius() &&
                         balls[i].getPosition().x + balls[i].getRadius() > balls[j].getPosition().x - balls[j].getRadius())) {
                        std::map<Ball, std::set<Ball> *>::iterator it = closeEntities.find(balls[i]);
                        if (it != closeEntities.end()) {
                            std::set<Ball> *newSet = new std::set<Ball>();
                            newSet->insert(balls[j]);
                            closeEntities.insert(make_pair(balls[i], newSet));
                        }
                    }
                }
            }*/

            // For every entity...
            for (i = 0; i < balls.size(); i++) {
                // Update state
                balls[i]->update(ballSet);
                //balls[i].update(closeEntities.find(balls[i])->second);
                info.setString(balls[2]->getInfo());
                // Render the scene
                balls[i]->draw(window);
                window.draw(info);
            }

            // Display on screen
            window.display();
        }
    }

    return EXIT_SUCCESS;
}