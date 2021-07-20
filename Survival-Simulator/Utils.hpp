#include <SFML/Graphics/CircleShape.hpp>
#include <cmath>

// Returns the magnitude of the given 2-D vector
float magnitude(sf::Vector2f v) {
	return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

// Returns the distance between two points
float distance(sf::Vector2f p1, sf::Vector2f p2) {
	return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

// Returns the dot product of two vectors
float dot(sf::Vector2f v1, sf::Vector2f v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

// Returns the unit vector in the direction of v
sf::Vector2f unit(sf::Vector2f v) {
	return v / magnitude(v);
}

// Computes the final velocity of object 1 (https://stackoverflow.com/questions/31620730/2d-elastic-collision-with-sfml)
sf::Vector2f computeCollision(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f v1, sf::Vector2f v2, float m1, float m2) {
	return v1 - (((2 * m2) / (m1 + m2)) * (dot(v1 - v2, p1 - p2) / pow(magnitude(p2 - p1), 2)) * (p1 - p2));
}