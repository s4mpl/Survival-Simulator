#pragma once
#include "Weapon.h"

class SmartPistol : public Weapon {
	protected:
		std::list<Entity*>* entities;
		sf::RenderWindow *window;

	public:
		SmartPistol(Entity* user, sf::Clock clock, int weaponUpgrades);
		void shoot(std::list<Entity*>* e) final;
		void altfire(std::list<Entity*>* e) final; // burst-fire mode
		void update() override;
		void draw(sf::RenderWindow& window) final;
		std::string getInfo() const final;
};