#define _USE_MATH_DEFINES
#include "Pistol.h"
#include "Utils.h"
#include "Bullet.h"

extern int GLOBAL_ID_COUNT;

Pistol::Pistol(Entity *user, sf::Clock clock) : Weapon{ user, clock } {
    id = 0;
    ammo = 7;
    maxAmmo = 7;
    totalAmmo = 98;
    attackSpeed = 0.75f;
    reloadSpeed = 2.5f;
    numSounds = 2;

    if (!gunSoundBuffer2.loadFromFile("resources/pistol-shot-2.wav")) exit(-1);
    gunSound2.setBuffer(gunSoundBuffer2);
}

void Pistol::shoot(std::list<Entity *> *e) {
    userRadius = user->getLength();
    userPos = user->getPosition();
    userRelPos = user->getRelativePosition();
    userRotationAngle = user->getRotationAngle();

    if (attackTime >= attackSpeed && !reloading) {
        if (ammo > 0) {
            e->push_back(new Bullet(GLOBAL_ID_COUNT, barrelPos, userRelPos, c));
            switch (rand() % numSounds) {
                case 0:
                    defaultSound.play();
                case 1:
                    gunSound2.play();
                default:
                    emptyGunSound.play();
            }
            GLOBAL_ID_COUNT++;

            attackTime = 0;
            ammo--;
            if (ammo < 1) reload();
        }
        else {
            emptyGunSound.play();
            attackTime = 0;
        }
    }
}

void Pistol::draw(sf::RenderWindow& window) {
    // Weapon
    sf::RectangleShape pistol({ 20, 6 });
    pistol.setOrigin({ -userRadius + 4, -8 });
    pistol.setPosition(userPos);
    pistol.setRotation(userRotationAngle);
    pistol.setFillColor(sf::Color(45, 45, 45, 255));
    window.draw(pistol);
    barrelPos = { userPos + sf::Vector2f{ unit(userRelPos).x * (userRadius + 16), unit(userRelPos).y * (userRadius + 16) }
                    -sf::Vector2f{ (float)cos((userRotationAngle - 90) * M_PI / 180) * 11, (float)sin((userRotationAngle - 90) * M_PI / 180) * 11 } }; // perpendicular vector

    Weapon::draw(window);
}

std::string Pistol::getInfo() const {
    return "Pistol: " + std::to_string(ammo) + "/" + std::to_string(maxAmmo) + " (" + std::to_string(totalAmmo) + ")";
}