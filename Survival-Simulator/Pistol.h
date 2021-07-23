#pragma once
#include "Weapon.h"

class Pistol : public Weapon {
	protected:
		sf::SoundBuffer gunSoundBuffer2;

	public:
		Pistol(Entity *user, sf::Clock clock);
		void shoot(std::list<Entity *> *e) override;
		std::string getInfo() const override;

		sf::Sound gunSound2;
};