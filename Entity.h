#pragma once
class Action;
class Enchant;
class Spell;
class Renderer;
#include "Strings.h"
#include "Geometry.h"
#include "Constants.h"
#include <string>
#include <list>
#include <cstdlib>
using std::vector;
using std::list;
using std::pair;

enum DefEntFlags
{
    ENT_IS_UNIT = 1,
    ENT_IS_PROJECTILE = 2,
    ENT_IS_SOLID = 4,
    ENT_IS_TALL = 8,
    ENT_IS_OPAQUE = 16,
    ENT_IS_UNDEAD = 32,
    ENT_IS_OBELISK = 64,
    ENT_IS_DOOR = 128,
    ENT_IS_IMMORTAL = 256,
    ENT_IS_IMMOBILE = 512,
    ENT_IS_FLYING = 1024,
    ENT_IS_INVISIBLE = 2048,
    ENT_IS_CHARMABLE = 4096,
    ENT_IS_UNTARGETABLE = 8192,
    ENT_IS_OMNIDIRECTIONAL = 16384,
    ENT_IS_HOMING_PROJECTILE = 32768,
    ENT_IS_AOE_DAMAGING = 65536,
    ENT_IS_TRIGGER = 131072,
    ENT_IS_DIE_FUNC = 262144,
    ENT_IS_DISPELLABLE = 524288,
    ENT_IS_CONJURED = 1048576
};
#define ENT_IS_PLAYER (ENT_IS_UNIT + ENT_IS_SOLID + ENT_IS_TALL)
enum ProjectileReflectFlags
{
    REFLECTEDBY_LONGSWORD = 1,
    REFLECTEDBY_SHIELD = 2,
    REFLECTEDBY_MAGICSHIELD = 4,
    REFLECTEDBY_WALL = 8,
};
#define REFLECTEDBY_ANY (REFLECTEDBY_LONGSWORD+REFLECTEDBY_MAGICSHIELD+REFLECTEDBY_SHIELD+REFLECTEDBY_WALL)
enum ProjectileCollisionFlags
{
    COLLIDESWITH_WALLS = 1,
    COLLIDESWITH_UNITS = 2,
    COLLIDESWITH_OBJECTS = 4
};
#define COLLIDESWITH_EVERYTHING (COLLIDESWITH_OBJECTS+COLLIDESWITH_UNITS+COLLIDESWITH_WALLS)
enum BehaviourFlags
{
    BEHAVIOUR_NO_AOE_AT_FIRST_CELL = 1
};

struct Weapon
{
    Action* action;
    BLOCK_QUALITY block_quality;
    DmgType type;
    int mana_per_ammo; //how much mana is needed per 1 ammo
    struct WeaponEnchant
    {
        enumWeapontEnchants type;
        int power;
        WeaponEnchant(enumWeapontEnchants type = NO_CHANT, int power = 0): type(type), power(power) {}
    } Enchants[max_weapon_enchants];
    int BaseDamage;
    Weapon(Action* action, BLOCK_QUALITY blocktype, int manacost, int basedamage = 0, DmgType type = DmgType::DMG_FIRE, vector<WeaponEnchant> chants = vector<WeaponEnchant>());
};

struct DefaultEntity_AOEDamage
{
    Dmg aura_damage, aura_radius_damage;
    enum DamageTarget
    {
        DAMAGES_HEALTH,
        DAMAGES_MANA,
        DAMAGES_BOTH,
        DAMAGES_NONE
    } type;
    bool affects_fliers;
    DefaultEntity_AOEDamage(Dmg aura_damage, Dmg aura_radius_damage, bool AffectsFliers = true, DamageTarget aoe_type = DAMAGES_HEALTH):
        aura_damage(aura_damage), aura_radius_damage(aura_radius_damage), type(aoe_type), affects_fliers(AffectsFliers){}
};
struct DefaultEntity_Trigger
{
    ptfTrigger trigger;
    long entity_activating_flags;
    DefaultEntity_Trigger(ptfTrigger trigger, long entity_activating_flags): 
        trigger(trigger), entity_activating_flags(entity_activating_flags) {}
};

class DefaultEntity
{
protected:
    int hp, mp, reghp, regmp;
    enumStrings name, log_die;
    Sound* snd_hurt;
    Sound* snd_die;
    enumMaterial Material;
    long flags;
    DefaultEntity *shares_textures_with;
    int animation_frames;
    std::vector<TextureHandler> textures;
    std::vector<SoundHandler> sounds;
    DefaultEntity_AOEDamage* aoe;
    DefaultEntity_Trigger* trigger;
    ptfDie diefunc;
public:
    DefaultEntity(int hp, int mp, int reghp, int regmp, enumStrings name, enumStrings log_die,
        enumMaterial Material, long flags,
        DefaultEntity_AOEDamage* aoe = nullptr, DefaultEntity_Trigger* trigger = nullptr, ptfDie diefunc = nullptr,
        DefaultEntity *shares_textures_with  = nullptr, int animation_frames = 0);
    bool Flag(DefEntFlags flag) const { return ((flags & flag) > 0); }
    enumStrings Name() const { return name; }
    
    Dmg AuraDamage() const;
    Dmg AuraRadiusDamage() const;
    bool AuraDamageAffectsFliers() const;
    DefaultEntity_AOEDamage::DamageTarget AuraDamageType() const;
    ptfTrigger MyTrigger() const;
    bool TriggerFlagsMaskTest(const DefaultEntity& EntityToTest) const;
    virtual Entity* Create(const CoordI& coor, Team team, Direction dir = NO_DIRECTION) const;
    virtual bool BlocksMoving(const DefaultEntity* ent, bool UnitJumps = false) const;
    int getAnimationFrames() const { return animation_frames; }
    void SetTextures(const std::vector<TextureHandler>& textures) { this->textures = textures; }
    void SetSounds(const std::vector<SoundHandler>& sounds) { this->sounds = sounds; }
    TextureHandler MyTexture() const {
        if (textures.empty()) {
            return EMPTY_TEXTURE;
        }
        if (Flag(ENT_IS_OMNIDIRECTIONAL)) {
            if (Flag(ENT_IS_OBELISK)) {
                return textures[obelisk_picture_states-1];
            } else {
                return textures[0];
            }
        } else {
            return textures[0];
        }
    }
    friend class Entity;
    DefaultEntity(const DefaultEntity& ent) {
        *this = ent;
        if (ent.aoe) { aoe = new DefaultEntity_AOEDamage(*ent.aoe); }
        if (ent.trigger) { trigger = new DefaultEntity_Trigger(*ent.trigger); }
    }
    virtual ~DefaultEntity() {
        if (aoe) { delete aoe; }
        if (trigger) { delete trigger; }
    }
};
class Entity
{
protected:
    int hp, mp;
    long flags;
    CoordI coor, lastcoor;
    Direction dir;
    const DefaultEntity& prototype;
    Team team;
    Entity *object_owner;
    vector<Entity *> subs;
    bool is_dead, is_paralyzed;
public:
    virtual ErrorBase* CheckConsistency() const;
    const DefaultEntity& GetPrototype() const { return prototype; }
    virtual ~Entity() {}
    Entity(const DefaultEntity& prototype, const CoordI& coor, Team team, Direction dir = NO_DIRECTION);

    virtual bool BlocksMoving(const Entity* ent, bool UnitJumps = false) { return GetPrototype().BlocksMoving(&ent->GetPrototype()); }

    virtual void Move(const CoordI& dest, const Direction dir = NO_DIRECTION);
    virtual void Turn(const Direction dir);
    virtual void GetDamage(Dmg damage);
    void Kill(Battle* battle, Entity* killer);

    virtual void StartTurn();
    virtual void EndTurn();
    virtual void DrainMana(Entity *source) {}

    virtual void EnchantWith(const Enchant& enchant, int time = 0) {}
    virtual void EnchantReduce(const enumEnchants id) {}
    virtual bool IsEnchanted(const enumEnchants id) const { return false; }
    virtual void Disenchant(const enumEnchants id) {}
    virtual int EnchantTime(const enumEnchants id) const { return 0; }

    virtual void RestoreHP(const int value) { hp = max(0, min(hp + value, MaxHP())); }
    virtual void RestoreMP(const int value) { mp = max(0, min(mp + value, MaxMP())); }
    virtual void BurnMP(const int value) { RestoreMP(-value); }

    virtual const Weapon* MyWeapon() const { return nullptr; }
    virtual int Speed() const { return 0; }
    virtual int CharmSize() const { return 0; }
    virtual int getActionPoints() const {
        return 0;
    }
    virtual const Action* Spellbar(int ID) const { return nullptr; }

    virtual void StopCSpells() {}

    bool Flag(DefEntFlags flag) const { return prototype.Flag(flag); }
    int HP() const { return hp; }
    int MP() const { return mp; }
    Team Team() const { return team; }
    enumStrings Name() const { return GetPrototype().Name(); }
    enumStrings LogDie() const { return GetPrototype().log_die; }
    int MaxHP() const { return GetPrototype().hp; }
    int MaxMP() const { return GetPrototype().mp; }
    virtual CoordI Coor() const { return coor; }
    virtual CoordD ExCoor() const { return CoordD(0.5, 0.5) + coor; }
    virtual Direction Dir() const { return dir; }
    virtual Direction LastMoveDirection() const {
        if (lastcoor != coor) {
            return lastcoor.getDirection(coor);
        } else {
            return dir;
        }
    }
    virtual int MovePoints() const { return 0; }
    virtual int Ammo() const { return 0; }
    virtual bool CanUseTrap() const { return false; }
    virtual bool CanUseBomber() const { return false; }

    Dmg AuraDamage() const { return GetPrototype().AuraDamage(); }
    Dmg AuraRadiusDamage() const { return GetPrototype().AuraRadiusDamage(); }
    DefaultEntity_AOEDamage::DamageTarget AuraDamageType() const { return GetPrototype().AuraDamageType(); }
    bool AuraDamageAffectsFliers() const { return GetPrototype().AuraDamageAffectsFliers(); }
    ptfTrigger MyTrigger() const;
    bool TriggerFlagsMaskTest(Entity* EntityToTest) const;

    TextureHandler MyTexture() const {
        if (Flag(ENT_IS_OMNIDIRECTIONAL)) {
            if (Flag(ENT_IS_OBELISK)) {
                return GetPrototype().textures[min(obelisk_picture_states - 1, mp / (MaxMP() / obelisk_picture_states))];
            } else {
                return GetPrototype().textures[0];
            }
        } else {
            return GetPrototype().textures[Dir() - 1];
        }
    }
    virtual SoundHandler SoundDamaged() const { return GetPrototype().sounds[1]; }
    virtual SoundHandler SoundDied() const { return GetPrototype().sounds[0]; }
    virtual SoundHandler SoundTaunt() const { return EMPTY_SOUND; }
    virtual SoundHandler SoundMove() const { return EMPTY_SOUND; }

    //returns itself, if it has no owner; otherwise returns the "deepest" owner of the owners
    Entity* GetTrueOwner() {
        Entity* tmp = this;
        while (tmp->object_owner) {
            tmp = tmp->object_owner;
        }
        return tmp;
    }
    bool IsDead() const { return is_dead; }
    virtual Entity* Clone() const;
    friend Action;
    friend Spell;
    friend Ability;
    friend Battle;

    friend class Unit;
};

class DefaultUnit: public DefaultEntity
{
    int resist_fire, resist_shock, armor;
    int speed, charm_size;
    const vector<Action *> Actions;
    Weapon* weapon;
    int ammo;
    bool can_trap, can_bomber;
public:
    DefaultUnit(const DefaultEntity& defent,
        int fireresist, int shockresist, int armor, int speed, int charmsize,
        vector<Action *>& actions_, Weapon* weapon, int ammo, bool can_trap = false, bool can_bomber = false);
    friend class Unit;
    virtual Entity* Create(const CoordI& coor, Team team, Direction dir = NO_DIRECTION) const;
    virtual ~DefaultUnit(){}
};
class Unit: public Entity
{//TODO: remake channel actions all-slot array into "by demand array" with direct reference to action;
protected:
    int enchants[Counters::enchants];
    
    int movepoints, ammo, ammo_manabuffer;
    bool is_in_blocking_state;
    bool is_jumping;
    int action_points;  //0 = cant, 1 = can do one quick, 2 = can do one full or 2 quick
    struct L_Action
    {
        int time;
        CoordI coor;
        const Action *action;
        void Nullify() { time = 0; coor = CoordI(0,0); action = nullptr; }
        L_Action(int time, CoordI& coor, const Action *action): time(time), coor(coor), action(action) {}
        L_Action(): time(0), coor(Coord0), action(nullptr) {}
    } long_action; //currently under casting
    
    int FireResist() { return min(GetPrototype().resist_fire + (IsEnchanted(ENCHANT_PROTECTION_FIRE) ? 50 : 0), 100); }
    int ShockResist() { return min(GetPrototype().resist_shock + (IsEnchanted(ENCHANT_PROTECTION_SHOCK) ? 50 : 0), 100); }
    int VenomResist() { return min(IsEnchanted(ENCHANT_PROTECTION_POISON) ? 50 : 0, 100); }
    int Armor() { return GetPrototype().armor; }
public:
    class UnitCooldowns
    {
        list<pair<const Action*, int>> cds;
    public:
        typedef list<pair<const Action*, int>>::const_iterator const_iterator;
        int FindCooldown(const Action *action) const {
            for (auto it = cds.begin(); it != cds.end(); ++it) {
                if ((*it).first == action) {
                    return (*it).second;
                }
            }
            return 0;
        }
        void SetCooldown(const Action *action, int cooldown) {
            for (auto it = cds.begin(); it != cds.end(); ++it) {
                if ((*it).first == action) {
                    (*it).second = cooldown;
                }
            }
            cds.push_back(make_pair(action, cooldown));
        }
        void DeleteCooldown(const Action *action) {
            for (auto it = cds.begin(); it != cds.end(); ++it) {
                if ((*it).first == action) {
                    cds.erase(it);
                    return;
                }
            }
        }
        const_iterator GetIterator() const { return cds.cbegin(); }
        bool IsIteratorInside(const const_iterator& it) const { return (it != cds.cend()); }
        void DecreaseAll() {
            for (auto it = cds.begin(); it != cds.end(); ++it) {
                (*it).second--;
            }
        }
    } cooldowns;
    struct ChannelActionData
    {
        Entity* target;
        CoordI coor_target;
        int metadata;
    };
    class Channelings
    {
        list<pair<const Action*, ChannelActionData>> chnls;
    public:
        typedef list<pair<const Action*, ChannelActionData>>::const_iterator const_iterator;
        //returns data linked with specified action;
        ChannelActionData* FindChannelingAction(const Action *action) {
            for (auto it = chnls.begin(); it != chnls.end(); ++it) {
                if ((*it).first == action) {
                    return &((*it).second);
                }
            }
            return nullptr;
        }
        ChannelActionData* AddChannelingAction(const Action *action) {
            chnls.push_front(make_pair(action, ChannelActionData()));
            return (&(*chnls.begin()).second); //return addr of newly added channel;
        }
        void DeleteChannelingAction(const Action *action) {
            for (auto it = chnls.begin(); it != chnls.end(); ++it) {
                if ((*it).first == action) {
                    chnls.erase(it);
                    return;
                }
            }
        }
        void DeleteAll() {
            chnls.clear();
        }
        const_iterator GetIterator() const { return chnls.cbegin(); }
        bool IsIteratorInside(const const_iterator& it) const { return (it != chnls.cend());
        }
    } channelings;

    virtual ErrorBase* CheckConsistency() const;
    const DefaultUnit& GetPrototype() const {return dynamic_cast<const DefaultUnit&>(Entity::GetPrototype());}
    virtual ~Unit(){}
    Unit(const DefaultUnit& prototype, const CoordI& coor, ::Team team, Direction dir = NO_DIRECTION);
    virtual void GetDamage(Dmg damage);
    virtual void EnchantWith(const Enchant& enchant, int time = 0);
    virtual int Speed() const {
        if (is_paralyzed || IsEnchanted(ENCHANT_STUN)) { return 0; }
        return max(GetPrototype().speed + 
            ((IsEnchanted(ENCHANT_HASTE))? 2 : 0 ) -
            ((IsEnchanted(ENCHANT_SLOW))? 2 : 0 ), 1);
    }
    virtual int getActionPoints() const {
        if (is_paralyzed) {
            return 0;
        } else {
            return action_points;
        }
    }
    virtual int CharmSize() const { return GetPrototype().charm_size; }
    //returns the speed the Unit would have, if it was enchanted with this
    int Speedtest(const enumEnchants id) {
        if (IsEnchanted(ENCHANT_STUN)) { return 0; }
        if (id == ENCHANT_HASTE) {
            return max(GetPrototype().speed + 2 -
                ((IsEnchanted(ENCHANT_SLOW))? 2 : 0 ), 1);
        } else {
            return max(GetPrototype().speed + 
                ((IsEnchanted(ENCHANT_HASTE))? 2 : 0 ) - 2, 1);
        }
    }
    virtual void StartTurn();
    virtual void EndTurn();
    virtual void Turn(const Direction dir);
    virtual void Move(const CoordI& dest, const Direction dir = NO_DIRECTION);
    virtual void DrainMana(Entity *source);

    virtual void RestoreHP(const int value) { hp = min(hp + value, MaxHP()); }
    virtual void RestoreMP(const int value) { mp = min(mp + value, MaxMP()); }
    virtual void BurnMP(const int value) { mp = max(mp - value, 0); }

    virtual void StopCSpells();
    virtual void EnchantReduce(const enumEnchants id) { enchants[id]--; }
    virtual bool IsEnchanted(const enumEnchants id) const { return (enchants[id] > 0); }
    virtual void Disenchant(const enumEnchants id) { enchants[id] = 0; }
    virtual int EnchantTime(const enumEnchants id) const { return enchants[id]; }
    bool IsBusy() const { return long_action.time != 0; }
    bool ManaNotFull() { return mp != MaxMP(); }
    virtual int MovePoints() const { return movepoints; }
    virtual int Ammo() const { return ammo; }
    int getWeaponManaBuffer() const { return ammo_manabuffer; }
    bool IsJumping() const { return is_jumping; }
    BLOCK_QUALITY BlockType() const { return GetPrototype().weapon->block_quality; }
    bool isBlocking() const { return is_in_blocking_state; }
    L_Action getLAction() const { return long_action; }
    virtual const Action* Spellbar(int ID) const { return GetPrototype().Actions[ID-1]; }
    virtual const Weapon* MyWeapon() const { return GetPrototype().weapon; }
    virtual Entity* Clone() const;
    virtual bool CanUseTrap() const { return GetPrototype().can_trap; }
    virtual bool CanUseBomber() const { return GetPrototype().can_bomber; }

    friend Action;
    friend Spell;
    friend Ability;
    friend ChannelingSpell;

    friend Battle;
    friend Entity;
    friend Renderer;
};

#pragma warning(disable:4800)  //casting long into bool
class DefaultProjectile: public DefaultEntity
{
protected:
    int speed;
    ptfCollision Collide; //projectile, obstacle coor, direction
    long reflectflags;
    long collideflags;
    long behaviourflags;
public:
    DefaultProjectile(const DefaultEntity& defent, int speed, ptfCollision Collidefunc, 
        long reflectflag, long collideflags, long behaviourflags = 0);
    friend class Projectile;
    friend class CommonProjectile;
    friend class HomingProjectile;
    bool CollisionFlag(ProjectileCollisionFlags flag) const { return collideflags & flag; }
    bool ReflectionFlag(ProjectileReflectFlags flag) const { return reflectflags & flag; }
    bool BehaviourFlag(BehaviourFlags flag) const { return behaviourflags & flag; }
    virtual bool BlocksMoving(const DefaultEntity* ent, bool UnitJumps = false) const;

    virtual Entity* Create(const CoordI& coor, Team team, Entity* source, Angle angle) const;
    virtual Entity* Create(const CoordI& coor, Team team, Entity* target, Entity* source, enumEnchants id) const;
    virtual Entity* Create(const CoordI& coor, Team team, Entity* target, Entity* source) const;
    virtual ~DefaultProjectile(){}
};
//this class is abstract pretty much
class Projectile: public Entity
{
protected:
public:
    virtual ErrorBase* CheckConsistency() const { return nullptr; }
    const DefaultProjectile& GetPrototype() const { return dynamic_cast<const DefaultProjectile&> (Entity::GetPrototype()); }
    virtual ~Projectile() {}
    virtual int Speed() { return 0; }
    bool Collision(Battle* battle, const CoordI& coor, const Direction dir) { return (battle->*GetPrototype().Collide)(this, coor, dir); } 
    bool CollisionFlag(ProjectileCollisionFlags flag) const { return GetPrototype().CollisionFlag(flag); }
    bool ReflectionFlag(ProjectileReflectFlags flag) const { return GetPrototype().ReflectionFlag(flag); }
    bool BehaviourFlag(BehaviourFlags flag) const { return GetPrototype().BehaviourFlag(flag); }
    virtual bool BlocksMoving(const Entity* ent, bool UnitJumps = false) { return GetPrototype().BlocksMoving(&ent->GetPrototype()); }
    virtual Entity* Clone() const;
    Projectile(const Entity& entity);
    friend Battle;
};
class CommonProjectile: public Projectile
{
protected:
    CoordD ex_coor;
    Angle angle;
    int speed;
public:
    virtual ErrorBase* CheckConsistency() const;
    const DefaultProjectile& GetPrototype() const { return Projectile::GetPrototype(); }
    virtual ~CommonProjectile() {}
    CommonProjectile(const DefaultProjectile& prototype, const CoordI& coor, ::Team team, Entity* source, ::Angle angle);
    virtual int Speed() const { return speed; }
    //virtual vInt Coor() const { return (ex_coor); }
    virtual CoordD ExCoor() const { return ex_coor; }
    virtual Direction Dir() const { return angle.getDirection(); }
    friend Battle;
};
class HomingProjectile: public Projectile
{
protected:
    CoordD ex_coor;
    Entity* target;
    int speed;
public:
    virtual ErrorBase* CheckConsistency() const;
    const DefaultProjectile& GetPrototype() const { return dynamic_cast<const DefaultProjectile&>(Entity::GetPrototype()); }
    virtual ~HomingProjectile() {}
    HomingProjectile(const DefaultProjectile& prototype, const CoordI& coor, ::Team team, Entity* source, Entity* target);
    virtual int Speed() const { return speed; }
    virtual CoordD ExCoor() const { return ex_coor; }
    virtual Direction Dir() const {
        Direction tmp =  Coor().getDirection(target->Coor()); 
        if (tmp != NO_DIRECTION) {
            return tmp;
        } else {
            return dir;
        }
    }
    friend Battle;
};
class EnchantProjectile: public HomingProjectile
{
protected:
    enumEnchants enchant_id;
public:
    virtual ErrorBase* CheckConsistency() const;
    const DefaultProjectile& GetPrototype() const { return dynamic_cast<const DefaultProjectile&>(Entity::GetPrototype()); }
    virtual ~EnchantProjectile(){}
    EnchantProjectile(const HomingProjectile& homing_base, enumEnchants enchant_id);
    friend Battle;
};

namespace Loader{
    using namespace ContainerDefs;
    void loadWeapons(WeaponContainer& container, ActionContainer& actions);
    void loadDefaultEntities(EntityContainer& container, const ActionContainer& actions, const WeaponContainer& weapons, const CollisionFunctionContainer& collisions,
        const TriggerFunctionContainer& triggers, const DieFunctionContainer& diefuncs);
};
