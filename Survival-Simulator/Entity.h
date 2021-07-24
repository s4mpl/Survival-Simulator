#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <string>

// General superclass for almost every object
class Entity {
    protected:
        int id;

        float length; // side length or radius
        float mass;
        sf::Vector2f pos;
        sf::Vector2f vel;
        sf::Vector2f acc;

        sf::Clock c;

        float elasticity;

        sf::Color color;

        float rotationAngle;
        sf::Vector2f relativePos;

        float health;
        float maxHealth;

    public:
        Entity(int id, sf::Clock &clock);
        Entity(int id, float xPos, float yPos, float xVel, float yVel, float xAcc, float yAcc, float length, sf::Clock &clock, float elasticity, sf::Color color);

        bool operator<(const Entity &other) const;

        virtual void update(std::set<Entity *> *closeEntities);
        virtual void advance();
        virtual void draw(sf::RenderWindow &window);

        sf::Vector2f getPosition() const;
        sf::Vector2f getVelocity() const;
        sf::Vector2f getAcceleration() const;
        void setPosition(sf::Vector2f pos);
        void setVelocity(sf::Vector2f vel);
        void setAcceleration(sf::Vector2f acc);
        float getLength() const;
        float getMass() const;
        float getElasticity() const;
        float getRotationAngle() const;
        sf::Vector2f getRelativePosition() const;
        int getId() const;
        virtual std::string getInfo() const;
        virtual std::string getEntity() const;
        virtual std::string getShape() const;
        void damageEntity(float amount);

        Entity *collided;
        float currTime;
        float lastTime;
        float dt;

        bool despawned;
};
