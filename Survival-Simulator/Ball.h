#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <string>
#include <deque>

class Ball {
    protected:
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

        Ball *collided;

        std::deque<sf::CircleShape *> trail;

    public:
        Ball(int id, sf::Clock& clock);
        Ball(int id, float xPos, float yPos, float xVel, float yVel, float radius, sf::Clock& clock);
        Ball(int id, float xPos, float yPos, float xVel, float yVel, float xAcc, float yAcc, float radius, sf::Clock& clock);
        Ball(int id, float xPos, float yPos, float xVel, float yVel, float xAcc, float yAcc, float radius, sf::Clock& clock, float elasticity);
        Ball(int id, float xPos, float yPos, float xVel, float yVel, float xAcc, float yAcc, float radius, sf::Clock& clock, float elasticity, sf::Color color);

        bool operator<(const Ball& other) const;

        void update(std::set<Ball *> *closeEntities);
        void advance();
        void draw(sf::RenderWindow& window);

        sf::Vector2f getPosition() const;
        sf::Vector2f getVelocity() const;
        sf::Vector2f getAcceleration() const;
        float getRadius() const;
        int getId() const;
        std::string getInfo() const;
};