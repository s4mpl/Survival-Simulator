#pragma once

class Bullet : public Entity {
	// superclass: destroyed on collision, does damage; takes spawn position and target position; make trail more visible
	// sniper bullet will ricochet for X time, smart bullet despawns after 5 seconds
	// pistol ammo stays the same, sniper will be larger rectangle, rifle is smaller rectangle, smg same as pistol, shotgun is gray pistol, frag is smaller shotgun, smart is blinking light/dark red (currTime % 2)
};