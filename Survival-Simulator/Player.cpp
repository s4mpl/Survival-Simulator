#include "Player.h"

const float xMax = 800;
const float xMin = 0;
const float yMax = 600;
const float yMin = 0;
const float vMax = 100;

Player::Player(int id, sf::Clock& clock) : Ball{ id, clock } {
    pos = { (xMin + xMax) / 2, (yMin + yMax) / 2 };
    health = maxHealth = 100;
    radius = 15;
    mass = pow(radius, 2);
    ammo = 5;
    maxAmmo = 5;
    totalAmmo = 100;
}

void Player::update(std::set<Ball *> *closeEntities) {
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
}

void Player::draw(sf::RenderWindow &window) {

}