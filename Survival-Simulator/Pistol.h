#pragma once
#include "Weapon.h"

class Pistol : public Weapon {
	protected:
		sf::SoundBuffer gunSoundBuffer2;

		std::list<Entity *> *entities;
		bool bursting;
		int burstFired;

	public:
		Pistol(Entity *user, sf::Clock clock, int weaponUpgrades);
		void shoot(std::list<Entity *> *e) final;
		void altfire(std::list<Entity *> *e) final; // burst-fire mode
		void update() final;
		void draw(sf::RenderWindow& window) final;
		std::string getInfo() const final;

		sf::Sound gunSound2;
};