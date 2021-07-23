#include "Pistol.h"
#include "Utils.h"

extern int GLOBAL_ID_COUNT;

Pistol::Pistol(Entity *user, sf::Clock clock) : Weapon{ user, clock } {
    id = 0;
    totalAmmo = 98;
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
            // change to new Bullet(Vector2f spawnPos, Vector2f targetPos)
            e->push_back(new Ball(GLOBAL_ID_COUNT, barrelPos.x, barrelPos.y, unit(userRelPos).x * 700, unit(userRelPos).y * 700, 0, 0, 2.5, this->c, 0, sf::Color(235, 205, 50, 255)));
            rand() % numSounds == 0 ? defaultSound.play() : gunSound2.play();
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

std::string Pistol::getInfo() const {
    return "Pistol: " + std::to_string(ammo) + "/" + std::to_string(maxAmmo) + " (" + std::to_string(totalAmmo) + ")";
}