#include <SFML/Graphics.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <list>
#include "Ball.h"
#include "Player.h"

int GLOBAL_ID_COUNT = 0;

////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main() {
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
    sf::Cursor cursor;
    if (cursor.loadFromSystem(sf::Cursor::Cross)) window.setMouseCursor(cursor);

    // Create the clock and entities
    sf::Clock clock;
    std::list<Entity *> entities; // Use a list so that it is not constantly copied around while resizing, losing the ability to play sound or something, and also I can add entities while traversing
    std::list<Entity *>::iterator it, it2;
    std::map<Entity*, std::set<Entity*> *> closeEntities;
    std::map<Entity*, std::set<Entity*> *>::iterator mit;
    std::set<Entity*> *entitySet;

    //Ball ball = Ball(200, 200, 150, 120, 0, gravityAccel, 20, clock, 1);
    /*balls.push_back(new Ball(0, 200, 200, 150, 120, 0, gravityAccel, 20, clock, 1));
    balls.push_back(new Ball(1, 100, 200, 50, 0, 0, gravityAccel, 50, clock, 0.75, sf::Color::Red));
    //balls.push_back(new Ball(2, 400, 100, -800, 100, 0, gravityAccel, 5, clock, 1.1, sf::Color::Black));
    balls.push_back(new Ball(3, 500, 200, 500, 0, 0, gravityAccel, 20, clock, 1, sf::Color::Yellow));
    balls.push_back(new Ball(4, 700, 200, -500, 0, 0, gravityAccel, 20, clock, 1, sf::Color::Green));
    balls.push_back(new Ball(5, 600, 350, 0, -200, 0, gravityAccel, 30, clock, 0.7, sf::Color::Magenta));*/
    /*for (i = 1; i <= 10; i++) {
        balls.push_back(new Ball(i-1, 45 * ((i % 20) + 1), 45 * ((i / 6) + 1), 200, 200, 0, gravityAccel, 10, clock, 1));
        GLOBAL_ID_COUNT++;
    }*/
    //balls.push_back(new Ball(2, 400, 100, -800, 100, 0, gravityAccel, 5, clock, 1.5, sf::Color(235, 205, 50, 100)));

    Player *player = new Player(-1, clock);
    entities.push_back(player);

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
            // Window closed -> exit
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }
            // Pause on escape key
            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)) {
                paused = !paused;
                player->damagePlayer(-100);
            }
        }

        if (!paused) {
            // Clear the window
            window.clear(sf::Color(175, 175, 175, 100));

            // Player movement
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                player->addVelocity({ 0, -50 });
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                player->addVelocity({ 0, 50 });
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                player->addVelocity({ 50, 0 });
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                player->addVelocity({ -50, 0 });
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                player->reload();
            }
            // Shoot on left click
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                player->shoot(&entities);
            }

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            player->rotateTo(mousePos);

            // Optimization to determine close entities for collision-handling (https://youtu.be/eED4bSkYCB8?t=949)
            for (it = entities.begin(); it != entities.end(); it++) {
                // Ensure every entity is in the map -- source of crashes for bullets that did not overlap with the player's 'x' value or another bullet upon spawning
                mit = closeEntities.find((*it));
                if (mit == closeEntities.end()) {
                    entitySet = new std::set<Entity *>();
                    closeEntities.insert(make_pair((*it), entitySet));
                }
                for (it2 = entities.begin(); it2 != entities.end(); it2++) {
                    if ((*it) == (*it2)) continue;
                    // Check "x-range" of each to see if it is within the "x-range" of another
                    if (((*it)->getPosition().x - (*it)->getLength() <= (*it2)->getPosition().x + (*it2)->getLength() + 1 &&
                         (*it)->getPosition().x - (*it)->getLength() >= (*it2)->getPosition().x - (*it2)->getLength() - 1)
                        ||
                        ((*it)->getPosition().x + (*it)->getLength() <= (*it2)->getPosition().x + (*it2)->getLength() + 1 &&
                         (*it)->getPosition().x + (*it)->getLength() >= (*it2)->getPosition().x - (*it2)->getLength() - 1)
                        ||
                        ((*it)->getPosition().x - (*it)->getLength() >= (*it2)->getPosition().x - (*it2)->getLength() &&
                         (*it)->getPosition().x + (*it)->getLength() <= (*it2)->getPosition().x + (*it2)->getLength())) {
                        mit = closeEntities.find((*it));
                        mit->second->insert((*it2));
                    }
                }
            }

            // For every entity...
            for (it = entities.begin(); it != entities.end(); it++) {
                // Update state
                mit = closeEntities.find(*it);
                (*it)->update(mit->second);
                // Render the scene
                (*it)->draw(window);
            }
            for (it = entities.begin(); it != entities.end(); it++) {
                (*it)->advance();
            }

            info.setString(player->getInfo());
            window.draw(info);

            // Display on screen
            window.display();
        }
    }

    return EXIT_SUCCESS;
}