#include "SmartBullet.h"
#include "Utils.h"

const float xMax = gameWidth;
const float xMin = 0;
const float yMax = gameHeight;
const float yMin = 0;
const float vMax = 2000;

SmartBullet::SmartBullet(int id, sf::Vector2f spawnPos, sf::Vector2f targetDir, sf::Clock& clock, sf::RenderWindow& window, int upgrades) : Bullet{ id, spawnPos, targetDir, clock } {
    this->window = &window;
    this->upgrades = upgrades;

    health = maxHealth = 1; // cannot rebound
    color = sf::Color(0, 165, 225, 255);
    elasticity = 0;
    lifeSpan = 10.0f;
    vel = unit(targetDir) * 50.0f;
    acc = sf::Vector2f{ 0, 0 };
    length = 4.0f;
}

void SmartBullet::update(std::set<Entity*>* closeEntities) {
    float overlap, mag, otherMag, colTime;
    sf::Vector2f p1Old, p2Old, p1New, p2New, p1Col, p2Col, v1Old, v2Old, v1New, v2New;
    collided = NULL;

    // Get delta time
    lastTime = currTime;
    currTime = c.getElapsedTime().asSeconds();
    dt = currTime - lastTime;

    // Update velocity
    mousePos = sf::Mouse::getPosition(*window);
    vel = sf::Vector2f{ mousePos.x - pos.x, mousePos.y - pos.y } * 2.5f;
    if (((upgrades >> 4) & 1) == 1) vel *= 2.0f;

    // Terminal velocity
    if (vel.x > vMax) vel.x = vMax;
    else if (vel.x < -vMax) vel.x = -vMax;
    if (vel.y > vMax) vel.y = vMax;
    else if (vel.y < -vMax) vel.y = -vMax;

    /* Infinitely small velocity
    if (fabs(vel.x) < 0.1) vel.x = 0;
    if (fabs(vel.y) < 0.1) vel.y = 0;*/

    // Check if colliding with other entities (except other SmartBullets), update velocities
    for (std::set<Entity*>::iterator other = closeEntities->begin(); other != closeEntities->end(); other++) {
        if (id == (*other)->getId() || (*other)->getEntity() == "SmartBullet") continue;

        if (getShape() == "circle" && (*other)->getShape() == "circle") {
            Entity* otherE = (*other);
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
                overlap = length + otherE->getLength() - distance(p1Col, p2Col);

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

                        overlap = length + otherE->getLength() - distance(p1Col, p2Col);
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

                        if (otherE->getId() == -1) {
                            // Health lost is proportional to how hard the collision was (force against the player)
                            (*other)->damageEntity(magnitude(otherE->getVelocity()) / 5);
                        }
                    }

                    health--;
                    break;
                }
            }

            /* If still inside each other, separate them more and check again next frame
            overlap = length + otherE->getLength() - distance(p1Col, p2Col);
            if (overlap > 0) {
                // Separate the balls using half the overlap times the direction of old velocity (https://youtu.be/guWIF87CmBg?t=854)
                // It's not enough to separate in all cases for some reason (or it is doing the colision on both balls from each ball?), so multiply by some constant for now
                if (v1Old != sf::Vector2f{ 0, 0 } && elasticity != 0) pos -= unit(v1Old) * (overlap / 2);
                if (v2Old != sf::Vector2f{ 0, 0 } && otherE->getElasticity() != 0) otherE->setPosition(otherE->getPosition() - unit(v2Old) * (overlap / 2));
            }
            else {
                // If the other ball already handled the collision, still get rid of its pointer from closeEntities
                closeEntities->erase(other); // Should make closeEntities a member variable and remove "collided"
            }*/
        }
    }

    // Continuous border collision detection
    p1Old = pos;
    p1New = p1Old + vel * dt;

    for (colTime = 0; colTime <= 1; colTime += 0.001f) { // Could be better
        // Interpolate positions
        p1Col = p1Old + (p1New - p1Old) * colTime;

        if (p1Col.x + length > xMax || p1Col.x - length < xMin) {
            // Handle the balls at this timestep
            ricochetSound.play();
            health--;
            dt *= colTime;
            pos.x = p1Col.x;

            vel.x = -vel.x * elasticity;

            // Get ball out of edge
            if (pos.x + length > xMax) pos.x = xMax - length;
            if (pos.x - length < xMin) pos.x = xMin + length;

            break;
        }

        if (p1Col.y + length > yMax || p1Col.y - length < yMin) {
            // Handle the balls at this timestep
            ricochetSound.play();
            health--;
            dt *= colTime;
            pos.y = p1Col.y;

            vel.y = -vel.y * elasticity;

            // Get ball out of edge
            if (pos.y + length > yMax) pos.y = yMax - length;
            if (pos.y - length < yMin) pos.y = yMin + length;

            break;
        }
    }

    // Blinking effect
    color = sf::Color(color.r, color.g, color.b, 155 + sin(currTime) * 100);

    // Add trail to bullet
    sf::CircleShape* trailCirc = new sf::CircleShape(length);
    trailCirc->setFillColor(sf::Color(color.r, color.g, color.b, color.a / 4));
    trailCirc->setOrigin(length, length);
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

    lifeSpan -= dt;
    if (health <= 0 || lifeSpan <= 0) despawned = true;
}

void SmartBullet::draw(sf::RenderWindow& window) {
    Bullet::draw(window);

    // Laser
    if (((upgrades >> 1) & 1) == 1) {
        // Draw tracking line from bullet to cursor
        sf::Vertex line[] =
        {
            sf::Vertex(pos, sf::Color(color.r, color.g, color.b, 100)),
            sf::Vertex(sf::Vector2f{ (float)mousePos.x, (float)mousePos.y }, sf::Color::Transparent)
        };
        window.draw(line, 2, sf::Lines);
    }
}

std::string SmartBullet::getEntity() const {
    return "SmartBullet";
}

std::string SmartBullet::getShape() const {
    return "circle";
}