#define _USE_MATH_DEFINES
#include "Player.h"
#include "Utils.h"
#include "Bullet.h"
#include <math.h>
#include <list>

const float xMax = 800;
const float xMin = 0;
const float yMax = 600;
const float yMin = 0;
const float vMax = 100;
extern int GLOBAL_ID_COUNT;

Player::Player(int id, sf::Clock& clock) : Entity{ id, clock } {
    pos = { (xMin + xMax) / 2, (yMin + yMax) / 2 };
    health = maxHealth = 100;
    radius = length = 15;
    mass = pow(radius, 2);
    elasticity = 1;

    ammo = 7;
    maxAmmo = 7;
    totalAmmo = 42;
    weapon = '0';
    attackSpeed = 0.75f;
    attackTime = currTime;
    reloadSpeed = 2.5f;
    reloadTime = currTime;
    reloading = false;

    if (!texture.loadFromFile("resources/eyes.png")) exit(-1);

    // move to Weapon
    if (!reloadSoundBuffer.loadFromFile("resources/pistol-reload.wav")) exit(-1);
    if (!gun1SoundBuffer.loadFromFile("resources/pistol-shot-1.wav")) exit(-1);
    if (!gun2SoundBuffer.loadFromFile("resources/pistol-shot-2.wav")) exit(-1);
    if (!emptyGunSoundBuffer.loadFromFile("resources/empty-gun-shot.wav")) exit(-1);
    reloadSound.setBuffer(reloadSoundBuffer);
    gunSound1.setBuffer(gun1SoundBuffer);
    gunSound2.setBuffer(gun2SoundBuffer);
    numSounds = 2;
    emptyGunSound.setBuffer(emptyGunSoundBuffer);
}

void Player::update(std::set<Entity *> *closeEntities) {
    float overlap, mag, otherMag, colTime;
    sf::Vector2f p1Old, p2Old, p1New, p2New, p1Col, p2Col, v1Old, v2Old;
    collided = NULL;
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
    }

    // Update velocity
    vel.x += acc.x * dt;
    vel.y += acc.y * dt;

    // Max velocity
    if (vel.x > vMax) vel.x = vMax;
    else if (vel.x < -vMax) vel.x = -vMax;
    if (vel.y > vMax) vel.y = vMax;
    else if (vel.y < -vMax) vel.y = -vMax;

    // Friction
    if(magnitude(vel) > 0) vel -= 5.0f > magnitude(vel) ? vel : unit(vel) * 5.0f;

    if (health > maxHealth) health = maxHealth;
    if (health < 0) health = 0;

    // Continuous border collision detection
    p1Old = pos;
    p1New = p1Old + vel * dt;

    for (colTime = 0; colTime <= 1; colTime += 0.001f) { // Could be better
        // Interpolate positions
        p1Col = p1Old + (p1New - p1Old) * colTime;

        if (p1Col.x + radius > xMax || p1Col.x - radius < xMin) {
            // Handle the balls at this timestep
            dt *= colTime;
            pos.x = p1Col.x;

            vel.x = -vel.x * elasticity;

            // Get ball out of edge
            if (pos.x + radius > xMax) pos.x = xMax - radius;
            if (pos.x - radius < xMin) pos.x = xMin + radius;

            break;
        }

        if (p1Col.y + radius > yMax || p1Col.y - radius < yMin) {
            // Handle the balls at this timestep
            dt *= colTime;
            pos.y = p1Col.y;

            vel.y = -vel.y * elasticity;

            // Get ball out of edge
            if (pos.y + radius > yMax) pos.y = yMax - radius;
            if (pos.y - radius < yMin) pos.y = yMin + radius;

            break;
        }
    }
}

void Player::draw(sf::RenderWindow &window) {
    // Player
    sf::CircleShape circle(radius);
    circle.setOrigin(radius, radius);
    circle.setPosition(pos);
    circle.setFillColor(sf::Color::White);
    circle.setOutlineColor(sf::Color::Black);
    circle.setOutlineThickness(-radius / 8);
    circle.setRotation(rotationAngle);
    circle.setTexture(&texture);

    // Health bar
    sf::RectangleShape hb({ 52, 4 });
    hb.setOrigin({ 26, 2 });
    hb.setPosition(pos + sf::Vector2f{ 0, -1 - radius - 5 });
    hb.setOutlineColor(sf::Color::Black);
    hb.setOutlineThickness(-1);
    sf::RectangleShape hp({ health / 2, 2});
    hp.setOrigin({ 26, 2 });
    hp.setPosition(pos + sf::Vector2f{ 1, -radius - 5 });
    if (health >= 80) hp.setFillColor(sf::Color::Green);
    else if (health >= 60) hp.setFillColor(sf::Color(150, 255, 0, 255));
    else if (health >= 40) hp.setFillColor(sf::Color(255, 200, 0, 255));
    else if (health >= 20) hp.setFillColor(sf::Color(255, 100, 0, 255));
    else hp.setFillColor(sf::Color::Red);

    // Weapon
    if (weapon == '0') {
        sf::RectangleShape pistol({ 20, 6 });
        pistol.setOrigin({ -radius + 4, -8 });
        pistol.setPosition(pos);
        pistol.setRotation(rotationAngle);
        pistol.setFillColor(sf::Color(45, 45, 45, 255));
        window.draw(pistol);
        barrelPos = { pos + sf::Vector2f{ unit(relativePos).x * (radius + 16), unit(relativePos).y * (radius + 16) }
                      - sf::Vector2f{ (float)cos((rotationAngle - 90) * M_PI / 180) * 11, (float)sin((rotationAngle - 90) * M_PI / 180) * 11 } }; // perpendicular vector
    }

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


    window.draw(circle);
    window.draw(hb);
    window.draw(hp);
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

std::string Player::getInfo() const {
    return "FPS: " + std::to_string((int)(1 / dt)) +
           //"\nVelocity: " + std::to_string((int)vel.x) + ", " + std::to_string((int)vel.y) +
           //"\nRotation: " + std::to_string(rotationAngle) +
           "\nHealth: " + std::to_string((int)health) + "/" + std::to_string((int)maxHealth) +
           "\nPistol: " + std::to_string(ammo) + "/" + std::to_string(maxAmmo) + " (" + std::to_string(totalAmmo) + ")"; // change to be weapon.getInfo()
}

void Player::setVelocity(sf::Vector2f v) {
    vel = v;
}

void Player::addVelocity(sf::Vector2f v) {
    vel += v;
}

void Player::damagePlayer(float amount) {
    health -= amount;
    if (health < 0) health = 0;
}

void Player::rotateTo(sf::Vector2i pos) {
    relativePos = { (float)pos.x - this->pos.x, (float)pos.y - this->pos.y };
    if (relativePos.x != 0) rotationAngle = atan(relativePos.y / relativePos.x) * 180 / M_PI;
    if (relativePos.x < 0) rotationAngle += 180; // arctan only defined from -pi/2 to pi/2
}

// Add new bullets to the entity list
void Player::shoot(std::list<Entity *> *e) {
    if (attackTime >= attackSpeed && !reloading) {
        if (ammo > 0) {
            switch (weapon) {
            case '0':
                // change to new Bullet(Vector2f spawnPos, Vector2f targetPos)
                e->push_back(new Ball(GLOBAL_ID_COUNT, barrelPos.x, barrelPos.y, unit(relativePos).x * 700, unit(relativePos).y * 700, 0, 0, 2.5, this->c, 0, sf::Color(235, 205, 50, 255)));
                rand() % numSounds == 0 ? gunSound1.play() : gunSound2.play();
                GLOBAL_ID_COUNT++;
                break;
            case '1':
                //
                break;
            default:
                break;
            }
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

void Player::reload() {
    if (ammo < maxAmmo && !reloading && totalAmmo > 0) {
        reloadTime = 0;
        reloading = true;
        reloadSound.play();
    }
}

std::string Player::getEntity() const {
    return "Player"; // alternatively, check id == -1
}

std::string Player::getShape() const {
    return "circle";
}