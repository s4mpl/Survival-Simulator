#include "Ball.h"
#include "Player.h"
#include "Utils.h"
#include <typeinfo>

const float xMax = 800;
const float xMin = 0;
const float yMax = 600;
const float yMin = 0;
const float vMax = 1000;
std::string s = "not working";

Ball::Ball(int id, float xPos, float yPos, float xVel, float yVel, float xAcc, float yAcc, float length, sf::Clock& clock, float elasticity, sf::Color color)
    : Entity{ id, xPos, yPos, xVel, yVel, xAcc, yAcc, length, clock, elasticity, color } {
    // Load the sounds used in the game
    if (!ballSoundBuffer.loadFromFile("resources/ball.wav")) exit(-1);
    ballSound.setBuffer(ballSoundBuffer);

    radius = length;
    ballSound.setPitch(20 / radius);
}


void Ball::update(std::set<Entity *> *closeEntities) {
    float overlap, mag, otherMag, colTime;
    sf::Vector2f p1Old, p2Old, p1New, p2New, p1Col, p2Col, v1Old, v2Old, v1New, v2New;
    collided = NULL;

    // Get delta time
    lastTime = currTime;
    currTime = c.getElapsedTime().asSeconds();
    dt = currTime - lastTime;

    // Update velocity
    vel.x += acc.x * dt;
    vel.y += acc.y * dt;

    // Terminal velocity
    if (vel.x > vMax) vel.x = vMax;
    else if (vel.x < -vMax) vel.x = -vMax;
    if (vel.y > vMax) vel.y = vMax;
    else if (vel.y < -vMax) vel.y = -vMax;

    /* Infinitely small velocity
    if (fabs(vel.x) < 0.1) vel.x = 0;
    if (fabs(vel.y) < 0.1) vel.y = 0;*/

    // Check if colliding with other entities, update velocities
    for (std::set<Entity*>::iterator other = closeEntities->begin(); other != closeEntities->end(); other++) {
        if (id == (*other)->getId()) continue;
        /*s = "Checking for collision... (" + std::to_string(currTime) + ")\n" +
            "p1: " + std::to_string(pos.x) + ", p2: " + std::to_string((*other)->pos.x) +
            "\ndistance: " + std::to_string(distance(pos, (*other)->pos));*/

        if (getShape() == "circle" && (*other)->getShape() == "circle") {
            Ball *otherB = (Ball *)(*other);
            // Continuous collision detection
            p1Old = pos;
            p2Old = otherB->pos;
            p1New = p1Old + vel * dt;
            p2New = p2Old + vel * dt;
            v1Old = vel;
            v2Old = otherB->vel;
            mag = magnitude(vel);
            otherMag = magnitude(otherB->vel);

            for (colTime = 0; colTime <= 1; colTime += 0.001f) { // Could be better
                // Interpolate positions
                p1Col = p1Old + (p1New - p1Old) * colTime;
                p2Col = p2Old + (p2New - p2Old) * colTime;

                // If distance <= combined radii, they are intersecting
                overlap = radius + otherB->radius - distance(p1Col, p2Col);

                if (overlap > 0) {
                    // Prevent collisions from happening again between these two balls before they are updated again
                    if (otherB->collided != this) {
                        collided = *other;
                        otherB->collided = this;

                        // Handle the balls at this timestep
                        dt *= colTime;
                        otherB->dt *= colTime;
                        pos = p1Col;
                        otherB->pos = p2Col;

                        overlap = radius + otherB->radius - distance(p1Col, p2Col);
                        if (overlap >= 0) {
                            // Separate the balls using half the overlap times the direction of old velocity (https://youtu.be/guWIF87CmBg?t=854)
                            // It's not enough to separate in all cases for some reason (or it is doing the colision on both balls from each ball?), so multiply by some constant for now
                            if (v1Old != sf::Vector2f{ 0, 0 } && elasticity != 0) pos -= unit(v1Old) * (/*4 / elasticity **/ overlap / 2);
                            if (v2Old != sf::Vector2f{ 0, 0 } && otherB->elasticity != 0) otherB->pos -= unit(v2Old) * (/*4 / (*other)->elasticity **/ overlap / 2);
                        }

                        // Use old velocities in both calculations or else it updates one before the other
                        v1New = elasticity * computeCollision(pos, otherB->pos, v1Old, v2Old, mass, otherB->mass);
                        v2New = otherB->elasticity * computeCollision(otherB->pos, pos, v2Old, v1Old, otherB->mass, mass);

                        vel = v1New;
                        otherB->vel = v2New;

                        if (mag > 7) {
                            // Update sound of collision relative to current state / mass
                            ballSound.setVolume(0.01 * mag * mass);
                            ballSound.play();
                        }
                        if (otherMag > 7) {
                            if (otherB->id != -1) {
                                // Update sound of collision relative to current state / mass
                                otherB->ballSound.setVolume(0.01 * otherMag * mass);
                                otherB->ballSound.play();
                            }
                        }
                        if (otherB->id == -1) {
                            // Health lost is proportional to how hard the collision was (force against the player)
                            ((Player *)(*other))->damagePlayer(magnitude(otherB->vel) / 10);
                        }
                    }

                    break;
                }
            }

            // If still inside each other, separate them more and check again next frame
            overlap = radius + otherB->radius - distance(p1Col, p2Col);
            if (overlap > 0) {
                // Separate the balls using half the overlap times the direction of old velocity (https://youtu.be/guWIF87CmBg?t=854)
                // It's not enough to separate in all cases for some reason (or it is doing the colision on both balls from each ball?), so multiply by some constant for now
                if (v1Old != sf::Vector2f{ 0, 0 } && elasticity != 0) pos -= unit(v1Old) * (/*4 / elasticity **/ overlap / 2);
                if (v2Old != sf::Vector2f{ 0, 0 } && otherB->elasticity != 0) otherB->pos -= unit(v2Old) * (/*4 / otherB->elasticity **/ overlap / 2);
            }
            else {
                // If the other ball already handled the collision, still get rid of its pointer from closeEntities
                closeEntities->erase(other); // Should make closeEntities a member variable and remove "collided"
            }
        }
    }

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

            if (fabs(vel.x) > 5) {
                // Update sound of collision relative to current state / mass
                ballSound.setVolume(0.01 * magnitude(vel) * mass);
                ballSound.play();
            }

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

            if (fabs(vel.y) > 5) {
                // Update sound of collision relative to current state / mass
                ballSound.setVolume(0.01 * magnitude(vel) * mass);
                ballSound.play();
            }

            vel.y = -vel.y * elasticity;

            // Get ball out of edge
            if (pos.y + radius > yMax) pos.y = yMax - radius;
            if (pos.y - radius < yMin) pos.y = yMin + radius;

            break;
        }
    }

    // Add trail to ball
    sf::CircleShape *trailCirc = new sf::CircleShape(radius);
    //trailCirc->setFillColor(sf::Color(235, 205, 50, 100)); // gold color
    trailCirc->setFillColor(sf::Color(color.r, color.g, color.b, 25));
    //trailCirc->setOrigin(radius, radius);
    trailCirc->setPosition(pos);
    if (trail.size() < 20) {
        trail.push_back(trailCirc);
    }
    else {
        trail.push_back(trailCirc);
        trailCirc = *trail.begin();
        trail.pop_front();
        delete trailCirc;
    }

    //pos += vel * dt;

    /* Update position if not out-of-bounds, else collide with border
    if (!(pos.x + radius + vel.x * dt > xMax || pos.x - radius + vel.x * dt < 0)) pos.x += vel.x * dt;
    else {
        // Get ball out of edge
        if (pos.x + radius + vel.x * dt > xMax) pos.x = xMax - radius;
        if (pos.x - radius + vel.x * dt < 0) pos.x = 0 + radius;

        if (fabs(vel.x) > 5) {
            // Update sound of collision relative to current state / mass
            ballSound.setVolume(0.01 * magnitude(vel) * mass);
            ballSound.play();
        }
        vel.x = -vel.x * elasticity;
    }
    if (!(pos.y + radius + vel.y * dt > yMax || pos.y - radius + vel.y * dt < 0)) pos.y += vel.y * dt;
    else {
        // Get ball out of edge
        if (pos.y + radius + vel.y * dt > yMax) pos.y = yMax - radius;
        if (pos.y - radius + vel.y * dt < 0) pos.y = 0 + radius;

        if (fabs(vel.y) > 5) {
            // Update sound of collision relative to current state / mass
            ballSound.setVolume(0.01 * magnitude(vel) * mass);
            ballSound.play();
        }
        vel.y = -vel.y * elasticity;
    }*/
}

void Ball::draw(sf::RenderWindow &window) {
    // Draw ball to the window using position vector
    sf::CircleShape circle(radius);
    circle.setOrigin(radius, radius);
    circle.setPosition(pos);
    circle.setFillColor(color);
    circle.setOutlineColor(sf::Color::Black);
    circle.setOutlineThickness(-radius / 6.5);

    float trailR;
    for (int i = 0; i < trail.size(); i++) {
        trailR = log2(i) * radius / 3 > radius ? radius : log2(i) * radius / 4;
        trail[i]->setRadius(trailR); // trail gets smaller at the end
        trail[i]->setOrigin(trailR, trailR);
        window.draw(*trail[i]);
    }

    window.draw(circle);
}

float Ball::getRadius() const {
    return radius;
}

std::string Ball::getEntity() const {
    return "Ball";
}

std::string Ball::getShape() const {
    return "circle";
}