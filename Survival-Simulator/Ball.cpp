#include "Ball.h"
#include "Player.h"
#include "Utils.h"
#include "Line.h"

const float xMax = gameWidth;
const float xMin = 0;
const float yMax = gameHeight;
const float yMin = 0;
const float vMax = 1000;

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
    sf::Vector2f p1Old, p2Old, p1New, p2New, p1Col, p2Col, v1Old, v2Old, v1New, v2New, p1, p2, p3, p4;
    sf::Vector2f *out = new sf::Vector2f();
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

        if (getShape() == "circle" && (*other)->getShape() == "circle") {
            Entity *otherE = (*other);
            // Continuous collision detection
            p1Old = pos;
            p2Old = otherE->getPosition();
            p1New = p1Old + vel * dt;
            p2New = p2Old + vel * dt;
            v1Old = vel;
            v2Old = otherE->getVelocity();
            mag = magnitude(vel);
            otherMag = magnitude(otherE->getVelocity());

            for (colTime = 0; colTime <= 1; colTime += 0.001f) { // Could be better
                // Interpolate positions
                p1Col = p1Old + (p1New - p1Old) * colTime;
                p2Col = p2Old + (p2New - p2Old) * colTime;

                // If distance <= combined radii, they are intersecting
                overlap = radius + otherE->getLength() - distance(p1Col, p2Col);

                if (overlap > 0) {
                    // Prevent collisions from happening again between these two balls before they are updated again
                    if (otherE->collided != this) {
                        collided = *other;
                        otherE->collided = this;

                        // Handle the balls at this timestep
                        dt *= colTime;
                        otherE->dt *= colTime;
                        pos = p1Col;
                        otherE->getPosition() = p2Col;

                        overlap = radius + otherE->getLength() - distance(p1Col, p2Col);
                        if (overlap >= 0) {
                            // Separate the balls using half the overlap times the direction of old velocity (https://youtu.be/guWIF87CmBg?t=854)
                            // It's not enough to separate in all cases for some reason (or it is doing the colision on both balls from each ball?), so multiply by some constant for now
                            if (v1Old != sf::Vector2f{ 0, 0 } && elasticity != 0) pos -= unit(v1Old) * (/*4 / elasticity **/ overlap / 2);
                            if (v2Old != sf::Vector2f{ 0, 0 } && otherE->getElasticity() != 0) otherE->setPosition(otherE->getPosition() - unit(v2Old) * (/*4 / (*other)->elasticity **/ overlap / 2));
                        }

                        // Use old velocities in both calculations or else it updates one before the other
                        v1New = elasticity * computeCollision(pos, otherE->getPosition(), v1Old, v2Old, mass, otherE->getMass());
                        v2New = otherE->getElasticity() * computeCollision(otherE->getPosition(), pos, v2Old, v1Old, otherE->getMass(), mass);

                        vel = v1New;
                        otherE->setVelocity(v2New);

                        if (mag > 7) {
                            // Update sound of collision relative to current state / mass
                            ballSound.setVolume(0.01 * mag * mass);
                            ballSound.play();
                        }
                        if (otherMag > 7) {
                            if (otherE->getEntity() == "Ball") {
                                Ball *b = (Ball *)otherE;
                                // Update sound of collision relative to current state / mass
                                b->ballSound.setVolume(0.01 * otherMag * mass);
                                b->ballSound.play();
                            }
                        }
                        if (otherE->getId() == -1) {
                            // Health lost is proportional to how hard the collision was (force against the player)
                            (*other)->damageEntity(magnitude(otherE->getVelocity()) / 10);
                        }
                    }

                    break;
                }
            }

            // If still inside each other, separate them more and check again next frame
            overlap = radius + otherE->getLength() - distance(p1Col, p2Col);
            if (overlap > 0) {
                // Separate the balls using half the overlap times the direction of old velocity (https://youtu.be/guWIF87CmBg?t=854)
                // It's not enough to separate in all cases for some reason (or it is doing the colision on both balls from each ball?), so multiply by some constant for now
                if (v1Old != sf::Vector2f{ 0, 0 } && elasticity != 0) pos -= unit(v1Old) * (/*4 / elasticity **/ overlap / 2);
                if (v2Old != sf::Vector2f{ 0, 0 } && otherE->getElasticity() != 0) otherE->setPosition(otherE->getPosition() - unit(v2Old) * (/*4 / otherE->elasticity **/ overlap / 2));
            }
            else {
                // If the other ball already handled the collision, still get rid of its pointer from closeEntities
                closeEntities->erase(other); // Should make closeEntities a member variable and remove "collided"
            }
        }
        else if (getShape() == "circle" && (*other)->getShape() == "line") {
            Line *l = (Line *)(*other);
            p3 = l->getPos1();
            p4 = l->getPos2();

            if (LineCollision(pos, pos + vel * dt, p3, p4, out)) {
                float lRotAngle = l->getRotationAngle();
                pos = *out;
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
            vel.y *= elasticity;

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
            vel.x *= elasticity;

            // Get ball out of edge
            if (pos.y + radius > yMax) pos.y = yMax - radius;
            if (pos.y - radius < yMin) pos.y = yMin + radius;

            break;
        }
    }

    // Add trail to ball
    sf::CircleShape *trailCirc = new sf::CircleShape(radius);
    trailCirc->setFillColor(sf::Color(color.r, color.g, color.b, 50));
    trailCirc->setOrigin(radius, radius);
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
        trailR = log2(i) * radius / 4 > radius ? radius : log2(i) * radius / 4;
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