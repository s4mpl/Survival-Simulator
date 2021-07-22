#include "Entity.h"
#include "Player.h"
#include "Utils.h"
#include <typeinfo>

const float xMax = 800;
const float xMin = 0;
const float yMax = 600;
const float yMin = 0;
const float vMax = 1000;

Entity::Entity(int id, sf::Clock& clock) {
    this->id = id;

    c = clock;
}

Entity::Entity(int id, float xPos, float yPos, float xVel, float yVel, float xAcc, float yAcc, float length, sf::Clock& clock, float elasticity, sf::Color color) {
    this->id = id;

    this->length = length;
    this->mass = pow(length, 2);
    pos = { xPos, yPos };
    vel = { xVel, yVel };
    acc = { xAcc, yAcc };

    c = clock;
    currTime = c.getElapsedTime().asSeconds();

    this->elasticity = elasticity;

    this->color = color;
}

// For maps and sets to work with Entities
bool Entity::operator<(const Entity& other) const {
    if (other.id < this->id) return true;
}


void Entity::update(std::set<Entity *> *closeEntities) {
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
    for (std::set<Entity *>::iterator other = closeEntities->begin(); other != closeEntities->end(); other++) {
        if (id == (*other)->id) continue;

        if (getShape() == "circle" && (*other)->getShape() == "circle") {
            // Continuous collision detection
            p1Old = pos;
            p2Old = (*other)->pos;
            p1New = p1Old + vel * dt;
            p2New = p2Old + vel * dt;
            v1Old = vel;
            v2Old = (*other)->vel;
            mag = magnitude(vel);
            otherMag = magnitude((*other)->vel);

            for (colTime = 0; colTime <= 1; colTime += 0.001f) { // Could be better
                // Interpolate positions
                p1Col = p1Old + (p1New - p1Old) * colTime;
                p2Col = p2Old + (p2New - p2Old) * colTime;

                // If distance <= combined radii, they are intersecting
                overlap = length + (*other)->length - distance(p1Col, p2Col);

                if (overlap > 0) {
                    // Prevent collisions from happening again between these two entities before they are updated again
                    if ((*other)->collided != this) {
                        collided = *other;
                        (*other)->collided = this;

                        // Handle the entities at this timestep
                        dt *= colTime;
                        (*other)->dt *= colTime;
                        pos = p1Col;
                        (*other)->pos = p2Col;

                        overlap = length + (*other)->length - distance(p1Col, p2Col);
                        if (overlap >= 0) {
                            // Separate the entities using half the overlap times the direction of old velocity (https://youtu.be/guWIF87CmBg?t=854)
                            // It's not enough to separate in all cases for some reason (or it is doing the colision on both entities from each entity?), so multiply by some constant for now
                            if (v1Old != sf::Vector2f{ 0, 0 } && elasticity != 0) pos -= unit(v1Old) * (/*4 / elasticity **/ overlap / 2);
                            if (v2Old != sf::Vector2f{ 0, 0 } && (*other)->elasticity != 0) (*other)->pos -= unit(v2Old) * (/*4 / (*other)->elasticity **/ overlap / 2);
                        }

                        // Use old velocities in both calculations or else it updates one before the other
                        v1New = elasticity * computeCollision(pos, (*other)->pos, v1Old, v2Old, mass, (*other)->mass);
                        v2New = (*other)->elasticity * computeCollision((*other)->pos, pos, v2Old, v1Old, (*other)->mass, mass);

                        vel = v1New;
                        (*other)->vel = v2New;

                        if ((*other)->id == -1) {
                            // Health lost is proportional to how hard the collision was (force against the player)
                            ((Player*)(*other))->damagePlayer(magnitude((*other)->vel) / 10);
                        }
                    }

                    break;
                }
            }

            // If still inside each other, separate them more and check again next frame
            overlap = length + (*other)->length - distance(p1Col, p2Col);
            if (overlap > 0) {
                // Separate the entities using half the overlap times the direction of old velocity (https://youtu.be/guWIF87CmBg?t=854)
                // It's not enough to separate in all cases for some reason (or it is doing the colision on both entities from each entity?), so multiply by some constant for now
                if (v1Old != sf::Vector2f{ 0, 0 } && elasticity != 0) pos -= unit(v1Old) * (/*4 / elasticity **/ overlap / 2);
                if (v2Old != sf::Vector2f{ 0, 0 } && (*other)->elasticity != 0) (*other)->pos -= unit(v2Old) * (/*4 / (*other)->elasticity **/ overlap / 2);
            }
            else {
                // If the other entity already handled the collision, still get rid of its pointer from closeEntities
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

        if (p1Col.x + length > xMax || p1Col.x - length < xMin) {
            // Handle the entities at this timestep
            dt *= colTime;
            pos.x = p1Col.x;

            vel.x = -vel.x * elasticity;

            // Get entity out of edge
            if (pos.x + length > xMax) pos.x = xMax - length;
            if (pos.x - length < xMin) pos.x = xMin + length;

            break;
        }

        if (p1Col.y + length > yMax || p1Col.y - length < yMin) {
            // Handle the entities at this timestep
            dt *= colTime;
            pos.y = p1Col.y;

            vel.y = -vel.y * elasticity;

            // Get entity out of edge
            if (pos.y + length > yMax) pos.y = yMax - length;
            if (pos.y - length < yMin) pos.y = yMin + length;

            break;
        }
    }
}

void Entity::advance() {
    pos += vel * dt;
    collided = NULL;
}

void Entity::draw(sf::RenderWindow& window) {

}

sf::Vector2f Entity::getPosition() const {
    return pos;
}

sf::Vector2f Entity::getVelocity() const {
    return vel;
}

sf::Vector2f Entity::getAcceleration() const {
    return acc;
}

void Entity::setPosition(sf::Vector2f pos) {
    this->pos = pos;
}

void Entity::setVelocity(sf::Vector2f vel) {
    this->vel = vel;
}

void Entity::setAcceleration(sf::Vector2f acc) {
    this->acc = acc;
}

float Entity::getLength() const {
    return length;
}

int Entity::getId() const {
    return id;
}

std::string Entity::getInfo() const {
    return "pos: (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")\n" +
           "vel: (" + std::to_string(vel.x) + ", " + std::to_string(vel.y) + ")\n" +
           "acc: (" + std::to_string(acc.x) + ", " + std::to_string(acc.y) + ")\n";
}

std::string Entity::getEntity() const {
    return "Entity";
}

std::string Entity::getShape() const {
    return "undefined";
}