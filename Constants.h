#pragma once
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#include "Geometry.h"
#include <vector>
typedef int TextureHandler;
typedef int SoundHandler;
const TextureHandler EMPTY_TEXTURE = 0;
const SoundHandler EMPTY_SOUND = 0;
using std::string;
const int actions_per_set = 5, actionsets_count = 5;
const CoordI picsize = CoordI(50, 50);
const CoordI texturesize = CoordI(64, 64);
const CoordD picscale = (CoordD)picsize / texturesize;
const int charms_per_row = 5;
const int max_charms = 15;

const int objectlist_mini_elements_per_row = 3;
const int objectlist_mini_size = 21;

const int projectile_steps_per_speed = 15;
const int obelisk_picture_states = 5;
const int max_flags = 6;

const CoordI field_window_size = CoordI(10, 10);
namespace Counters
{
    const int gestures = 9;
    const int actions = 54;
    const int enchants = 15;  //must be 18
    const int weapon_enchants = 11;
    const int cooldowns = 3;
    const int entities = 35;
    const int weapons = 11;
    const int sounds = 90;
    const int strings = 137;
    const int Channeling_actions = 8;
    const int projectiles = 13;
    const int triggers = 3;
    const int diefuncs = 5;
    const int directions = 8;
    const int button_types = 2;
    const int radiobutton_types = 2;
    const int tiles = 4;
    const int walls = 5;
    const int map_editor_modes = 8;
};
const string common_image_extension = ".bmp";
const string common_sound_extension = ".wav";
namespace Paths 
{
    const string PicturesPath = "\\data\\pictures\\";
    const string NoIcon = "common\\no icon" + common_image_extension;
    const string MoveArrow = "common\\Arrow";
    const string SelectionFrame = "common\\Selection" + common_image_extension;
    const string EntitiesPictures = "entities\\";
    const string ActionsIcons = "actions icons\\";
    const string ActionsActiveIcons = "actions icons\\active\\";
    const string EnchantsPictures = "enchants\\";
    const string EnchantsIcons = "enchants\\icons\\";
    const string WallsPictures = "walls\\";
    const string TilesPictures = "tiles\\";

    const string ActionSounds = "sounds\\actions\\";
    const string EntityDieSounds = "sounds\\entities\\die\\";
    const string EntityMoveSounds = "sounds\\entities\\move\\";
    const string EntityDamageSounds = "sounds\\entities\\damaged\\";
    const string EnchantOffSounds = "sounds\\enchants\\off\\";
    const string EnchantOnSounds = "sounds\\enchants\\on\\";
    const string GestureSounds = "sounds\\gestures\\";

    const string ActionBar_base = "actionbar\\base" + common_image_extension;
    const string ActionBar_title = "actionbar\\title" + common_image_extension;
    const string ActionBar_left_blank = "actionbar\\left" + common_image_extension;
    const string ActionBar_right_blank = "actionbar\\right" + common_image_extension;
    const string ActionBar_nugget = "actionbar\\nugget";
    const string ActionBar_right_trap = "actionbar\\trap" + common_image_extension;
    const string ActionBar_right_bomber = "actionbar\\bomber" + common_image_extension;
    const string ActionBar_right_trap_active = "actionbar\\active\\trap" + common_image_extension;
    const string ActionBar_right_bomber_active = "actionbar\\active\\bomber" + common_image_extension;
    const string ActionBar_left_scroll = "actionbar\\spellbar_scroll" + common_image_extension;
    const string ActionBar_left_scroll_active_up = "actionbar\\active\\spellbar_scroll_up" + common_image_extension;
    const string ActionBar_left_scroll_active_down = "actionbar\\active\\spellbar_scroll_down" + common_image_extension;
    const string ActionBar_spellbook = "actionbar\\spellbook" + common_image_extension;
    const string ActionBar_spellbook_active = "actionbar\\active\\spellbook" + common_image_extension;

    const string ButtonsFolder = "buttons\\";
    const string ButtonsDisabledFolder = "buttons\\disabled\\";
    const string ButtonsActiveFolder = "buttons\\active\\";
    const string ButtonCommon = "button";
    const string ButtonRadio = "radio";
    const string ButtonCross = "cross"+common_image_extension;

    const string FlagsFolder = "flags\\";
    const string MiniFlagsFolder = "flags\\miniflags\\";
    const string CursorPath = "cursor.ani";
    const string IconPath = "icon.ico";
    const string mapeditor_icons = "mapeditor icons\\";
    const string mapeditor_icons_active = "mapeditor icons\\active\\";
};
class Battle;
class Action;
class Spell;
class Ability;
class ChannelingSpell;
class Action_ChannelingEffect;
class DefaultEntity;
class Entity;
class Gesture;
class Enchant;
class Map;
class Sound;
struct Weapon;
class Tile;
class Wall;

enum enumEntities
{
    NO_ENTITY,
    ENT_WIZ_YELLOW,
    ENT_WIZ_BLUE,
    ENT_WIZ_RED,
    ENT_CONJ_GREEN,
    ENT_CONJ_ORANGE,
    ENT_WAR_RED,
    ENT_WAR_BLUE,
    ENT_FIREBALL,
    ENT_FIREBALL_SMALL,
    ENT_ARROW,
    ENT_SHURIKEN,
    ENT_FON,
    ENT_FON_SMALL,
    ENT_ENCHANTBALL,
    ENT_HEALBALL,
    ENT_SWAPBALL,
    ENT_FLAME,
    ENT_BLUEFLAME,
    ENT_DISPELLER_CENTER,
    ENT_DISPELLER_LIGHT,
    ENT_MAGIC_MISSILE,
    ENT_PIXIE_SWARM,
    ENT_FIST_SHADOW,
    ENT_METEOR_SHADOW,
    ENT_FIRERING_FLAME,
    ENT_MAGICWALL,
    ENT_OBELISK,
    ENT_OBELISK_PRIMITIVE,
    ENT_OBELISK_LOTD,
    ENT_OBELISK_INVISIBLE,
    ENT_TELEPORT_PENTAGRAM,
    ENT_TOXIC_CLOUD,
    ENT_DOOR_WOODEN,
    ENT_DOOR_JAIL
};
enum enumWeapons
{
    WEAP_FIRESTAFF,
    WEAP_FORCESTAFF,
    WEAP_FONSTAFF,
    WEAP_LONGSWORD,
    WEAP_HAMMER,
    WEAP_SHURIKEN,
    WEAP_CHAKRAM,
    WEAP_MACE,
    WEAP_FIRESWORD,
    WEAP_HELLFIRE_STAFF,
    WEAP_BOW
};
enum enumProjectiles
{
    PROJECTILE_FIREBALL,
    PROJECTILE_FIREBALL_SMALL, 
    PROJECTILE_ARROW,
    PROJECTILE_SHURIKEN,
    PROJECTILE_FON,
    PROJECTILE_FON_SMALL,
    PROJECTILE_ENCHANTBALL,
    PROJECTILE_HEALBALL,
    PROJECTILE_SWAPBALL,
    PROJECTILE_MAGIC_MISSILE,
    PROJECTILE_PIXIE_SWARM,
    PROJECTILE_DISPELLER_LIGHT,
    PROJECTILE_FIRERING_FLAME
};

enum enumTriggers
{
    TRIGGER_TELEPORT_PENTAGRAM,
    TRIGGER_TOXIC_CLOUD,
    TRIGGER_DISPELLER
};
enum enumDieFuncs
{
    DIEFUNC_FIREBALL,
    DIEFUNC_FIREBALLSMALL,
    DIEFUNC_METEORSHADOW,
    DIEFUNC_FISTSHADOW,
    DIEFUNC_FON
};

enum BLOCK_QUALITY
{
    NO_BLOCK = 0,
    BLOCK_STAFF = 1,
    BLOCK_LONGSWORD = 2,
    BLOCK_SHIELD = 4,
    BLOCK_QUICKSHIELD = 8
};
enum enumMaterial
{
    NO_MATERIAL,
    MATERIAL_FLESH,
    MATERIAL_METAL,
    MATERIAL_WOOD,
    MATERIAL_STONE
};

enum DmgType
{
    DMG_FIRE,
    DMG_SHOCK,
    DMG_VENOM,
    DMG_PHYS,
    DMG_BLUDGEON,
    DMG_MAGIC
};
enum DAMAGE_FLAGS
{
    DFLAG_CONTINUAL,
    DFLAG_IGNORES_FF,
    DFLAG_IGNORES_ALL,
    DFLAG_IGNORE_SOUND
};
struct Dmg
{
    DmgType type;
    long flags;
    int value;
    Dmg(DmgType type, int value, long flags = 0): type(type), flags(flags), value(value) {}
    Dmg(): type(DMG_FIRE), flags(0), value(0) {}
};

enum enumActions
{
    NO_ACTION,
    ACTION_FIREBALL,
    ACTION_ANCHOR,
    ACTION_BURN,
    ACTION_CHANNEL,
    ACTION_COUNTERSPELL,
    ACTION_DEATHRAY,
    ACTION_DISPELL_UNDEAD,
    ACTION_ENERGY_BOLT,
    ACTION_EARTHQUAKE,
    ACTION_LIGHTNING,
    ACTION_PROTECTION_FIRE,
    ACTION_PROTECTION_SHOCK,
    ACTION_PROTECTION_POISON,
    ACTION_FORCEFIELD,
    ACTION_HEAL_LESSER,
    ACTION_HEAL_GREATER,
    ACTION_HASTE,
    ACTION_INVERSION,
    ACTION_MAGIC_MISSILE,
    ACTION_MANA_DRAIN,
    ACTION_PULL,
    ACTION_PUSH,
    ACTION_REFLECTIVE_SHIELD,
    ACTION_FIRE_RING,
    ACTION_SHOCK,
    ACTION_SLOW,
    ACTION_SWAP,
    ACTION_TELEPORT,
    ACTION_WALL,
    ACTION_FIST_OF_VENGEANCE,
    ACTION_VAMPIRISM,
    ACTION_STUN,
    ACTION_TOXIC_CLOUD,
    ACTION_FORCE_OF_NATURE,
    ACTION_PIXIES,
    ACTION_CHARM,
    ACTION_METEOR,
    ACTION_POISON,
    ACTION_OBLITERATION,
    ACTION_NULLIFY,
    ACTION_BERSERKER,
    ACTION_HARPOON,
    ACTION_WARCRY,
    ACTION_HAMMER,
    ACTION_LONGSWORD,
    ACTION_MACE,
    ACTION_BOW,
    ACTION_FORCE_STAFF,
    ACTION_FIRE_STAFF,
    ACTION_HELLFIRE_STAFF,
    ACTION_SHURIKEN,
    ACTION_FIRESWORD,
    ACTION_FON_STAFF
};
enum enumGestures
{
    G_,
    G_UN,
    G_KA,
    G_ET,
    G_RO,
    G_ZO,
    G_DO,
    G_CHA,
    G_IN
};
enum enumEnchants
{
    NO_ENCHANT,
    ENCHANT_SLOW,
    ENCHANT_HASTE,
    ENCHANT_FORCE_FIELD,
    ENCHANT_REFLECTIVE_SHIELD,
    ENCHANT_PROTECTION_FIRE,
    ENCHANT_PROTECTION_SHOCK,
    ENCHANT_PROTECTION_POISON,
    ENCHANT_STUN,
    ENCHANT_PIXIES,
    ENCHANT_ANCHOR,
    ENCHANT_VAMPIRISM,
    ENCHANT_POISON,
    ENCHANT_SHOCK,
    ENCHANT_NULLIFICATION
};
enum enumWeapontEnchants
{
    NO_CHANT,
    CHANT_FIRE_SPLASH,
    CHANT_SHOCK_SPARK,
    CHANT_POISON,
    CHANT_IMPACT,
    CHANT_SLOW,
    CHANT_STUN,
    CHANT_LEECH,
    CHANT_MANA_LEECH,
    CHANT_MANA_BURN,
    CHANT_ANCHORING
};
enum enumCooldowns
{
    CD_WARCRY = 0,
    CD_BERSERKER,
    CD_HARPOON
};
enum enumChannelingActions
{
    CONT_NOTHING = -1,
    CONT_LIGHTNING,
    CONT_ENERGY_BOLT,
    CONT_GREATER_HEAL,
    CONT_CHARM,
    CONT_DRAIN_MANA,
    CONT_OBLITERATION,
    CONT_CHANNEL_LIFE,
    CONT_LIGHTNING_STAFF
};

enum enumTriggerActivationType
{
    TRIGGERACTIVATE_MOVED_IN,
    TRIGGERACTIVATE_END_TURN,
    TRIGGERACTIVATE_START_TURN,
    TRIGGERACTIVATE_TRIGGERMOVE
};
typedef void (Battle::* ptfAction)(Entity*, const CoordI&, const Action*);        //actor, target, action
typedef void (Battle::* ptfChannelingAction)(Entity*, Entity*, const Action*);  //actor, target, action
typedef bool (Battle::* ptfCollision)(Entity*, const CoordI&, const Direction);   //actor, obstacle coor, direction of collision; returns [can continue moving]
typedef void (Battle::* ptfDie)(Entity*, Entity*);                              //dying object, killer
typedef void (Battle::* ptfTrigger)(Entity*, Entity*, enumTriggerActivationType, const Direction);     //trigger activator object, signal object, activation type, direction of coming in trigger area
namespace ContainerDefs
{
    typedef std::vector<Action *> ActionContainer;
    typedef std::vector<DefaultEntity *> EntityContainer;
    typedef std::vector<ptfAction> FunctionContainer;
    typedef std::vector<ptfChannelingAction> ChannelingFunctionContainer;
    typedef std::vector<ptfCollision> CollisionFunctionContainer;
    typedef std::vector<ptfTrigger> TriggerFunctionContainer;
    typedef std::vector<ptfDie> DieFunctionContainer;
    typedef std::vector<Gesture*> GestureContainer;
    typedef std::vector<Enchant*> EnchantContainer;
    typedef std::vector<Map*> MapContainer;
    typedef std::vector<std::string> StringContainer;
    typedef std::vector<Sound*> SoundContainer;
    typedef std::vector<Weapon*> WeaponContainer;
    typedef std::vector<Wall*> WallContainer;
    typedef std::vector<Tile*> TileContainer;
};
enum Team
{
    NO_TEAM,
    TEAM_BLUE,
    TEAM_RED,
    TEAM_YELLOW,
    TEAM_GREEN,
    TEAM_WHITE,
    TEAM_BLACK
};

enum MouseFlags
{
    CLICK_SHIFT = 1,
    CLICK_CTRL = 2,
    CLICK_ALT = 4,
    CLICK_LEFT = 8,
    CLICK_RIGHT = 16
};
enum Keys
{
    NO_KEY,
    KEY_ESC,
    KEY_SPELL1,
    KEY_SPELL2,
    KEY_SPELL3,
    KEY_SPELL4,
    KEY_SPELL5,
    KEY_ARROW_LEFT,
    KEY_ARROW_RIGHT,
    KEY_ARROW_UP,
    KEY_ARROW_DOWN,
    KEY_ACCEPT,
    KEY_NEXT_SPELLSET,
    KEY_PREV_SPELLSET
};
enum Buttons
{
    NO_BUTTON,
    BUTTON_ENDTURN,
    BUTTON_SPELLICON,
    BUTTON_SPELLSCROLL_UP,
    BUTTON_SPELLSCROLL_DOWN,
    BUTTON_CHARMICON,
    BUTTON_OBJECTLIST,
    BUTTON_ME_SCROLLUP,
    BUTTON_ME_SCROLLDOWN,
    BUTTON_ME_EXPAND,
    BUTTON_SAVE,
    BUTTON_LOAD,
    BUTTON_EXIT
};
enum CursorMode
{
    CURSORMODE_NO,
    CURSORMODE_SELECT,
    CURSORMODE_TARGETING,
    CURSORMODE_LTARGETING,
    CURSORMODE_ME_SET_WALL,
    CURSORMODE_ME_SET_TILE,
    CURSORMODE_ME_CREATE_ENTITY,
    CURSORMODE_ME_DELETE_ENTITY,
    CURSORMODE_ME_DELETE_ALL_ENTITIES,
    CURSORMODE_ME_SET_OWNER,
    CURSORMODE_ME_SET_ENCHANT
};
enum ActionFlags
{
    AFLAG_CANT_TARGET_SELF = 1,
    AFLAG_MUST_TARGET_UNIT = 2,
    AFLAG_MUST_TARGET_VISIBLE = 4,
    AFLAG_CANT_BE_ANCHORED = 8,
    AFLAG_MUST_TARGET_EMPTY = 16,
    AFLAG_NEED_AMMO = 32,
    AFLAG_MUST_TURN_TO_TARGET = 64,
    AFLAG_IS_MELEE = 128,
    AFLAG_IS_AGGRESSIVE = 256,
    AFLAG_IS_TARGETLESS = 512,
    AFLAG_IS_SPELL = 1024,
    AFLAG_IS_ABILITY = 2048,
    AFLAG_IS_QUICK = 4096,//this flag is obsolete, and can be replaced
    AFLAG_RANGE_LIMITED = 8192,
    AFLAG_IS_Channeling = 16384,
    AFLAG_IS_ENCHANT = 32768,
    AFLAG_IS_BROKEN_BY_DAMAGE = 65536
};
#define AFLAG_IS_BUFF (AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_MUST_TURN_TO_TARGET + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE)
#define AFLAG_IS_DEBUFF (AFLAG_IS_BUFF+AFLAG_IS_AGGRESSIVE)
const int max_weapon_enchants = 4;
enum ObjectlistType
{
    OBJLIST_NOTHING,
    OBJLIST_WALLS,
    OBJLIST_TILES,
    OBJLIST_ENTITIES
};

namespace GameBaseConstants
{
    const int visibility_range = 8;
    const int mana_drain_per_turn = 10;
    const Dmg ShockEnchantDamage = Dmg(DMG_SHOCK, 45);
    const Dmg DeathrayDamage = Dmg(DMG_MAGIC, 60);
    const int ObliterationManaPerTurn = 60;
    const int ObliterationDamagePerMana = 4;
    const int EarthquakeCentralDamage = 40;
    const int EarthquakeDamageDecreasePerCell = 8;
    const Dmg FistOfVengeanceDamage = Dmg(DMG_BLUDGEON, 250);
    const Dmg MeteorDamage = Dmg(DMG_FIRE, 80);
    const int RingOfFire_Number = 20;
    const int DispellUndead_Number = 20;
    const int HellStaff_Spread = 30;
    const int WallHalfLength = 3;   //half of the wall, excluding central fragment; actual length will be 2x+1
    const int ShieldAngleHalf = 45;
    const Dmg BerserkerDamage = Dmg(DMG_BLUDGEON, 150);
    const double BersekerSelfdamageMultiplier = 0.2;
    const Dmg ArrowDamage = Dmg(DMG_PHYS, 64);
    const Dmg FireballBlastDamage = Dmg(DMG_FIRE, 128);
    const Dmg FireballSmallBlastDamage = Dmg(DMG_FIRE, 64);
    const Dmg ShurikenDamage = Dmg(DMG_PHYS, 40);
    const Dmg MissilesDamage = Dmg(DMG_FIRE, 40);
    const Dmg PixiesDamage = Dmg(DMG_MAGIC, 40);
    const int HealValue = 25;
    const Dmg RingOfFireDamage = Dmg(DMG_FIRE, 30);
    const Dmg ToxicCloudDamage = Dmg(DMG_VENOM, 30);
    const Dmg PoisonDamage = Dmg(DMG_VENOM, 5, DFLAG_IGNORES_FF + DFLAG_IGNORE_SOUND);
    const Dmg LightningImpulseDamage = Dmg(DMG_SHOCK, 10);
    const int LightningImpulseNumber = 4;
    const int LightningImpulseCost = 16;
    const Dmg EnergyboltImpulseDamage = Dmg(DMG_SHOCK, 10);
    const int EnergyboltImpulseNumber = 5;
    const int EnergyboltImpulseCost = 10;
    const int SecondaryLightningsRange = 3;
    const int GreaterHealImpulseHeal = 5;
    const int GreaterHealImpulseNumber = 8;
    const int GreaterHealImpulseCost = 5;
    const int ManaDrainPerApply = 40;
    const int ChannelPerApply = 40;
    const int ForceOfNatureSpread = 120; //degrees

};
class ErrorBase
{
public:
    virtual string Text() const = 0;
    virtual ~ErrorBase() {}
};
namespace Errors
{
    class EntityOutsideGrid: public ErrorBase
    {
        Entity* ent;
    public:
        EntityOutsideGrid(Entity* ent): ent(ent) {}
        virtual string Text() const {
            return "Entity lies outside the field grid";
        }
        virtual ~EntityOutsideGrid() {}
    };
    class EntityNotInGrid: public ErrorBase
    {
        Entity* ent;
    public:
        EntityNotInGrid(Entity* ent): ent(ent) {}
        virtual string Text() const {
            return "Entity cannot be found in the grid";
        }
        virtual ~EntityNotInGrid() {}
    };
    class CurSelCoorMismatchingEntity: public ErrorBase
    {
    public:
        CurSelCoorMismatchingEntity() {}
        virtual string Text() const {
            return "Currently selected entity mismatches coor with currently selected cell";
        }
        virtual ~CurSelCoorMismatchingEntity() {}
    };
    class SpellsetNotInRange: public ErrorBase
    {
    public:
        SpellsetNotInRange() {}
        virtual string Text() const {
            return "Currently selected spellset is outside of proper range";
        }
        virtual ~SpellsetNotInRange() {}
    };
    class SpellNotInRange: public ErrorBase
    {
    public:
        SpellNotInRange() {}
        virtual string Text() const {
            return "Currently selected spell is outside of proper range";
        }
        virtual ~SpellNotInRange() {}
    };
    class GridWrongSize: public ErrorBase
    {
    public:
        GridWrongSize() {}
        virtual string Text() const {
            return "The grid has wrong size";
        }
        virtual ~GridWrongSize() {}
    };
    class GridNullWall: public ErrorBase
    {
        CoordI coor;
    public:
        GridNullWall(CoordI coor): coor(coor) {}
        virtual string Text() const {
            return "The grid has got null wall";
        }
        virtual ~GridNullWall() {}
    };
    class GridNullTile: public ErrorBase
    {
        CoordI coor;
    public:
        GridNullTile(CoordI coor): coor(coor) {}
        virtual string Text() const {
            return "The grid has got null tile";
        }
        virtual ~GridNullTile() {}
    };
    class GridEntityMismatchingCoor: public ErrorBase
    {
        CoordI coor;
        Entity* ent;
    public:
        GridEntityMismatchingCoor(CoordI coor, Entity* ent): coor(coor), ent(ent) {}
        virtual string Text() const {
            return "The grid has got an entity not registered in corresponding cell";
        }
        virtual ~GridEntityMismatchingCoor() {}
    };
    class EntityAliveWithNegativeHealth: public ErrorBase
    {
        const Entity* ent;
    public:
        EntityAliveWithNegativeHealth(const Entity* ent): ent(ent) {}
        virtual string Text() const {
            return "The entity is alive with negative health";
        }
        virtual ~EntityAliveWithNegativeHealth() {}
    };

    class EntityFakeOwner: public ErrorBase
    {
        const Entity* ent;
        const Entity* object_owner;
        const Entity* unregistered_owner;
    public:
        EntityFakeOwner(const Entity* ent, const Entity* owner, const Entity* fake_owner):
            ent(ent), object_owner(owner), unregistered_owner(fake_owner) {}
        virtual string Text() const {
            return "The entity has a sub, that has different owner";
        }
        virtual ~EntityFakeOwner() {}
    };
    class EntityFakeSub: public ErrorBase
    {
        const Entity* ent;
        const Entity* sub;
    public:
        EntityFakeSub(const Entity* ent, const Entity* sub):
            ent(ent), sub(sub) {}
        virtual string Text() const {
            return "The entity claims to be a sub, but is not listed";
        }
        virtual ~EntityFakeSub() {}
    };
    class EntityNoDirection: public ErrorBase
    {
        const Entity* ent;
    public:
        EntityNoDirection(const Entity* ent): ent(ent) {}
        virtual string Text() const {
            return "The entity has got no direction";
        }
        virtual ~EntityNoDirection() {}
    };
    class UnitNegativeEnchant: public ErrorBase
    {
        const Entity* ent;
        enumEnchants id;
    public:
        UnitNegativeEnchant(const Entity* ent, enumEnchants id): ent(ent), id(id) {}
        virtual string Text() const {
            return "The unit has got negative value of an enchant";
        }
        virtual ~UnitNegativeEnchant() {}
    };
    class UnitNegativeCooldown: public ErrorBase
    {
        const Entity* ent;
        const Action* action;
    public:
        UnitNegativeCooldown(const Entity* ent, const Action* action): ent(ent), action(action) {}
        virtual string Text() const {
            return "The unit has got negative value of a cooldown";
        }
        virtual ~UnitNegativeCooldown() {}
    };
    class UnitMovepointsMoreThanSpeed: public ErrorBase
    {
        const Entity* ent;
    public:
        UnitMovepointsMoreThanSpeed(const Entity* ent): ent(ent) {}
        virtual string Text() const {
            return "The unit has got more move points, than speed";
        }
        virtual ~UnitMovepointsMoreThanSpeed() {}
    };
    class UnitNullCSpellTarget: public ErrorBase
    {
        const Entity* ent;
        const Action* action;
    public:
        UnitNullCSpellTarget(const Entity* ent, const Action* action): ent(ent), action(action) {}
        virtual string Text() const {
            return "The unit has got null pointer as channel spell target";
        }
        virtual ~UnitNullCSpellTarget() {}
    };
    class ProjectileMismatchingCoors: public ErrorBase
    {
        const Entity* ent;
    public:
        ProjectileMismatchingCoors(const Entity* ent): ent(ent) {}
        virtual string Text() const {
            return "The projectile has mismatching coor and exact coor";
        }
        virtual ~ProjectileMismatchingCoors() {}
    };
};