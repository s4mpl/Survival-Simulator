#pragma once
#include "Entity.h"
#include <SFML/Audio.hpp>
#include <deque>

// Superclass: despawned upon collision(s) or after a timespan, does damage; takes a spawn position and target position; more visible trail

// Notes:
// sniper bullet will ricochet for X time, smart bullet despawns after 5 seconds
// pistol ammo stays the same, sniper will be larger rectangle, rifle is smaller rectangle, smg same as pistol, shotgun is gray pistol, frag is smaller shotgun, smart is blinking light/dark red
// rocket is dark green

class Bullet : public Entity {
	protected:
		float damage;
		sf::Vector2f targetDir;
		std::deque<sf::CircleShape *> trail;

		sf::SoundBuffer ricochetSoundBuffer;
		sf::Sound ricochetSound;

		float lifeSpan;

	public:
		Bullet(int id, sf::Vector2f spawnPos, sf::Vector2f targetDir, sf::Clock& clock);
		void update(std::set<Entity *> *closeEntities) override;
		void draw(sf::RenderWindow& window) override;
		std::string getEntity() const override;
		std::string getShape() const override;
};