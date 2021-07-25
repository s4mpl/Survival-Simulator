#define _USE_MATH_DEFINES
#include "SmartPistol.h"
#include "Utils.h"
#include "SmartBullet.h"

extern int GLOBAL_ID_COUNT;

SmartPistol::SmartPistol(Entity* user, sf::Clock clock, int weaponUpgrades) : Weapon{ user, clock, weaponUpgrades } {
    id = 0;
    ammo = 5;
    maxAmmo = 5;
    totalAmmo = 45;
    attackSpeed = 1.25f;
    reloadSpeed = 2.5f;
    numSounds = 1;

    if (((weaponUpgrades >> 0) & 1) == 1) ammo = 12;

    if (!defaultSoundBuffer.loadFromFile("resources/smartpistol-shot.wav")) exit(-1);
}

void SmartPistol::shoot(std::list<Entity*>* e) {
    userRadius = user->getLength();
    userPos = user->getPosition();
    userRelPos = user->getRelativePosition();
    userRotationAngle = user->getRotationAngle();

    if (attackTime >= attackSpeed && !reloading) {
        if (ammo > 0) {
            e->push_back(new SmartBullet(GLOBAL_ID_COUNT, barrelPos, userRelPos, c, *window, weaponUpgrades));
            switch (rand() % numSounds) {
                case 0:
                    defaultSound.play();
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

void SmartPistol::altfire(std::list<Entity*>* e) {
    //if (((weaponUpgrades >> 4) & 1) == 1)
}

void SmartPistol::update() {
    int numReloaded = 0;
    if (((weaponUpgrades >> 0) & 1) == 1) maxAmmo = 12;
    if (((weaponUpgrades >> 2) & 1) == 1) attackSpeed = 0.30f;
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
}

void SmartPistol::draw(sf::RenderWindow& window) {
    this->window = &window;

    // Weapon
    sf::RectangleShape pistol({ 25, 6 });
    pistol.setOrigin({ -userRadius + 4, -8 });
    pistol.setPosition(userPos);
    pistol.setRotation(userRotationAngle);
    pistol.setFillColor(sf::Color(45, 45, 45, 255));
    window.draw(pistol);
    barrelPos = { userPos + sf::Vector2f{ unit(userRelPos).x * (userRadius + 24), unit(userRelPos).y * (userRadius + 24) }
                    -sf::Vector2f{ (float)cos((userRotationAngle - 90) * M_PI / 180) * 11, (float)sin((userRotationAngle - 90) * M_PI / 180) * 11 } }; // perpendicular vector

    sf::RectangleShape decal({ 15, 2 });
    decal.setOrigin({ -userRadius - 3, -10 });
    decal.setPosition(userPos);
    decal.setRotation(userRotationAngle);
    decal.setFillColor(sf::Color(0, 165, 225, 255));
    window.draw(decal);
    sf::RectangleShape suppressor({ 12, 8 });
    suppressor.setOrigin({ -userRadius - 18, -7 });
    suppressor.setPosition(userPos);
    suppressor.setRotation(userRotationAngle);
    suppressor.setFillColor(sf::Color(45, 45, 45, 255));
    window.draw(suppressor);
    sf::RectangleShape sight({ 2.5, 10 });
    sight.setOrigin({ -userRadius - 8, 0 });
    sight.setPosition(userPos);
    sight.setRotation(userRotationAngle);
    sight.setFillColor(sf::Color(0, 165, 225, 125));
    window.draw(sight);

    Weapon::draw(window);
}

std::string SmartPistol::getInfo() const {
    return "Smart Pistol: " + std::to_string(ammo) + "/" + std::to_string(maxAmmo) + " (" + std::to_string(totalAmmo) + ")";
}