#pragma once
#include "Weapon.h"

class Pistol : public Weapon {
	protected:
		sf::SoundBuffer gunSoundBuffer2;

		std::list<Entity *> *entities;
		bool bursting;
		int burstFired;

	public:
		Pistol(Entity *user, sf::Clock clock);
		void shoot(std::list<Entity *> *e) override;
		void altfire(std::list<Entity *> *e) override; // burst-fire mode
		void update() override;
		void draw(sf::RenderWindow& window) override;
		std::string getInfo() const override;

		sf::Sound gunSound2;
};