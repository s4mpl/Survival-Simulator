#pragma once
#include "Entity.h"
#include <deque>

// Class for general circular game objects but also standalone bouncy ball entity

class Ball : public Entity {
    protected:
        float radius;

        sf::SoundBuffer ballSoundBuffer;
        sf::Sound ballSound;

        std::deque<sf::CircleShape *> trail;

    public:
        Ball(int id, float xPos, float yPos, float xVel, float yVel, float xAcc, float yAcc, float length, sf::Clock& clock, float elasticity, sf::Color color);

        void update(std::set<Entity *> *closeEntities) override;
        void draw(sf::RenderWindow &window) override;

        float getRadius() const;
        virtual std::string getEntity() const;
        virtual std::string getShape() const;
};