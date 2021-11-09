#include "Utils.h"

// Returns the magnitude of the given 2-D vector
float magnitude(sf::Vector2f v) {
	return sqrt(v.x * v.x + v.y * v.y);
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

// Calculates line-line collisions (https://en.sfml-dev.org/forums/index.php?topic=23624.0)
float PerpDot(const sf::Vector2f& a, const sf::Vector2f& b) { return (a.y * b.x) - (a.x * b.y); }

bool LineCollision(const sf::Vector2f& A1, const sf::Vector2f& A2, const sf::Vector2f& B1, const sf::Vector2f& B2, sf::Vector2f* out) {
    sf::Vector2f a(A2 - A1);
    sf::Vector2f b(B2 - B1);
    sf::Vector2f c(B2 - A2);

    float f = PerpDot(a, b);

    // lines are parallel
    if (!f) { return false; }

    float aa = PerpDot(a, c);
    float bb = PerpDot(b, c);

    if (f < 0)
    {
        if (aa > 0) return false;
        if (bb > 0) return false;
        if (aa < f) return false;
        if (bb < f) return false;
    }
    else
    {
        if (aa < 0) return false;
        if (bb < 0) return false;
        if (aa > f) return false;
        if (bb > f) return false;
    }

    if (out) { *out = b * (1.0f - (aa / f)) + B1; } // assigns the point of intersection

    return true;
}