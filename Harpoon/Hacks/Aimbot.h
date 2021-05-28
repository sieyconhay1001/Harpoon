
class UserCmd;
class Entity;
class Vector;
class WeaponInfo;
namespace Aimbot {
	float hitChance(Entity* localPlayer, Entity* entity, Entity* weaponData, const Vector& destination, const UserCmd* cmd, const int hitChance) noexcept;
	bool canScan(Entity* entity, const Vector& destination, const WeaponInfo* weaponData, int minDamage, bool allowFriendlyFire, float& damageret) noexcept;
	void Run(UserCmd* cmd, bool& SendPacket);
}