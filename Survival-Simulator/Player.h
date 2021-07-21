#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include "Ball.h"

class Player : public Ball {
	protected:
        float health;
        float maxHealth;
        int ammo;
        int maxAmmo;
        int totalAmmo;

	public:
        Player(int id, sf::Clock& clock);
        void update(std::set<Ball *> *closeEntities) final;
        void draw(sf::RenderWindow &window) final;
        std::string getInfo() const final;
        void setVelocity(sf::Vector2f v);
        void addVelocity(sf::Vector2f v);
        void damagePlayer(float amount);
};