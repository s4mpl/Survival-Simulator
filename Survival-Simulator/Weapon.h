#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <list>
#include "Player.h"

class Weapon {
	// General weapon superclass
	// Handle weapon upgrades here as well (laser sight, faster attack/reload, more ammo, alt-fire, etc.)

    // Notes:
	// weapon powerup idea: attack/move faster the lower your health is, also allow weapon to modify user's velocity (max is pistol(s) at default)
	// sniper, shotgun, smg, rifle, dual pistols, smart pistol (bullet follows mouse), frag grenade, gauss rifle (charge up), homing rocket launcher
	// explosive barrel (red-orange -> red color when close to exploding based on hp)
	// pistol desc: Slow firing, but accurate and reliable, add bullet spread to smg and rifle
    // shotgun loads shells 1 at a time and can shoot to cancel
    // alt fire missile -- tracking, otherwise go straight

    protected:
        int id; // -1 = undef, 0 = pistol, 1 = ?
        Entity *user;
        sf::Clock c;

        int ammo;
        int maxAmmo;
        int totalAmmo;
        float attackSpeed;
        float attackTime;
        float reloadSpeed;
        float reloadTime;
        bool reloading;

        sf::Vector2f barrelPos;
        float userRadius;
        sf::Vector2f userPos;
        sf::Vector2f userRelPos;
        float userRotationAngle;

        sf::SoundBuffer reloadSoundBuffer;
        sf::SoundBuffer defaultSoundBuffer;
        sf::SoundBuffer emptyGunSoundBuffer;
        int numSounds; // number of weapon firing sound variants

        float currTime;
        float lastTime;
        float dt;

    public:
        Weapon(Entity *user, sf::Clock &clock);
        virtual void update();
        virtual void draw(sf::RenderWindow &window);
        virtual void shoot(std::list<Entity *> *e);
        virtual void reload();
        virtual std::string getInfo() const;

        sf::Sound reloadSound;
        sf::Sound defaultSound;
        sf::Sound emptyGunSound;
};