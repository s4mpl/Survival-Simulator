#pragma once
#include "Entity.h"

// General superclass for objects that are lines (walls, AR bullets, and frags)

class Line : public Entity {
	protected:
		sf::Vector2f p1, p2;

	public:
		Line(int id, sf::Clock &clock, sf::Vector2f p1, sf::Vector2f p2);
		void update(std::set<Entity*>* closeEntities) override;
		void draw(sf::RenderWindow& window) override;
		std::string getEntity() const override;
		std::string getShape() const override;
		sf::Vector2f getPos1() const;
		sf::Vector2f getPos2() const;
};