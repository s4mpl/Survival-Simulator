#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <list>
#include "Ball.h"
#include "Weapon.h"

class Player : public Entity {
	protected:
        float radius;

        //sf::Vector2f relativePos; // player position relative to the mouse
        
        sf::Texture texture;

        class Weapon *weapon;
        std::list<Weapon *> inventory;
        std::list<Weapon *>::iterator invIt;

        sf::SoundBuffer hurtSoundBuffer;
        sf::Sound hurtSound;
        sf::SoundBuffer deathSoundBuffer;
        sf::Sound deathSound;

	public:
        Player(int id, sf::Clock &clock);
        void update(std::set<Entity *> *closeEntities) final;
        void draw(sf::RenderWindow &window) final;
        void drawHealthBar(sf::RenderWindow& window);
        std::string getInfo() const final;
        void setVelocity(sf::Vector2f v);
        void addVelocity(sf::Vector2f v);
        void rotateTo(sf::Vector2i pos);
        std::string getEntity() const override;
        std::string getShape() const override;
        class Weapon *getWeapon() const;
        void damageEntity(float amount) override;

        void inventoryNext();
        void inventoryPrev();
};