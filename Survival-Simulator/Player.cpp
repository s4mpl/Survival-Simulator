#define _USE_MATH_DEFINES
#include "Player.h"
#include "Utils.h"
#include "Bullet.h"
#include <math.h>
#include <list>
#include "Pistol.h"
#include "SmartPistol.h"

const float xMax = gameWidth;
const float xMin = 0;
const float yMax = gameHeight;
const float yMin = 0;
const float vMax = 100;
extern int GLOBAL_ID_COUNT;

Player::Player(int id, sf::Clock& clock) : Entity{ id, clock } {
    pos = { (xMin + xMax) / 2, (yMin + yMax) / 2 };
    health = maxHealth = 100;
    radius = length = 15;
    mass = pow(radius, 2);
    elasticity = 1;

    weapon = new SmartPistol(this, clock, 0);

    if (!texture.loadFromFile("resources/eyes.png")) exit(-1);
}

void Player::update(std::set<Entity *> *closeEntities) {
    float overlap, mag, otherMag, colTime;
    sf::Vector2f p1Old, p2Old, p1New, p2New, p1Col, p2Col, v1Old, v2Old;
    collided = NULL;

    // Get delta time
    lastTime = currTime;
    currTime = c.getElapsedTime().asSeconds();
    dt = currTime - lastTime;

    // Update velocity
    vel.x += acc.x * dt;
    vel.y += acc.y * dt;

    // Max velocity
    if (vel.x > vMax) vel.x = vMax;
    else if (vel.x < -vMax) vel.x = -vMax;
    if (vel.y > vMax) vel.y = vMax;
    else if (vel.y < -vMax) vel.y = -vMax;

    // Friction
    if(magnitude(vel) > 0) vel -= 5.0f > magnitude(vel) ? vel : unit(vel) * 5.0f;

    if (health > maxHealth) health = maxHealth;
    if (health < 0) health = 0;

    // Continuous border collision detection
    p1Old = pos;
    p1New = p1Old + vel * dt;

    for (colTime = 0; colTime <= 1; colTime += 0.001f) { // Could be better
        // Interpolate positions
        p1Col = p1Old + (p1New - p1Old) * colTime;

        if (p1Col.x + radius > xMax || p1Col.x - radius < xMin) {
            // Handle the balls at this timestep
            dt *= colTime;
            pos.x = p1Col.x;

            vel.x = -vel.x * elasticity;

            // Get ball out of edge
            if (pos.x + radius > xMax) pos.x = xMax - radius;
            if (pos.x - radius < xMin) pos.x = xMin + radius;

            break;
        }

        if (p1Col.y + radius > yMax || p1Col.y - radius < yMin) {
            // Handle the balls at this timestep
            dt *= colTime;
            pos.y = p1Col.y;

            vel.y = -vel.y * elasticity;

            // Get ball out of edge
            if (pos.y + radius > yMax) pos.y = yMax - radius;
            if (pos.y - radius < yMin) pos.y = yMin + radius;

            break;
        }
    }
}

void Player::draw(sf::RenderWindow &window) {
    sf::CircleShape circle(radius);
    circle.setOrigin(radius, radius);
    circle.setPosition(pos);
    circle.setFillColor(sf::Color::White);
    circle.setOutlineColor(sf::Color::Black);
    circle.setOutlineThickness(-radius / 8);
    circle.setRotation(rotationAngle);
    circle.setTexture(&texture);

    window.draw(circle);
}

void Player::drawHealthBar(sf::RenderWindow& window) {
    sf::RectangleShape hb({ 52, 4 });
    hb.setOrigin({ 26, 2 });
    hb.setPosition(pos + sf::Vector2f{ 0, -1 - radius - 5 });
    hb.setOutlineColor(sf::Color::Black);
    hb.setOutlineThickness(-1);
    sf::RectangleShape hp({ health / 2, 2 });
    hp.setOrigin({ 26, 2 });
    hp.setPosition(pos + sf::Vector2f{ 1, -radius - 5 });
    if (health >= 80) hp.setFillColor(sf::Color::Green);
    else if (health >= 60) hp.setFillColor(sf::Color(150, 255, 0, 255));
    else if (health >= 40) hp.setFillColor(sf::Color(255, 200, 0, 255));
    else if (health >= 20) hp.setFillColor(sf::Color(255, 100, 0, 255));
    else hp.setFillColor(sf::Color::Red);

    window.draw(hb);
    window.draw(hp);
}

std::string Player::getInfo() const {
    return "FPS: " + std::to_string((int)(1 / dt)) +
           //"\nVelocity: " + std::to_string((int)vel.x) + ", " + std::to_string((int)vel.y) +
           //"\nRotation: " + std::to_string(rotationAngle) +
           "\nHealth: " + std::to_string((int)health) + "/" + std::to_string((int)maxHealth) +
           "\n" + weapon->getInfo();
}

void Player::setVelocity(sf::Vector2f v) {
    vel = v;
}

void Player::addVelocity(sf::Vector2f v) {
    vel += v;
}

void Player::rotateTo(sf::Vector2i pos) {
    relativePos = { (float)pos.x - this->pos.x, (float)pos.y - this->pos.y };
    if (relativePos.x != 0) rotationAngle = atan(relativePos.y / relativePos.x) * 180 / M_PI;
    if (relativePos.x < 0) rotationAngle += 180; // arctan only defined from -pi/2 to pi/2
}

std::string Player::getEntity() const {
    return "Player"; // alternatively, check id == -1
}

std::string Player::getShape() const {
    return "circle";
}

Weapon *Player::getWeapon() const {
    return weapon;
}