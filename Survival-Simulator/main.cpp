#include <SFML/Graphics.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <list>
#include "Ball.h"
#include "Player.h"

int GLOBAL_ID_COUNT = 0;
// Define some constants
const float gameWidth = 1600;
const float gameHeight = 1080;
const float gravityAccel = 0;


////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main() {
    int i, j;
    bool paused = false;

    // Create the window of the application
    sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "Physics Test",
                            sf::Style::None);
    window.setVerticalSyncEnabled(true);
    sf::Cursor cursor;
    if (cursor.loadFromSystem(sf::Cursor::Cross)) window.setMouseCursor(cursor);

    // Create the clock and entities
    sf::Clock clock;
    std::list<Entity *> entities; // Use a list so that it is not constantly copied around while resizing, losing the ability to play sound or something, and also I can add entities while traversing
    std::list<Entity *>::iterator it, it2;
    std::map<Entity *, std::set<Entity *> *> closeEntities;
    std::map<Entity *, std::set<Entity *> *>::iterator mit;
    std::set<Entity *> *entitySet;

    //Ball ball = Ball(200, 200, 150, 120, 0, gravityAccel, 20, clock, 1);
    /*balls.push_back(new Ball(0, 200, 200, 150, 120, 0, gravityAccel, 20, clock, 1));
    balls.push_back(new Ball(1, 100, 200, 50, 0, 0, gravityAccel, 50, clock, 0.75, sf::Color::Red));
    //balls.push_back(new Ball(2, 400, 100, -800, 100, 0, gravityAccel, 5, clock, 1.1, sf::Color::Black));
    balls.push_back(new Ball(3, 500, 200, 500, 0, 0, gravityAccel, 20, clock, 1, sf::Color::Yellow));
    balls.push_back(new Ball(4, 700, 200, -500, 0, 0, gravityAccel, 20, clock, 1, sf::Color::Green));
    balls.push_back(new Ball(5, 600, 350, 0, -200, 0, gravityAccel, 30, clock, 0.7, sf::Color::Magenta));*/

    /*for (i = 1; i <= 10; i++) {
        entities.push_back(new Ball(i-1, 45 * ((i % 20) + 1), 45 * ((i / 6) + 1), 200, 200, 0, gravityAccel, 10, clock, 1, sf::Color::Cyan));
        GLOBAL_ID_COUNT++;
    }*/

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
            // Pause on escape key release
            if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Escape)) {
                paused = !paused;
                //player->damageEntity(-100);
            }
            /* Alt-fire on right-click without auto-click
            if ((event.type == sf::Event::MouseButtonPressed) && (event.mouseButton.button == sf::Mouse::Right)) {
                player->getWeapon()->altfire(&entities);
            }*/
        }
        // Q to quit while paused
        if (paused) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) { window.close(); paused = !paused; }
        }
        else {
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
                player->getWeapon()->reload();
            }
            // Shoot on left-click
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                player->getWeapon()->shoot(&entities);
            }
            // Alt-fire on right-click
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                player->getWeapon()->altfire(&entities);
            }

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            player->rotateTo(mousePos);

            // For every entity...
            for (it = entities.begin(); it != entities.end(); it++) {
                mit = closeEntities.find(*it);
                // Check if it needs to be despawned
                if ((*it)->despawned) {
                    delete mit->second;
                    delete mit->first; // should be the same pointer as (*it)
                    closeEntities.erase(mit);
                    entities.erase(it);
                }
            }

            // Optimization to determine close entities for collision-handling (https://youtu.be/eED4bSkYCB8?t=949)
            for (it = entities.begin(); it != entities.end(); it++) {
                // Ensure every entity is in the map -- source of crashes for bullets that did not overlap with the player's 'x' value or another bullet upon spawning
                mit = closeEntities.find((*it));
                if (mit == closeEntities.end()) {
                    entitySet = new std::set<Entity *>();
                    closeEntities.insert(make_pair((*it), entitySet));
                }
                for (it2 = entities.begin(); it2 != entities.end(); it2++) {
                    if ((*it)->getId() == (*it2)->getId()) continue;
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

            for (it = entities.begin(); it != entities.end(); it++) {
                mit = closeEntities.find(*it);
                // Update state
                (*it)->update(mit->second);
                // Render the scene
                (*it)->draw(window);
            }
            for (it = entities.begin(); it != entities.end(); it++) {
                (*it)->advance();
            }

            player->getWeapon()->update();
            player->getWeapon()->draw(window);
            player->drawHealthBar(window);
            info.setString(player->getInfo());
            //info.setString("Number of entities: " + std::to_string(entities.size()) + ", " + std::to_string(closeEntities.size()));
            window.draw(info);

            // Display on screen
            window.display();
        }
    }

    return EXIT_SUCCESS;
}