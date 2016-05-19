class Action;
class Enchant;
#include "Entity.h"
#include "Constants.h"
#include "Geometry.h"
#include "Enchants.h"
#include <string>
#pragma warning (disable:4351)
#pragma warning(disable:4244)  //casting long into double 
#pragma warning(disable:4800)  //casting long into bool
using namespace ContainerDefs;
using namespace GameBaseConstants;



DefaultEntity::DefaultEntity(int hp, int mp, int reghp, int regmp, enumStrings name, enumStrings log_die,
    enumMaterial Material, long flags, DefaultEntity_AOEDamage* aoe, DefaultEntity_Trigger* trigger, ptfDie diefunc, 
    DefaultEntity *shares_textures_with, int animation_frames):
    hp(hp),  mp(mp), reghp(reghp), regmp(regmp), name(name), log_die(log_die), Material(Material), flags(flags),
    textures(), aoe(aoe), trigger(trigger), diefunc(diefunc), shares_textures_with(shares_textures_with), animation_frames(animation_frames) {

    if (shares_textures_with) {
        textures = shares_textures_with->textures;
    }
}
DefaultUnit::DefaultUnit(const DefaultEntity& defent,
    int fireresist, int shockresist, int armor, int speed, int charmsize,
    vector<Action *>& actions_, Weapon* weapon, int ammo, bool can_trap, bool can_bomber):
        DefaultEntity(defent),
        resist_fire(fireresist), resist_shock(shockresist), armor(armor), speed(speed),
        charm_size(charmsize), Actions(actions_), weapon(weapon), ammo(ammo), can_trap(can_trap), can_bomber(can_bomber){}
DefaultProjectile::DefaultProjectile(const DefaultEntity& defent, int speed, ptfCollision Collidefunc, 
    long reflectflags, long collideflags, long behaviourflags) :
DefaultEntity(defent), speed(speed), Collide(Collidefunc), 
reflectflags(reflectflags), collideflags(collideflags), behaviourflags(behaviourflags) {}

Entity* DefaultEntity::Create(const CoordI& coor, Team team, Direction dir) const {
    return new Entity(*this, coor, team, dir);
}
Entity* DefaultUnit::Create(const CoordI& coor, Team team, Direction dir) const {
    return new Unit(*this, coor, team, dir);
}

Entity* DefaultProjectile::Create(const CoordI& coor, Team team, Entity* target, Entity* source, enumEnchants id) const {
    return new EnchantProjectile(HomingProjectile(*this, coor, team, source, target), id);
}
Entity* DefaultProjectile::Create(const CoordI& coor, Team team, Entity* target, Entity* source) const {
    return new HomingProjectile(*this, coor, team, source, target);
}
Entity* DefaultProjectile::Create(const CoordI& coor, Team team, Entity* source, Angle angle) const {
    return new CommonProjectile(*this, coor, team, source, angle);
}
//TODO: set retrieving from pointer;
Dmg DefaultEntity::AuraDamage() const {
    if (aoe) {
        return aoe->aura_damage;
    } else {
        return Dmg();
    }
}
Dmg DefaultEntity::AuraRadiusDamage() const {
    if (aoe) {
        return aoe->aura_radius_damage;
    } else {
        return Dmg();
    }
}
bool DefaultEntity::AuraDamageAffectsFliers() const {
    if (aoe) {
        return aoe->affects_fliers;
    } else {
        return false;
    }
}
DefaultEntity_AOEDamage::DamageTarget DefaultEntity::AuraDamageType() const {
    if (aoe) {
        return aoe->type;
    } else {
        return DefaultEntity_AOEDamage::DAMAGES_NONE;
    }
}
ptfTrigger DefaultEntity::MyTrigger() const {
    return trigger->trigger;
}
bool DefaultEntity::TriggerFlagsMaskTest(const DefaultEntity& EntityToTest) const {
    return EntityToTest.flags & (trigger->entity_activating_flags);
}

bool DefaultEntity::BlocksMoving(const DefaultEntity* ent, bool UnitJumps) const
{
    if (Flag(ENT_IS_FLYING) || UnitJumps) { //it flies
        if (ent->Flag(ENT_IS_SOLID) &&
            (ent->Flag(ENT_IS_TALL) || ent->Flag(ENT_IS_FLYING))) {
            return true;
        }
    } else { //it doesnt fly
        if (ent->Flag(ENT_IS_SOLID)) { //it doesnt jump nor flies
            return true;
        }
    }
    return false;
}
bool DefaultProjectile::BlocksMoving(const DefaultEntity* ent, bool UnitJumps /*= false*/) const
{
    if (Flag(ENT_IS_FLYING) || UnitJumps) { //it flies
        if (ent->Flag(ENT_IS_SOLID) &&
            (ent->Flag(ENT_IS_TALL) || !ent->Flag(ENT_IS_FLYING))) {
            if (ent->Flag(ENT_IS_UNIT) && CollisionFlag(COLLIDESWITH_UNITS) ||
                !ent->Flag(ENT_IS_UNIT) && CollisionFlag(COLLIDESWITH_OBJECTS)) {
                return true;
            }
        }
    } else { //it doesnt fly
         if (ent->Flag(ENT_IS_SOLID)) { //it doesnt jump nor flies
            if (ent->Flag(ENT_IS_UNIT) && CollisionFlag(COLLIDESWITH_UNITS) ||
                !ent->Flag(ENT_IS_UNIT) && CollisionFlag(COLLIDESWITH_OBJECTS)) {
                return true;
            }
        }
    }
    return false;
}

Weapon::Weapon(Action* action, BLOCK_QUALITY blocktype, int manacost, int basedamage, DmgType type, vector<WeaponEnchant> chants): 
        action(action), block_quality(blocktype), mana_per_ammo(manacost), BaseDamage(basedamage), type(type) {
            for (int i = 0; i <max_weapon_enchants; ++i) {
                Enchants[i] = WeaponEnchant();
            }
            for (unsigned int i = 0; i < min(max_weapon_enchants, chants.size()); ++i) {
                Enchants[i] = chants[i];
            }
        }

Entity::Entity(const DefaultEntity& prototype, const CoordI& coor, ::Team team, Direction dir):
hp(prototype.hp), mp(prototype.mp), flags(0), prototype(prototype), coor(coor), lastcoor(coor),
dir(dir), team(team), object_owner(nullptr), subs(), is_dead(false), is_paralyzed(false) {}
Unit::Unit(const DefaultUnit& prototype, const CoordI& coor, ::Team team, Direction dir):
Entity(static_cast<const DefaultEntity&>(prototype), coor, team, dir),
enchants(), cooldowns(), movepoints(prototype.speed), ammo(prototype.ammo),
ammo_manabuffer(0), is_in_blocking_state(true), action_points(2), long_action(), channelings(){}
CommonProjectile::CommonProjectile(const DefaultProjectile& prototype, const CoordI& coor, ::Team team, Entity* source, ::Angle angle):
Projectile(Entity(static_cast<const DefaultEntity&>(prototype), coor, team, angle.getDirection())),
speed(prototype.speed), angle(angle), ex_coor(CoordD(0.5, 0.5) + coor) {}
Projectile::Projectile(const Entity& entity): Entity(entity) {}
HomingProjectile::HomingProjectile(const DefaultProjectile& prototype, const CoordI& coor, ::Team team, Entity* source, Entity* target) :
Projectile(Entity(static_cast<const DefaultEntity&>(prototype), coor, team, coor.getDirection(target->Coor()))), 
speed(prototype.speed), target(target), ex_coor(CoordD(0.5, 0.5) + coor) {}
EnchantProjectile::EnchantProjectile(const HomingProjectile& homing_base, enumEnchants enchant_id) :
HomingProjectile(homing_base), enchant_id(enchant_id) {}

void Loader::loadWeapons(WeaponContainer& container, ActionContainer& actions){
    vector<Weapon::WeaponEnchant> enchants;
    container.reserve(Counters::weapons);
    container.push_back(new Weapon(actions[ACTION_FIRE_STAFF], BLOCK_STAFF, 3));
    container.push_back(new Weapon(actions[ACTION_FORCE_STAFF], BLOCK_STAFF, 1));
    container.push_back(new Weapon(actions[ACTION_FON_STAFF], BLOCK_STAFF, 20));
    container.push_back(new Weapon(actions[ACTION_LONGSWORD], BLOCK_LONGSWORD, 0, 80, DMG_PHYS));
    container.push_back(new Weapon(actions[ACTION_HAMMER], NO_BLOCK, 0, 140, DMG_BLUDGEON));
    container.push_back(new Weapon(actions[ACTION_SHURIKEN], BLOCK_SHIELD, 0, 40, DMG_PHYS));
    container.push_back(new Weapon(actions[NO_ACTION], NO_BLOCK, 0)); //chakram
    container.push_back(new Weapon(actions[ACTION_MACE], BLOCK_SHIELD, 0, 40, DMG_BLUDGEON));
    enchants.push_back(Weapon::WeaponEnchant(CHANT_FIRE_SPLASH, 16));
    container.push_back(new Weapon(actions[ACTION_FIRESWORD], BLOCK_SHIELD, 0, 32, DMG_PHYS, enchants));
    container.push_back(new Weapon(actions[ACTION_HELLFIRE_STAFF], BLOCK_STAFF, 6));
    container.push_back(new Weapon(actions[ACTION_BOW], NO_BLOCK, 0));
}
void LoadDefaultUnits(EntityContainer& container, const ActionContainer& actions, const WeaponContainer& weapons) {
    ActionContainer tmp;
    tmp.assign(actions_per_set * actionsets_count, actions[NO_ACTION]);

    //WIZARD YELLOW
    tmp[0] = actions[ACTION_HASTE];
    tmp[1] = actions[ACTION_PROTECTION_FIRE];
    tmp[2] = actions[ACTION_PROTECTION_SHOCK];
    tmp[3] = actions[ACTION_FORCEFIELD];
    tmp[4] = actions[ACTION_HEAL_LESSER];

    tmp[5] = actions[ACTION_ANCHOR];
    tmp[6] = actions[ACTION_SWAP];
    tmp[7] = actions[ACTION_REFLECTIVE_SHIELD];
    tmp[8] = actions[ACTION_PROTECTION_POISON];
    tmp[9] = actions[ACTION_BURN];

    tmp[10] = actions[ACTION_INVERSION];
    tmp[11] = actions[ACTION_CHANNEL];
    tmp[12] = actions[ACTION_OBLITERATION];
    tmp[13] = actions[ACTION_MANA_DRAIN];
    tmp[14] = actions[ACTION_COUNTERSPELL];

    tmp[15] = actions[ACTION_EARTHQUAKE];
    tmp[16] = actions[ACTION_FIRE_STAFF];
    tmp[17] = actions[NO_ACTION];
    tmp[18] = actions[NO_ACTION];
    tmp[19] = actions[NO_ACTION];

    tmp[20] = actions[ACTION_PULL];
    tmp[21] = actions[ACTION_TELEPORT];
    tmp[22] = actions[ACTION_DISPELL_UNDEAD];
    tmp[23] = actions[ACTION_WALL];
    tmp[24] = actions[ACTION_PUSH];

    container[ENT_WIZ_YELLOW] = new DefaultUnit(DefaultEntity(75, 150, 1, 2, STR_ENT_WIZ_YELLOW, STR_WAS_KILLED,
        MATERIAL_FLESH, ENT_IS_PLAYER), 0, 0, 10, 2, 0, tmp, weapons[WEAP_FIRESTAFF], 20, true);

    //WIZARD BLUE

    tmp[0] = actions[ACTION_HASTE];
    tmp[1] = actions[ACTION_ENERGY_BOLT];
    tmp[2] = actions[ACTION_PROTECTION_SHOCK];
    tmp[3] = actions[ACTION_FORCEFIELD];
    tmp[4] = actions[ACTION_HEAL_LESSER];

    tmp[5] = actions[ACTION_INVERSION];
    tmp[6] = actions[ACTION_LIGHTNING];
    tmp[7] = actions[ACTION_REFLECTIVE_SHIELD];
    tmp[8] = actions[ACTION_COUNTERSPELL];
    tmp[9] = actions[ACTION_BURN];

    tmp[10] = actions[ACTION_CHANNEL];
    tmp[11] = actions[ACTION_EARTHQUAKE];
    tmp[12] = actions[ACTION_FORCE_STAFF];
    tmp[13] = actions[NO_ACTION];
    tmp[14] = actions[NO_ACTION];

    tmp[15] = actions[NO_ACTION];
    tmp[16] = actions[NO_ACTION];
    tmp[17] = actions[NO_ACTION];
    tmp[18] = actions[NO_ACTION];
    tmp[19] = actions[NO_ACTION];

    tmp[20] = actions[ACTION_PULL];
    tmp[21] = actions[ACTION_SHOCK];
    tmp[22] = actions[ACTION_DISPELL_UNDEAD];
    tmp[23] = actions[ACTION_WALL];
    tmp[24] = actions[ACTION_PUSH];
    container[ENT_WIZ_BLUE] = new DefaultUnit(DefaultEntity(75, 150, 1, 1, STR_ENT_WIZ_BLUE, STR_WAS_KILLED,
        MATERIAL_FLESH, ENT_IS_PLAYER), 0, 20, 10, 2, 0, tmp, weapons[WEAP_FORCESTAFF], 150, true);

    //WIZARD RED
    tmp[0] = actions[ACTION_HASTE];
    tmp[1] = actions[ACTION_FIREBALL];
    tmp[2] = actions[ACTION_PROTECTION_FIRE];
    tmp[3] = actions[ACTION_FORCEFIELD];
    tmp[4] = actions[ACTION_HEAL_LESSER];

    tmp[5] = actions[ACTION_ANCHOR];
    tmp[6] = actions[ACTION_MAGIC_MISSILE];
    tmp[7] = actions[ACTION_SLOW];
    tmp[8] = actions[ACTION_SWAP];
    tmp[9] = actions[ACTION_BURN];

    tmp[10] = actions[ACTION_TELEPORT];
    tmp[11] = actions[ACTION_HELLFIRE_STAFF];
    tmp[12] = actions[NO_ACTION];
    tmp[13] = actions[NO_ACTION];
    tmp[14] = actions[NO_ACTION];

    tmp[15] = actions[NO_ACTION];
    tmp[16] = actions[NO_ACTION];
    tmp[17] = actions[NO_ACTION];
    tmp[18] = actions[NO_ACTION];
    tmp[19] = actions[NO_ACTION];

    tmp[20] = actions[ACTION_INVERSION];
    tmp[21] = actions[ACTION_DEATHRAY];
    tmp[22] = actions[ACTION_FIRE_RING];
    tmp[23] = actions[ACTION_MANA_DRAIN];
    tmp[24] = actions[ACTION_COUNTERSPELL];
    container[ENT_WIZ_RED] = new DefaultUnit(DefaultEntity(75, 150, 1, 1, STR_ENT_WIZ_RED, STR_WAS_KILLED,
        MATERIAL_FLESH, ENT_IS_PLAYER), 20, 0, 10, 2, 0, tmp, weapons[WEAP_HELLFIRE_STAFF], 10, true);
    //CONJURER GREEN
    tmp[0] = actions[ACTION_STUN];
    tmp[1] = actions[ACTION_FIST_OF_VENGEANCE];
    tmp[2] = actions[ACTION_TOXIC_CLOUD];
    tmp[3] = actions[ACTION_METEOR];
    tmp[4] = actions[ACTION_SLOW];

    tmp[5] = actions[ACTION_HEAL_LESSER];
    tmp[6] = actions[ACTION_PIXIES];
    tmp[7] = actions[ACTION_INVERSION];
    tmp[8] = actions[ACTION_COUNTERSPELL];
    tmp[9] = actions[ACTION_BOW];

    tmp[10] = actions[ACTION_BURN];
    tmp[11] = actions[NO_ACTION];
    tmp[12] = actions[NO_ACTION];
    tmp[13] = actions[NO_ACTION];
    tmp[14] = actions[NO_ACTION];

    tmp[15] = actions[NO_ACTION];
    tmp[16] = actions[NO_ACTION];
    tmp[17] = actions[NO_ACTION];
    tmp[18] = actions[NO_ACTION];
    tmp[19] = actions[NO_ACTION];

    tmp[20] = actions[NO_ACTION];
    tmp[21] = actions[NO_ACTION];
    tmp[22] = actions[NO_ACTION];
    tmp[23] = actions[NO_ACTION];
    tmp[24] = actions[NO_ACTION];
    container[ENT_CONJ_GREEN] = new DefaultUnit(DefaultEntity(100, 125, 1, 2, STR_ENT_CONJ_GREEN, STR_WAS_KILLED,
        MATERIAL_FLESH, ENT_IS_PLAYER), 0, 0, 25, 3, 0, tmp, weapons[WEAP_BOW], 20, false, true);

    //CONJURER ORANGE
    tmp[0] = actions[ACTION_HEAL_LESSER];
    tmp[1] = actions[ACTION_PROTECTION_FIRE];
    tmp[2] = actions[ACTION_PROTECTION_SHOCK];
    tmp[3] = actions[ACTION_PROTECTION_POISON];
    tmp[4] = actions[ACTION_VAMPIRISM];

    tmp[5] = actions[ACTION_HEAL_GREATER];
    tmp[6] = actions[ACTION_CHARM];
    tmp[7] = actions[ACTION_FORCE_OF_NATURE]; //FON
    tmp[8] = actions[ACTION_METEOR];
    tmp[9] = actions[ACTION_SLOW];

    tmp[10] = actions[ACTION_FON_STAFF];//FON STAFF5
    tmp[11] = actions[NO_ACTION];
    tmp[12] = actions[NO_ACTION];
    tmp[13] = actions[NO_ACTION];
    tmp[14] = actions[NO_ACTION];

    tmp[15] = actions[NO_ACTION];
    tmp[16] = actions[NO_ACTION];
    tmp[17] = actions[NO_ACTION];
    tmp[18] = actions[NO_ACTION];
    tmp[19] = actions[NO_ACTION];

    tmp[20] = actions[NO_ACTION];
    tmp[21] = actions[NO_ACTION];
    tmp[22] = actions[NO_ACTION];
    tmp[23] = actions[NO_ACTION];
    tmp[24] = actions[NO_ACTION];
    container[ENT_CONJ_ORANGE] = new DefaultUnit(DefaultEntity(100, 125, 2, 1, STR_ENT_CONJ_ORANGE, STR_WAS_KILLED,
        MATERIAL_FLESH, ENT_IS_PLAYER), 0, 0, 25, 3, 0, tmp, weapons[WEAP_FONSTAFF], 3, false, true);

    //WARRIOR RED
    tmp[0] = actions[ACTION_BERSERKER];
    tmp[1] = actions[ACTION_HARPOON];
    tmp[2] = actions[ACTION_WARCRY];
    tmp[3] = actions[ACTION_FIRESWORD];
    tmp[4] = actions[NO_ACTION];

    tmp[5] = actions[NO_ACTION];
    tmp[6] = actions[NO_ACTION];
    tmp[7] = actions[NO_ACTION];
    tmp[8] = actions[NO_ACTION];
    tmp[9] = actions[NO_ACTION];

    tmp[10] = actions[NO_ACTION];
    tmp[11] = actions[NO_ACTION];
    tmp[12] = actions[NO_ACTION];
    tmp[13] = actions[NO_ACTION];
    tmp[14] = actions[NO_ACTION];

    tmp[15] = actions[NO_ACTION];
    tmp[16] = actions[NO_ACTION];
    tmp[17] = actions[NO_ACTION];
    tmp[18] = actions[NO_ACTION];
    tmp[19] = actions[NO_ACTION];

    tmp[20] = actions[NO_ACTION];
    tmp[21] = actions[NO_ACTION];
    tmp[22] = actions[NO_ACTION];
    tmp[23] = actions[NO_ACTION];
    tmp[24] = actions[NO_ACTION];
    container[ENT_WAR_BLUE] = new DefaultUnit(DefaultEntity(150, 0, 2, 1, STR_ENT_WAR_RED, STR_WAS_KILLED,
        MATERIAL_FLESH, ENT_IS_PLAYER), 25, -50, 75, 4, 0, tmp, weapons[WEAP_FIRESWORD], 0);
    //WARRIOR BLUE
    tmp[0] = actions[ACTION_BERSERKER];
    tmp[1] = actions[ACTION_HARPOON];
    tmp[2] = actions[ACTION_WARCRY];
    tmp[3] = actions[ACTION_LONGSWORD];
    tmp[4] = actions[NO_ACTION];

    tmp[5] = actions[NO_ACTION];
    tmp[6] = actions[NO_ACTION];
    tmp[7] = actions[NO_ACTION];
    tmp[8] = actions[NO_ACTION];
    tmp[9] = actions[NO_ACTION];

    tmp[10] = actions[NO_ACTION];
    tmp[11] = actions[NO_ACTION];
    tmp[12] = actions[NO_ACTION];
    tmp[13] = actions[NO_ACTION];
    tmp[14] = actions[NO_ACTION];

    tmp[15] = actions[NO_ACTION];
    tmp[16] = actions[NO_ACTION];
    tmp[17] = actions[NO_ACTION];
    tmp[18] = actions[NO_ACTION];
    tmp[19] = actions[NO_ACTION];

    tmp[20] = actions[NO_ACTION];
    tmp[21] = actions[NO_ACTION];
    tmp[22] = actions[NO_ACTION];
    tmp[23] = actions[NO_ACTION];
    tmp[24] = actions[NO_ACTION];
    container[ENT_WAR_RED] = new DefaultUnit(DefaultEntity(150, 0, 2, 1, STR_ENT_WAR_BLUE, STR_WAS_KILLED,
        MATERIAL_FLESH, ENT_IS_PLAYER), 0, 50, 25, 4, 0, tmp, weapons[WEAP_LONGSWORD], 0);
}
void Loader::loadDefaultEntities(EntityContainer& container, const ActionContainer& actions, const WeaponContainer& weapons, const CollisionFunctionContainer& collisions,
    const TriggerFunctionContainer& triggers, const DieFunctionContainer& diefuncs)
{
    
    container.resize(Counters::entities);
    container[NO_ENTITY] = new DefaultEntity(0, 0, 0, 0, NO_STRING, NO_STRING, NO_MATERIAL, 0);

    LoadDefaultUnits(container, actions, weapons);

    auto iter = collisions.begin();
    container[ENT_FIREBALL] = new DefaultProjectile(DefaultEntity(4, 0, -1, 0, STR_ENT_FIREBALL, STR_WAS_DESTROYED,
        NO_MATERIAL, ENT_IS_PROJECTILE+ENT_IS_FLYING+ENT_IS_IMMORTAL+ENT_IS_DIE_FUNC+ENT_IS_DISPELLABLE,
        nullptr, nullptr, diefuncs[DIEFUNC_FIREBALL]), 8, collisions[PROJECTILE_FIREBALL], REFLECTEDBY_MAGICSHIELD, COLLIDESWITH_EVERYTHING); 

    container[ENT_FIREBALL_SMALL] = new DefaultProjectile(DefaultEntity(4, 0, -1, 0, STR_ENT_FIREBALL_SMALL, STR_WAS_DESTROYED,
        NO_MATERIAL, ENT_IS_PROJECTILE+ENT_IS_FLYING+ENT_IS_IMMORTAL+ENT_IS_DIE_FUNC+ENT_IS_DISPELLABLE,
        nullptr, nullptr, diefuncs[DIEFUNC_FIREBALLSMALL], container[ENT_FIREBALL]), 4, collisions[PROJECTILE_FIREBALL_SMALL], REFLECTEDBY_MAGICSHIELD, COLLIDESWITH_EVERYTHING);

    container[ENT_ARROW] = new DefaultProjectile(DefaultEntity(4, 0, -1, 0, STR_ENT_ARROW, STR_WAS_DESTROYED,
        MATERIAL_METAL, ENT_IS_PROJECTILE+ENT_IS_FLYING+ENT_IS_IMMORTAL),
        10, collisions[PROJECTILE_ARROW], REFLECTEDBY_MAGICSHIELD+REFLECTEDBY_LONGSWORD, COLLIDESWITH_EVERYTHING);

    container[ENT_SHURIKEN] = new DefaultProjectile(DefaultEntity(4, 0, -1, 0, STR_ENT_SHURIKEN, STR_WAS_DESTROYED,
        MATERIAL_METAL, ENT_IS_PROJECTILE+ENT_IS_FLYING+ENT_IS_IMMORTAL+ENT_IS_OMNIDIRECTIONAL),
        8, collisions[PROJECTILE_SHURIKEN], REFLECTEDBY_MAGICSHIELD+REFLECTEDBY_LONGSWORD, COLLIDESWITH_EVERYTHING);
    //TODO: set a diefunc for fon; if killer differs from itself, divide into small fons
    container[ENT_FON] = new DefaultProjectile(DefaultEntity(4, 0, -1, 0, STR_ENT_FON, STR_WAS_DESTROYED, NO_MATERIAL,
        ENT_IS_PROJECTILE+ENT_IS_FLYING+ENT_IS_IMMORTAL+ENT_IS_AOE_DAMAGING+ENT_IS_OMNIDIRECTIONAL+ENT_IS_DISPELLABLE+ENT_IS_DIE_FUNC,
        new DefaultEntity_AOEDamage(Dmg(DMG_MAGIC, 800, 0), Dmg(DMG_MAGIC, 30, 0)), nullptr, diefuncs[DIEFUNC_FON], nullptr, 8),
        3, collisions[PROJECTILE_FON], REFLECTEDBY_ANY, 0, BEHAVIOUR_NO_AOE_AT_FIRST_CELL);

    container[ENT_FON_SMALL] = new DefaultProjectile(DefaultEntity(4, 0, -1, 0, STR_ENT_FON_SMALL, STR_WAS_DESTROYED, NO_MATERIAL,
        ENT_IS_PROJECTILE+ENT_IS_FLYING+ENT_IS_IMMORTAL+ENT_IS_AOE_DAMAGING +ENT_IS_OMNIDIRECTIONAL+ENT_IS_DISPELLABLE,
        new DefaultEntity_AOEDamage(Dmg(DMG_MAGIC, 20, 0), Dmg())),
        6, collisions[PROJECTILE_FON_SMALL], REFLECTEDBY_ANY, 0);

    container[ENT_ENCHANTBALL] = new DefaultProjectile(DefaultEntity(4, 0, -1, 0, STR_ENT_ENCHANTBALL, STR_WAS_DESTROYED,
        NO_MATERIAL, ENT_IS_PROJECTILE+ENT_IS_FLYING+ENT_IS_IMMORTAL+ENT_IS_HOMING_PROJECTILE+ENT_IS_DISPELLABLE),
        4, collisions[PROJECTILE_ENCHANTBALL], REFLECTEDBY_MAGICSHIELD+REFLECTEDBY_LONGSWORD+REFLECTEDBY_SHIELD, COLLIDESWITH_EVERYTHING);
    container[ENT_HEALBALL] = new DefaultProjectile(DefaultEntity(4, 0, -1, 0, STR_ENT_ENCHANTBALL, STR_WAS_DESTROYED,
        NO_MATERIAL, ENT_IS_PROJECTILE+ENT_IS_FLYING+ENT_IS_IMMORTAL+ENT_IS_HOMING_PROJECTILE+ENT_IS_DISPELLABLE, nullptr, nullptr, nullptr,
        container[ENT_ENCHANTBALL]),
        4, collisions[PROJECTILE_HEALBALL], REFLECTEDBY_MAGICSHIELD+REFLECTEDBY_LONGSWORD+REFLECTEDBY_SHIELD, COLLIDESWITH_EVERYTHING);
    container[ENT_SWAPBALL] = new DefaultProjectile(DefaultEntity(4, 0, -1, 0, STR_ENT_ENCHANTBALL, STR_WAS_DESTROYED,
        NO_MATERIAL, ENT_IS_PROJECTILE+ENT_IS_FLYING+ENT_IS_IMMORTAL+ENT_IS_HOMING_PROJECTILE+ENT_IS_DISPELLABLE, nullptr, nullptr, nullptr,
        container[ENT_ENCHANTBALL]),
        4, collisions[PROJECTILE_SWAPBALL], REFLECTEDBY_MAGICSHIELD+REFLECTEDBY_LONGSWORD+REFLECTEDBY_SHIELD, COLLIDESWITH_EVERYTHING);

    container[ENT_FLAME] = new DefaultEntity(10, 0, -1, 0, STR_ENT_FLAME, STR_WAS_DESTROYED, NO_MATERIAL,
        ENT_IS_IMMOBILE+ENT_IS_IMMORTAL+ENT_IS_OMNIDIRECTIONAL+ENT_IS_AOE_DAMAGING, 
        new DefaultEntity_AOEDamage(Dmg(DMG_FIRE, 10, 0), Dmg(), false));

    container[ENT_BLUEFLAME] = new DefaultEntity(10, 0, -1, 0, STR_ENT_BLUEFLAME, STR_WAS_DESTROYED, NO_MATERIAL,
        ENT_IS_IMMOBILE+ENT_IS_IMMORTAL+ENT_IS_OMNIDIRECTIONAL+ENT_IS_AOE_DAMAGING,
        new DefaultEntity_AOEDamage(Dmg(DMG_FIRE, 10, 0), Dmg(), false, DefaultEntity_AOEDamage::DAMAGES_MANA));

    container[ENT_DISPELLER_CENTER] = new DefaultEntity(5, 200, -1, 0, STR_ENT_DISPELLER_CENTER, STR_WAS_DESTROYED, NO_MATERIAL,
        ENT_IS_IMMOBILE+ENT_IS_IMMORTAL+ENT_IS_UNTARGETABLE+ENT_IS_INVISIBLE+ENT_IS_OMNIDIRECTIONAL);

    container[ENT_DISPELLER_LIGHT] = new DefaultProjectile(DefaultEntity(4, 0, -1, 0, STR_ENT_DISPELLER_LIGHT, STR_WAS_DESTROYED,
        NO_MATERIAL, ENT_IS_PROJECTILE+ENT_IS_IMMORTAL+ENT_IS_OMNIDIRECTIONAL+ENT_IS_TRIGGER, nullptr, new DefaultEntity_Trigger(triggers[TRIGGER_DISPELLER], 0)),
        2, collisions[PROJECTILE_DISPELLER_LIGHT], 0, COLLIDESWITH_EVERYTHING);

    container[ENT_MAGIC_MISSILE] = new DefaultProjectile(DefaultEntity(4, 0, -1, 0, STR_ENT_MAGIC_MISSILE, STR_WAS_DESTROYED,
        NO_MATERIAL, ENT_IS_PROJECTILE+ENT_IS_HOMING_PROJECTILE+ENT_IS_FLYING+ENT_IS_IMMORTAL+ENT_IS_DISPELLABLE,
        nullptr, nullptr, nullptr), 4, collisions[PROJECTILE_MAGIC_MISSILE], REFLECTEDBY_MAGICSHIELD+REFLECTEDBY_LONGSWORD+REFLECTEDBY_SHIELD, COLLIDESWITH_EVERYTHING);

    container[ENT_PIXIE_SWARM] = new DefaultProjectile(DefaultEntity(4, 0, -1, 0, STR_ENT_PIXIE_SWARM, STR_WAS_DESTROYED,
        NO_MATERIAL, ENT_IS_PROJECTILE+ENT_IS_HOMING_PROJECTILE+ENT_IS_FLYING+ENT_IS_IMMORTAL+ENT_IS_OMNIDIRECTIONAL+ENT_IS_DISPELLABLE), 4,
        collisions[PROJECTILE_PIXIE_SWARM], REFLECTEDBY_MAGICSHIELD+REFLECTEDBY_LONGSWORD+REFLECTEDBY_SHIELD, COLLIDESWITH_EVERYTHING);

    container[ENT_FIST_SHADOW] = new DefaultEntity(1, 0, -1, 0, STR_ENT_FIST_SHADOW, STR_WAS_DESTROYED, NO_MATERIAL,
        ENT_IS_IMMOBILE+ENT_IS_INVISIBLE+ENT_IS_IMMORTAL+ENT_IS_UNTARGETABLE+ENT_IS_OMNIDIRECTIONAL+ENT_IS_DIE_FUNC, nullptr, nullptr, diefuncs[DIEFUNC_FISTSHADOW]);

    container[ENT_METEOR_SHADOW] = new DefaultEntity(1, 0, -1, 0, STR_ENT_METEOR_SHADOW, STR_WAS_DESTROYED, NO_MATERIAL,
        ENT_IS_IMMOBILE+ENT_IS_INVISIBLE+ENT_IS_IMMORTAL+ENT_IS_UNTARGETABLE+ENT_IS_OMNIDIRECTIONAL+ENT_IS_DIE_FUNC, nullptr, nullptr, diefuncs[DIEFUNC_METEORSHADOW]);

    container[ENT_FIRERING_FLAME] = new DefaultProjectile(DefaultEntity(4, 0, -1, 0, STR_ENT_FIRERING_FLAME, STR_WAS_DESTROYED,
        NO_MATERIAL, ENT_IS_PROJECTILE+ENT_IS_IMMORTAL+ENT_IS_OMNIDIRECTIONAL+ENT_IS_AOE_DAMAGING, 
        new DefaultEntity_AOEDamage(Dmg(DMG_FIRE, 10, 0), Dmg(), false), nullptr, nullptr,
        container[ENT_FLAME]), 3, collisions[PROJECTILE_FIRERING_FLAME], 0, COLLIDESWITH_EVERYTHING);

    container[ENT_MAGICWALL] = new DefaultEntity(255, 0, 0, 0, STR_ENT_MAGICWALL, STR_WAS_DESTROYED, MATERIAL_METAL,
        ENT_IS_IMMOBILE+ENT_IS_OMNIDIRECTIONAL+ENT_IS_SOLID+ENT_IS_OPAQUE+ENT_IS_TALL+ENT_IS_CONJURED);

    container[ENT_OBELISK] = new DefaultEntity(1, 50, 0, 10, STR_ENT_OBELISK, STR_WAS_DESTROYED, MATERIAL_STONE,
        ENT_IS_IMMOBILE+ENT_IS_IMMORTAL+ENT_IS_OBELISK+ENT_IS_OMNIDIRECTIONAL+ENT_IS_SOLID+ENT_IS_TALL);

    container[ENT_OBELISK_PRIMITIVE] = new DefaultEntity(1, 50, 0, 10, STR_ENT_OBELISK_PRIMITIVE, STR_WAS_DESTROYED, MATERIAL_STONE,
        ENT_IS_IMMOBILE+ENT_IS_IMMORTAL+ENT_IS_OBELISK+ENT_IS_OMNIDIRECTIONAL+ENT_IS_SOLID+ENT_IS_TALL);

    container[ENT_OBELISK_LOTD] = new DefaultEntity(1, 50, 0, 10, STR_ENT_OBELISK_LOTD, STR_WAS_DESTROYED, MATERIAL_STONE,
        ENT_IS_IMMOBILE+ENT_IS_IMMORTAL+ENT_IS_OBELISK+ENT_IS_OMNIDIRECTIONAL+ENT_IS_SOLID+ENT_IS_TALL);

    container[ENT_OBELISK_INVISIBLE] = new DefaultEntity(1, 50, 0, 10, STR_ENT_OBELISK, STR_WAS_DESTROYED, MATERIAL_STONE,
        ENT_IS_IMMOBILE+ENT_IS_IMMORTAL+ENT_IS_INVISIBLE+ENT_IS_OBELISK+ENT_IS_OMNIDIRECTIONAL);

    container[ENT_TOXIC_CLOUD] = new DefaultEntity(4, 0, -1, 0, STR_ENT_TOXIC_CLOUD, STR_WAS_DESTROYED, NO_MATERIAL,
        ENT_IS_IMMOBILE+ENT_IS_FLYING+ENT_IS_OMNIDIRECTIONAL+ENT_IS_TRIGGER+ENT_IS_IMMORTAL,
        nullptr, new DefaultEntity_Trigger(triggers[TRIGGER_TOXIC_CLOUD], ENT_IS_UNIT));

    container[ENT_TELEPORT_PENTAGRAM] = new DefaultEntity(0, 0, 0, 0, NO_STRING, STR_WAS_DESTROYED, NO_MATERIAL, 0);
    container[ENT_DOOR_WOODEN] = new DefaultEntity(0, 0, 0, 0, NO_STRING, STR_WAS_DESTROYED, NO_MATERIAL, 0);
    container[ENT_DOOR_JAIL] = new DefaultEntity(0, 0, 0, 0, NO_STRING, STR_WAS_DESTROYED, NO_MATERIAL, 0);
}
void Entity::GetDamage(Dmg damage) {
    hp = max(0, hp-damage.value);
}
void Unit::GetDamage(Dmg damage) {
    if (!(damage.flags & DFLAG_IGNORES_ALL)) {
        switch (damage.type) {
        case DMG_FIRE:
            damage.value = ceil(static_cast<double>(damage.value) * (100 - FireResist()) / 100);
            break;
        case DMG_SHOCK:
            damage.value = ceil(static_cast<double>(damage.value) * (100 - ShockResist()) / 100);
            break;
        case DMG_VENOM:
            damage.value = ceil(static_cast<double>(damage.value) * (100 - VenomResist()) / 100);
            break;
        case DMG_PHYS:
            damage.value = ceil(static_cast<double>(damage.value) * (100 - Armor()) / 100);
            break;
        case DMG_BLUDGEON:
            damage.value = ceil(static_cast<double>(damage.value) * (100 - Armor()/2) / 100);
            break;
        };
    }
    if (!(damage.flags & (DFLAG_IGNORES_FF|DFLAG_IGNORES_ALL)) && IsEnchanted(ENCHANT_FORCE_FIELD)) {
        damage.value /= 2;
        if (damage.value >= hp) {
            Disenchant(ENCHANT_FORCE_FIELD);
            hp = 1;
        } else {
            enchants[ENCHANT_FORCE_FIELD] = max(0, enchants[ENCHANT_FORCE_FIELD] - damage.value);
            hp = max(1, hp - damage.value);
        }
    } else {
        hp -= damage.value;
    }
    
}
void Entity::Kill(Battle* battle, Entity* killer) {
    is_dead = true;
    if (GetPrototype().diefunc) {
        (battle->*GetPrototype().diefunc)(this, killer);
    }
}

void Unit::EnchantWith(const Enchant& enchant, int time) {
    enumEnchants id = static_cast<enumEnchants>(enchant.id);
    if (!IsEnchanted(id)) {
        switch (id){
        case ENCHANT_SLOW:
        case ENCHANT_HASTE:
            movepoints += (Speedtest(id) - Speed());
            break;
        case ENCHANT_STUN:
            movepoints = 0;
            break;
        case NO_ENCHANT:
            return;
        };
    }

    //enchant.sound_on.PlaySound();
    if (time) {
        enchants[id] = time;
    } else {
        enchants[id] = enchant.time_to_last;
    }
}

void Entity::Turn(const Direction dir) { this->dir = dir; }
void Unit::Turn(const Direction dir) { this->dir = dir; }

void Entity::StartTurn(){
    hp += GetPrototype().reghp;
    mp += GetPrototype().regmp;
    hp = min(hp, MaxHP());
    mp = min(mp, MaxMP());
}
void Entity::EndTurn() { lastcoor = coor; }
void Unit::StartTurn(){
    hp += GetPrototype().reghp;
    mp += GetPrototype().regmp;
    hp = min(hp, MaxHP());
    mp = min(mp, MaxMP());
    movepoints = Speed();
    if (!IsBusy()) {
        action_points = 2;
    }
    cooldowns.DecreaseAll();
}
void Unit::EndTurn() {
    lastcoor = coor;
    bool set_block = !IsBusy();
    for (int i = 0; i < Counters::Channeling_actions; ++i) {
        if (!channelings.IsIteratorInside(channelings.GetIterator())) { set_block = false; }       //if any channel actions are made, unit can't block
    }
    if (set_block) {
        switch (GetPrototype().weapon->block_quality) {
        case BLOCK_STAFF:
        case BLOCK_LONGSWORD:
        case BLOCK_QUICKSHIELD:
            if (action_points != 0 && movepoints != 0) {
                is_in_blocking_state = true;
            }
            break;
        case BLOCK_SHIELD:
            if (action_points == 2 && movepoints == Speed()) {  //can only block with shield if did nothing
                is_in_blocking_state = true;
            }
            break;
        };
    }
}

void Unit::StopCSpells() {
    channelings.DeleteAll();
}

Entity* Entity::Clone() const {
    return new Entity(*this);
}
Entity* Unit::Clone() const {
    return new Unit(*this);
}
Entity* Projectile::Clone() const {
    return new Projectile(*this);
}

void Entity::Move(const CoordI& dest, const Direction dir) {
    lastcoor = coor;
    coor = dest;
    if (dir != NO_DIRECTION) {
        this->dir = dir;
    }
}

ptfTrigger Entity::MyTrigger() const {
    return GetPrototype().MyTrigger();
}
bool Entity::TriggerFlagsMaskTest(Entity* EntityToTest) const {
    return GetPrototype().TriggerFlagsMaskTest(EntityToTest->GetPrototype());
}

void Unit::Move(const CoordI& dest, const Direction dir) {
    Entity::Move(dest, dir);
}
void Unit::DrainMana(Entity *source) {
    int mana_available = min(source->mp, mana_drain_per_turn);
    int mana_needed = MaxMP() - mp;
    int mana = min(mana_needed, mana_available);
    mp += mana;
    source->mp -= mana;

    int mana_per_ammo = GetPrototype().weapon->mana_per_ammo;
    if (source->mp != 0 &&
        mana_per_ammo != 0 && 
        GetPrototype().ammo != ammo) {
            
        mana_available = min(source->mp, mana_drain_per_turn);
        mana_needed = mana_per_ammo * (GetPrototype().ammo - ammo) - ammo_manabuffer;
        mana = min(mana_needed, mana_available);

        ammo += (mana + ammo_manabuffer) / mana_per_ammo;
        ammo_manabuffer = (mana + ammo_manabuffer) % mana_per_ammo;
        source->mp -= mana;
    }
}

ErrorBase* Entity::CheckConsistency() const
{
    if (!is_dead && (hp <= 0)) {
        return new Errors::EntityAliveWithNegativeHealth(this);
    }
    for (auto it = subs.begin(); it != subs.end(); ++it) {
        if ((*it)->object_owner != this) {
            return new Errors::EntityFakeOwner(*it, (*it)->object_owner, this);
        }
    }
    if (object_owner) {
        bool hasnt_proper_sub = true;
        for (auto it = object_owner->subs.begin(); it != object_owner->subs.end(); ++it) {
            if (this == *it) {
                hasnt_proper_sub = false;
                break;
            }
        }
        if (hasnt_proper_sub) {
            return new Errors::EntityFakeSub(object_owner, this);
        }
    }
    return nullptr;
}
ErrorBase* Unit::CheckConsistency() const
{
    ErrorBase* tmp = nullptr;
    tmp = Entity::CheckConsistency();
    if (tmp) { return tmp; }
    for (int i = 0; i < Counters::enchants; ++i) {
        if (enchants[i] < 0) {
            return new Errors::UnitNegativeEnchant(this, static_cast<enumEnchants>(i));
        }
    }
    for (auto it = cooldowns.GetIterator(); cooldowns.IsIteratorInside(it); ++it) {
        if (it->second < 0) {
            return new Errors::UnitNegativeCooldown(this, it->first);
        }
    }
    if (movepoints > Speed()) {
        return new Errors::UnitMovepointsMoreThanSpeed(this);
    }
    for (auto it = channelings.GetIterator(); channelings.IsIteratorInside(it); ++it) {
        if ((*it).second.target == nullptr) {
            return new Errors::UnitNullCSpellTarget(this, (*it).first);
        }
    }
    return nullptr;
}

ErrorBase* CommonProjectile::CheckConsistency() const
{
    ErrorBase* tmp = nullptr;
    tmp = Entity::CheckConsistency();
    if (tmp) { return tmp; }
    if (Coor() != coor) {
        return new Errors::ProjectileMismatchingCoors(this);
    }
    return nullptr;
}
ErrorBase* HomingProjectile::CheckConsistency() const
{
    ErrorBase* tmp = nullptr;
    tmp = Entity::CheckConsistency();
    if (tmp) { return tmp; }
    if (Coor() != coor) {
        return new Errors::ProjectileMismatchingCoors(this);
    }
    return nullptr;
}
ErrorBase* EnchantProjectile::CheckConsistency() const
{
    ErrorBase* tmp = nullptr;
    tmp = HomingProjectile::CheckConsistency();
    if (tmp) { return tmp; }
    return nullptr;
}