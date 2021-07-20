#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include "Ball.h"

class Player : Ball {
	protected:
        float health;
        float maxHealth;
        int ammo;
        int maxAmmo;
        int totalAmmo;

	public:
        Player(int id, sf::Clock& clock);
        void update(std::set<Ball *> *closeEntities);
        void draw(sf::RenderWindow &window);
};