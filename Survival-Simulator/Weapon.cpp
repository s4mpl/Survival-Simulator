#define _USE_MATH_DEFINES
#include "Weapon.h"
#include "Utils.h"
#include <math.h>

extern int GLOBAL_ID_COUNT;

Weapon::Weapon(Entity *user, sf::Clock &clock, int weaponUpgrades) {
	this->user = user;
    c = clock;

    if (!reloadSoundBuffer.loadFromFile("resources/pistol-reload.wav")) exit(-1);
    if (!defaultSoundBuffer.loadFromFile("resources/pistol-shot-1.wav")) exit(-1);
    if (!emptyGunSoundBuffer.loadFromFile("resources/empty-gun-shot.wav")) exit(-1);
    reloadSound.setBuffer(reloadSoundBuffer);
    defaultSound.setBuffer(defaultSoundBuffer);
    numSounds = 1;
    emptyGunSound.setBuffer(emptyGunSoundBuffer);

    id = -1;
    ammo = 7;
    maxAmmo = 7;
    totalAmmo = 7;
    currTime = c.getElapsedTime().asSeconds();
    attackSpeed = 0.05f;
    attackTime = currTime;
    reloadSpeed = 2.5f;
    reloadTime = currTime;
    reloading = false;

    this->weaponUpgrades = weaponUpgrades;
}

void Weapon::update() {
    int numReloaded = 0;

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

void Weapon::draw(sf::RenderWindow &window) {
    userRadius = user->getLength();
    userPos = user->getPosition();
    userRelPos = user->getRelativePosition();
    userRotationAngle = user->getRotationAngle();

    // Reloading bar / reloading progress
    sf::RectangleShape rb({ 102, 4 });
    rb.setPosition(5, 65);
    rb.setOutlineColor(sf::Color::Black);
    rb.setOutlineThickness(-1);
    sf::RectangleShape rp({ 100 * reloadTime / reloadSpeed, 2 });
    rp.setPosition(6, 66);
    rp.setFillColor(sf::Color::Black);

    sf::Font font;
    if (!font.loadFromFile("resources/UbuntuMono-Regular.ttf")) exit(-1);
    sf::Text rt;
    rt.setFont(font);
    rt.setCharacterSize(14);
    rt.setFillColor(sf::Color::Black);
    rt.setString(std::to_string(reloadTime));
    rt.setPosition(5, 70);

    // Attack speed bar
    sf::RectangleShape ab({ 102, 4 });
    ab.setPosition(5, 90);
    ab.setOutlineColor(sf::Color::Black);
    ab.setOutlineThickness(-1);
    sf::RectangleShape ap({ 100 * attackTime / attackSpeed, 2 });
    ap.setPosition(6, 91);
    ap.setFillColor(sf::Color(150, 0, 0, 255));

    sf::Text at;
    at.setFont(font);
    at.setCharacterSize(14);
    at.setFillColor(sf::Color(150, 0, 0, 255));
    at.setString(std::to_string(attackTime));
    at.setPosition(5, 95);

    window.draw(rb);
    window.draw(rp);
    window.draw(rt);
    window.draw(ab);
    window.draw(ap);
    window.draw(at);

    /* Draw barrel position for testing
    sf::CircleShape bp(2);
    bp.setFillColor(sf::Color::Black);
    bp.setOrigin(2, 2);
    bp.setPosition(barrelPos);
    window.draw(bp);*/
}

void Weapon::shoot(std::list<Entity *> *e) {
    userRadius = user->getLength();
    userPos = user->getPosition();
    userRelPos = user->getRelativePosition();
    userRotationAngle = user->getRotationAngle();

    if (attackTime >= attackSpeed && !reloading) {
        if (ammo > 0) {
            e->push_back(new Ball(GLOBAL_ID_COUNT, barrelPos.x, barrelPos.y, unit(userRelPos).x * 700, unit(userRelPos).y * 700, 0, 0, 2.5, this->c, 0, sf::Color(235, 205, 50, 255)));
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

void Weapon::altfire(std::list<Entity *> *e) {}

void Weapon::reload() {
    if (ammo < maxAmmo && !reloading && totalAmmo > 0) {
        reloadTime = 0;
        reloading = true;
        reloadSound.play();
    }
}

std::string Weapon::getInfo() const {
    return "Undefined: " + std::to_string(ammo) + "/" + std::to_string(maxAmmo) + " (" + std::to_string(totalAmmo) + ")";
}