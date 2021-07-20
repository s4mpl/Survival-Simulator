#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include <cmath>

float magnitude(sf::Vector2f v);
float distance(sf::Vector2f p1, sf::Vector2f p2);
float dot(sf::Vector2f v1, sf::Vector2f v2);
sf::Vector2f unit(sf::Vector2f v);
sf::Vector2f computeCollision(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f v1, sf::Vector2f v2, float m1, float m2);