#pragma once
#include "Bullet.h"

class SmartBullet : public Bullet {
	protected:
		sf::Vector2i mousePos;
		sf::RenderWindow *window;
		int upgrades;

		sf::SoundBuffer sonarSoundBuffer;
		sf::Sound sonarSound;
		float soundTimer;

	public:
		SmartBullet(int id, sf::Vector2f spawnPos, sf::Vector2f targetDir, sf::Clock& clock, sf::RenderWindow& window, int upgrades);
		void update(std::set<Entity*>* closeEntities) final;
		void draw(sf::RenderWindow& window) final;
		std::string getEntity() const final;
		std::string getShape() const final;
};