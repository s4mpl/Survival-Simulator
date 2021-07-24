#define _USE_MATH_DEFINES
#include "Pistol.h"
#include "Utils.h"
#include "LightBullet.h"

extern int GLOBAL_ID_COUNT;

Pistol::Pistol(Entity *user, sf::Clock clock) : Weapon{ user, clock } {
    id = 0;
    ammo = 7;
    maxAmmo = 7;
    totalAmmo = 98;
    attackSpeed = 0.75f;
    reloadSpeed = 2.5f;
    numSounds = 2;

    bursting = false;
    burstFired = 0;

    if (!gunSoundBuffer2.loadFromFile("resources/pistol-shot-2.wav")) exit(-1);
    gunSound2.setBuffer(gunSoundBuffer2);

    weaponUpgrades = 1+2+4+8+16;
    if (((weaponUpgrades >> 0) & 1) == 1) ammo = 15;
}

void Pistol::shoot(std::list<Entity *> *e) {
    userRadius = user->getLength();
    userPos = user->getPosition();
    userRelPos = user->getRelativePosition();
    userRotationAngle = user->getRotationAngle();

    if (attackTime >= attackSpeed && !reloading) {
        if (ammo > 0) {
            e->push_back(new LightBullet(GLOBAL_ID_COUNT, barrelPos, userRelPos, c));
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
            if (bursting) burstFired++; // put here to prevent bug where shooting and bursting at the same time would allow the auto-shoot on left-click to shoot all bullets in one burst
            if (ammo < 1) reload();
        }
        else {
            emptyGunSound.play();
            attackTime = 0;
            if (bursting) burstFired++;
        }
    }
}

void Pistol::altfire(std::list<Entity *> *e) {
    if (((weaponUpgrades >> 4) & 1) == 1)
    if (!bursting && attackTime >= attackSpeed && !reloading) {
        entities = e;
        bursting = true;
        burstFired = 0;
    }
}

void Pistol::update() {
    int numReloaded = 0;
    if (((weaponUpgrades >> 0) & 1) == 1) maxAmmo = 15;
    if (((weaponUpgrades >> 2) & 1) == 1) attackSpeed = 0.45f;
    if (((weaponUpgrades >> 3) & 1) == 1) reloadSpeed = 1.45f;

    // Get delta time
    lastTime = currTime;
    currTime = c.getElapsedTime().asSeconds();
    dt = currTime - lastTime;

    attackTime += dt;
    reloadTime += dt;
    if (!reloading) reloadTime = 0; // for the reload bar
    if (attackTime > attackSpeed) attackTime = attackSpeed; // for the attack bar

    if (reloading && reloadTime >= reloadSpeed) {
        numReloaded = totalAmmo < maxAmmo - ammo ? totalAmmo : maxAmmo - ammo; // give the lesser of the two
        totalAmmo -= numReloaded;
        ammo += numReloaded;
        reloadTime = 0;
        reloading = false;
        reloadSound.stop(); // for fast reloads
    }

    if (bursting) {
        if (burstFired < 3 && !reloading) {
            // If the shot would not be successful, advance the timer -- should fire about once every 3 frames
            if (attackTime >= attackSpeed) shoot(entities);
            else attackTime += attackSpeed / 3;
        }
        // Reset burst upon reload
        else {
            bursting = false;
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

    // Laser sight
    if (((weaponUpgrades >> 1) & 1) == 1) {
        sf::Vertex line[] =
        {
            sf::Vertex(barrelPos, sf::Color::Red),
            sf::Vertex(barrelPos + 1000.0f * unit(userRelPos), sf::Color::Transparent)
        };
        window.draw(line, 2, sf::Lines);
    }

    Weapon::draw(window);
}

std::string Pistol::getInfo() const {
    return "Pistol: " + std::to_string(ammo) + "/" + std::to_string(maxAmmo) + " (" + std::to_string(totalAmmo) + ")";
}