#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Audio.hpp>
#include "Utils.hpp"
#include <cstdlib>
#include <deque>

class Ball {
    protected:
        const float xMax = 800;
        const float xMin = 0;
        const float yMax = 600;
        const float yMin = 0;
        const float vMax = 10000;

        int id;

        float radius;
        float mass;
        sf::Vector2f pos;
        sf::Vector2f vel;
        sf::Vector2f acc;

        sf::SoundBuffer ballSoundBuffer;
        sf::Sound ballSound;

        sf::Clock c;
        float currTime;
        float lastTime;
        float dt;

        float elasticity;

        sf::Color color;

        std::string s = "not working";
        bool collided;

        std::deque<sf::CircleShape *> trail;

    public:
        Ball(int id, sf::Clock &clock) {
            // Load the sounds used in the game
            if (!ballSoundBuffer.loadFromFile("resources/ball.wav")) exit(-1);
            ballSound.setBuffer(ballSoundBuffer);

            this->id = id;

            radius = rand() % 50 + 5;
            mass = pow(radius, 2);
            pos = { 0, 0 };
            vel = { 0, 0 };
            acc = { 0, 0 };

            c = clock;
            currTime = c.getElapsedTime().asSeconds();

            elasticity = 1;

            color = sf::Color::Cyan;
            ballSound.setPitch(20 / radius);
        }

        Ball(int id, float xPos, float yPos, float xVel, float yVel, float radius, sf::Clock &clock) {
            // Load the sounds used in the game
            if (!ballSoundBuffer.loadFromFile("resources/ball.wav")) exit(-1);
            ballSound.setBuffer(ballSoundBuffer);

            this->id = id;

            this->radius = radius;
            mass = pow(radius, 2);
            pos = { xPos, yPos };
            vel = { xVel, yVel };
            acc = { 0, 0 };

            c = clock;
            currTime = c.getElapsedTime().asSeconds();

            elasticity = 1;

            color = sf::Color::Cyan;
            ballSound.setPitch(20 / radius);
        }

        Ball(int id, float xPos, float yPos, float xVel, float yVel, float xAcc, float yAcc, float radius, sf::Clock &clock) {
            // Load the sounds used in the game
            if (!ballSoundBuffer.loadFromFile("resources/ball.wav")) exit(-1);
            ballSound.setBuffer(ballSoundBuffer);

            this->id = id;

            this->radius = radius;
            mass = pow(radius, 2);
            pos = { xPos, yPos };
            vel = { xVel, yVel };
            acc = { xAcc, yAcc };

            c = clock;
            currTime = c.getElapsedTime().asSeconds();

            elasticity = 1;

            color = sf::Color::Cyan;
            ballSound.setPitch(20 / radius);
        }

        Ball(int id, float xPos, float yPos, float xVel, float yVel, float xAcc, float yAcc, float radius, sf::Clock& clock, float elasticity) {
            // Load the sounds used in the game
            if (!ballSoundBuffer.loadFromFile("resources/ball.wav")) exit(-1);
            ballSound.setBuffer(ballSoundBuffer);

            this->id = id;

            this->radius = radius;
            mass = pow(radius, 2);
            pos = { xPos, yPos };
            vel = { xVel, yVel };
            acc = { xAcc, yAcc };

            c = clock;
            currTime = c.getElapsedTime().asSeconds();

            this->elasticity = elasticity;

            color = sf::Color::Cyan;
            ballSound.setPitch(20 / radius);
        }

        Ball(int id, float xPos, float yPos, float xVel, float yVel, float xAcc, float yAcc, float radius, sf::Clock& clock, float elasticity, sf::Color color) {
            // Load the sounds used in the game
            if (!ballSoundBuffer.loadFromFile("resources/ball.wav")) exit(-1);
            ballSound.setBuffer(ballSoundBuffer);

            this->id = id;

            this->radius = radius;
            mass = pow(radius, 2);
            pos = { xPos, yPos };
            vel = { xVel, yVel };
            acc = { xAcc, yAcc };

            c = clock;
            currTime = c.getElapsedTime().asSeconds();

            this->elasticity = elasticity;

            this->color = color;
            ballSound.setPitch(20 / radius);
        }

        // For maps and sets to work with Balls
        bool operator<(const Ball &other) const {
            if (other.id < this->id) return true;
        }


        void update(std::set<Ball *> *closeEntities) { // Change to Entity later
            float overlap, mag, otherMag, colTime;
            sf::Vector2f p1Old, p2Old, p1New, p2New, p1Col, p2Col, v1Old, v2Old;
            collided = false;

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
            for (std::set<Ball *>::iterator other = closeEntities->begin(); other != closeEntities->end(); other++) {
                if (id == (*other)->id) continue;
                /*s = "Checking for collision... (" + std::to_string(currTime) + ")\n" +
                    "p1: " + std::to_string(pos.x) + ", p2: " + std::to_string((*other)->pos.x) +
                    "\ndistance: " + std::to_string(distance(pos, (*other)->pos));*/

                // Continuous collision detection
                p1Old = pos;
                p2Old = (*other)->pos;
                p1New = p1Old + vel * dt;
                p2New = p2Old + vel * dt;
                v1Old = vel;
                v2Old = (*other)->vel;
                mag = magnitude(vel);
                otherMag = magnitude((*other)->vel);

                for (colTime = 0; colTime <= 1; colTime += 0.001) { // Could be better
                    // Interpolate positions
                    p1Col = p1Old + (p1New - p1Old) * colTime;
                    p2Col = p2Old + (p2New - p2Old) * colTime;

                    // If distance <= combined radii, they are intersecting
                    overlap = radius + (*other)->radius - distance(p1Col, p2Col);

                    if (overlap > 0) {
                        // Prevent collisions from happening again with this ball before it is updated again
                        if (!(*other)->collided) {
                            collided = true;

                            // Handle the balls at this timestep
                            dt *= colTime;
                            (*other)->dt *= colTime;
                            pos = p1Col;
                            (*other)->pos = p2Col;

                            overlap = radius + (*other)->radius - distance(p1Col, p2Col);
                            if (overlap >= 0) {
                                // Separate the balls using half the overlap times the direction of old velocity (https://youtu.be/guWIF87CmBg?t=854)
                                // It's not enough to separate in all cases for some reason (or it is doing the colision on both balls from each ball?), so multiply by some constant for now
                                if (v1Old != sf::Vector2f{ 0, 0 } && elasticity != 0) pos -= unit(v1Old) * (/*4 / elasticity **/ overlap / 2);
                                if (v2Old != sf::Vector2f{ 0, 0 } && (*other)->elasticity != 0) (*other)->pos -= unit(v2Old) * (/*4 / (*other)->elasticity **/ overlap / 2);
                            }

                            // Use old velocities in both calculations or else it updates one before the other
                            vel = elasticity * computeCollision(p1Col, p2Col, v1Old, v2Old, mass, (*other)->mass);
                            (*other)->vel = (*other)->elasticity * computeCollision(p2Col, p1Col, v2Old, v1Old, (*other)->mass, mass);
                            if (mag > 7) {
                                // Update sound of collision relative to current state / mass
                                ballSound.setVolume(0.01 * mag * mass);
                                ballSound.play();
                            }
                            if (otherMag > 7) {
                                // Update sound of collision relative to current state / mass
                                (*other)->ballSound.setVolume(0.01 * otherMag * mass);
                                (*other)->ballSound.play();
                            }
                        }
                        // If the other ball already handled the collision, still get rid of the pointer from closeEntities
                        closeEntities->erase(other);
                        break;
                    }
                }
            }

            // Continuous border collision detection
            p1Old = pos;
            p1New = p1Old + vel * dt;

            for (colTime = 0; colTime <= 1; colTime += 0.001) { // Could be better
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
            trailCirc->setFillColor(sf::Color(color.r, color.g, color.b, 75));
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
            pos += vel * dt;

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

        void draw(sf::RenderWindow &window) {
            // Draw ball to the window using position vector
            sf::CircleShape circle(radius);
            circle.setOrigin(radius, radius);
            circle.setPosition(pos);
            circle.setFillColor(color);
            circle.setOutlineColor(sf::Color::Black);
            circle.setOutlineThickness(-3);

            for (int i = 0; i < trail.size(); i++) {
                window.draw(*trail[i]);
            }

            window.draw(circle);
        }

        sf::Vector2f getPosition() const {
            return pos;
        }

        sf::Vector2f getVelocity() const {
            return vel;
        }

        sf::Vector2f getAcceleration() const {
            return acc;
        }

        float getRadius() const {
            return radius;
        }

        int getId() const {
            return id;
        }

        sf::Sound getSound() const {
            return ballSound;
        }

        std::string getInfo() const {
            /*return "pos: (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")\n" +
                   "vel: (" + std::to_string(vel.x) + ", " + std::to_string(vel.y) + ")\n" +
                   "acc: (" + std::to_string(acc.x) + ", " + std::to_string(acc.y) + ")\n";*/
            //return s;
            return std::to_string(currTime);
        }
};