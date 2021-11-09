#include "Line.h"
#include "Utils.h"

Line::Line(int id, sf::Clock& clock, sf::Vector2f p1, sf::Vector2f p2) : Entity { id, clock } {
	pos = sf::Vector2f{ (p2.x + p1.x) / 2, (p2.y + p1.y) / 2 };
	this->p1 = p1;
	this->p2 = p2;

	length = distance(p1, p2);

	rotationAngle = atan((p2 - p1).y / (p2 - p1).x);
}

void Line::update(std::set<Entity*>* closeEntities) {}

void Line::draw(sf::RenderWindow& window) {
	sf::Vertex line[] =
	{
		sf::Vertex(p1, color),
		sf::Vertex(p2, color)
	};
	window.draw(line, 2, sf::Lines);
}

std::string Line::getEntity() const {
	return "Line";
}

std::string Line::getShape() const {
	return "line";
}

sf::Vector2f Line::getPos1() const {
	return p1;
}

sf::Vector2f Line::getPos2() const {
	return p2;
}