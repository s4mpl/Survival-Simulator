#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <list>
#include "Ball.h"

class Player : public Entity {
	protected:
        float radius;

        float health;
        float maxHealth;
        int ammo;
        int maxAmmo;
        int totalAmmo;
        float rotationAngle;
        sf::Vector2f relativePos; // player position relative to the mouse
        
        sf::Texture texture;

        char weapon; // 0 = pistol (1.0 attack speed), 1 = 
        sf::Vector2f barrelPos;
        float attackSpeed;
        float attackTime;
        float reloadSpeed;
        float reloadTime;
        bool reloading;

	public:
        Player(int id, sf::Clock &clock);
        void update(std::set<Entity *> *closeEntities) final;
        void draw(sf::RenderWindow &window) final;
        std::string getInfo() const final;
        void setVelocity(sf::Vector2f v);
        void addVelocity(sf::Vector2f v);
        void damagePlayer(float amount);
        void rotateTo(sf::Vector2i pos);
        void shoot(std::list<Entity *> *e);
        void reload();
};