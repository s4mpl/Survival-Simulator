#pragma once
#include "Ball.h"
#include "Bullet.h"

class LightBullet : public Bullet {
	public:
		LightBullet(int id, sf::Vector2f spawnPos, sf::Vector2f targetDir, sf::Clock& clock);
		void update(std::set<Entity*>* closeEntities) override;
		std::string getEntity() const override;
		std::string getShape() const override;
};