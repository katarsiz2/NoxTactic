#include "Strings.h"
#include "Constants.h"
std::string int_to_string(int a) {
    char buf[100];
    _itoa_s(a, buf, 10);
    std::string tmp(buf);
    return tmp;
}
void Loader::loadStrings(StringContainer& container) {
    container.resize(Counters::strings);
    container[NO_STRING] = "";
    //Enchants
    container[STR_ENCHANT_SLOW] = "Slow";
    container[STR_ENCHANT_HASTE] = "Haste";
    container[STR_ENCHANT_FORCE_FIELD] = "Force Field";
    container[STR_ENCHANT_REFLECTIVE_SHIELD] = "Reflective Shield";
    container[STR_ENCHANT_PROTECTION_FIRE] = "Protection from Fire";
    container[STR_ENCHANT_PROTECTION_SHOCK] = "Protection from Shock";
    container[STR_ENCHANT_PROTECTION_POISON] = "Protection from Poison";
    container[STR_ENCHANT_STUN] = "Stun";
    container[STR_ENCHANT_PIXIES] = "Pixies";
    container[STR_ENCHANT_ANCHOR] = "Anchor";
    container[STR_ENCHANT_VAMPIRISM] = "Vampirism";
    container[STR_ENCHANT_POISON] = "Poison";
    container[STR_ENCHANT_SHOCK] = "Shock";
    container[STR_ENCHANT_NULLIFICATION] = "Nullification";
    //enumActions
    container[STR_ACTION_FIREBALL] = "Fireball";
    container[STR_ACTION_ANCHOR] = "Anchor";
    container[STR_ACTION_BURN] = "Burn";
    container[STR_ACTION_CHANNEL] = "Channel";
    container[STR_ACTION_COUNTERSPELL] = "Counterspell";
    container[STR_ACTION_DEATHRAY] = "Death ray";
    container[STR_ACTION_DISPELL_UNDEAD] = "Dispell Undead";
    container[STR_ACTION_ENERGY_BOLT] = "Energy Bolt";
    container[STR_ACTION_EARTHQUAKE] = "Earthquake";
    container[STR_ACTION_LIGHTNING] = "Lightning";
    container[STR_ACTION_PROTECTION_FIRE] = "Protection from Fire";
    container[STR_ACTION_PROTECTION_SHOCK] = "Protection from Shock";
    container[STR_ACTION_PROTECTION_VENOM] = "Protection from Poison";
    container[STR_ACTION_FORCEFIELD] = "Force Field";
    container[STR_ACTION_HEAL_LESSER] = "Lesser Heal";
    container[STR_ACTION_HEAL_GREATER] = "Greater Heal";
    container[STR_ACTION_HASTE] = "Haste";
    container[STR_ACTION_INVERSION] = "Inversion";
    container[STR_ACTION_MAGIC_MISSILE] = "Missiles of Magic";
    container[STR_ACTION_MANA_DRAIN] = "Mana Drain";
    container[STR_ACTION_PULL] = "Pull";
    container[STR_ACTION_PUSH] = "Push";
    container[STR_ACTION_REFLECTIVE_SHIELD] = "Reflective Shield";
    container[STR_ACTION_FIRE_RING] = "Ring of Fire";
    container[STR_ACTION_SHOCK] = "Shock";
    container[STR_ACTION_SLOW] = "Slow";
    container[STR_ACTION_SWAP] = "Swap Locations";
    container[STR_ACTION_TELEPORT] = "Teleport to Target";
    container[STR_ACTION_WALL] = "Magic Wall";
    container[STR_ACTION_FIST_OF_VENGEANCE] = "Fist of Vengeance";
    container[STR_ACTION_VAMPIRISM] = "Vampirism";
    container[STR_ACTION_STUN] = "Stun";
    container[STR_ACTION_TOXIC_CLOUD] = "Toxic Cloud";
    container[STR_ACTION_FORCE_OF_NATURE] = "Force of Nature";
    container[STR_ACTION_PIXIES] = "Pixie Swarm";
    container[STR_ACTION_CHARM] = "Charm Creature";
    container[STR_ACTION_METEOR] = "Meteor";
    container[STR_ACTION_POISON] = "Poison";
    container[STR_ACTION_NULLIFY] = "Nullification";
    container[STR_ACTION_OBLITERATION] = "Obliteration";
    container[STR_ACTION_BERSERKER] = "Berserker Charge";
    container[STR_ACTION_HARPOON] = "Harpoon";
    container[STR_ACTION_WARCRY] = "Warcry";
    container[STR_ACTION_HAMMER] = "Hammer strike";
    container[STR_ACTION_LONGSWORD] = "Longsword strike";
    container[STR_ACTION_MACE] = "Mace strike";
    container[STR_ACTION_BOW] = "Bow shot";
    container[STR_ACTION_FORCE_STAFF] = "Force Staff charge";
    container[STR_ACTION_FIRE_STAFF] = "Fire Staff shot";
    container[STR_ACTION_HELLFIRE_STAFF] = "Hellfire Staff shot";
    container[STR_ACTION_SHURIKEN] = "Shuriken throw";
    container[STR_ACTION_FIRESWORD] = "Fire sword strike";
    container[STR_ACTION_FON_STAFF] = "Force of Nature Staff charge";
    //Entities
    container[STR_ENT_WIZ_YELLOW] = "Mage";
    container[STR_ENT_WIZ_BLUE] = "Guardian wizard";
    container[STR_ENT_WIZ_RED] = "Warlock";
    container[STR_ENT_CONJ_GREEN] = "Conjurer";
    container[STR_ENT_CONJ_ORANGE] = "Ix priest";
    container[STR_ENT_WAR_RED] = "Fire knight";
    container[STR_ENT_WAR_BLUE] = "Warrior";
    container[STR_ENT_FIREBALL] = "Fireball";
    container[STR_ENT_FIREBALL_SMALL] = "Fireball";
    container[STR_ENT_ARROW] = "Arrow";
    container[STR_ENT_SHURIKEN] = "Shuriken";
    container[STR_ENT_FON] = "Force of Nature";
    container[STR_ENT_FON_SMALL] = "Force of Nature piece";
    container[STR_ENT_ENCHANTBALL] = "Incantation";
    container[STR_ENT_FLAME] = "Flame";
    container[STR_ENT_BLUEFLAME] = "Blue flame";
    container[STR_ENT_DISPELLER_CENTER] = "Dispeller";
    container[STR_ENT_DISPELLER_LIGHT] = "Dispeller";
    container[STR_ENT_MAGIC_MISSILE] = "Magic Missile";
    container[STR_ENT_PIXIE_SWARM] = "Pixie Swarm";
    container[STR_ENT_FIST_SHADOW] = "Fist of Vengeance Shadow";
    container[STR_ENT_METEOR_SHADOW] = "Meteor Shadow";
    container[STR_ENT_FIRERING_FLAME] = "Flame";
    container[STR_ENT_MAGICWALL] = "Magic Wall";
    container[STR_ENT_OBELISK] = "Obelisk";
    container[STR_ENT_OBELISK_PRIMITIVE] = "ObeliskPrimitive";
    container[STR_ENT_OBELISK_LOTD] = "ObeliskLOTD";
    container[STR_ENT_TELEPORT_PENTAGRAM] = "Teleport pentagram";
    container[STR_ENT_TOXIC_CLOUD] = "Toxic Cloud";
    container[STR_ENT_DOOR_WOODEN] = "Wooden Door";
    container[STR_ENT_DOOR_JAIL] = "Jail Door";
    //Actions
    container[STR_CASTED_SPELL] = "$1 has casted $2";
    container[STR_OBJECT_CREATED] = "$1 has been summoned at $2";
    container[STR_WAS_KILLED] = "$1 has been killed";
    container[STR_WAS_DESTROYED] = "$1 has been destroyed";
    container[STR_WAS_DAMAGED_BY] = "$1 has been damaged by $2 by $3 points";
    container[STR_WAS_DAMAGED] = "$1 has been damaged by $2 points";
    container[STR_MOVED_TO] = "$1 moved from $2 to $3";
    container[STR_WAS_ENCHANTED_FOR] = "$1 was enchanted with $2 for $3 turns";
    container[STR_WAS_ENCHANTED] = "$1 was enchanted with $2";
    container[STR_DISENCHANTED] = "$2 faded away from $1";
    container[STR_REFLECTED] = "$1 was reflected at $2";
    container[STR_STOPPED_CASTING] = "$1 has stopped casting $2";
    container[STR_ENDTURN] = "Player #$1 finishes his turn";
    //enumTiles
    container[STR_TILE_LAVA] = "Lava";
    container[STR_TILE_RUG] = "Rug";
    container[STR_TILE_GRASS] = "Grass";
    container[STR_WALL_COBBLESTONE] = "Cobblestone";
    container[STR_WALL_BRICK] = "Brick";
    container[STR_WALL_UNDERWORLD] = "Underworld";
    container[STR_WALL_MAGIC] = "Magic";

    container[STR_CAN_ACT_0] = "Can not act this turn";
    container[STR_CAN_ACT_1] = "Has partially acted this turn";
    container[STR_CAN_ACT_2] = "Did not act this turn";

    container[STR_IS_PERFORMING] = "This unit is performing ";
    container[STR_IS_BLOCKING] = "This unit can block with its equipment ";
    
    container[STR_CONT_LIGHTNING] = "This unit is chanelling lightning";
    container[STR_CONT_ENERGY_BOLT] = "This unit is chanelling energy";
    container[STR_CONT_GREATER_HEAL] = "This unit is chanelling life";
    container[STR_CONT_CHARM] = "This unit is charming";
    container[STR_CONT_DRAIN_MANA] = "This unit is draining mana";
    container[STR_CONT_OBLITERATION] = "This unit is preparing obliteration";
    container[STR_CONT_CHANNEL_LIFE] = "This unit is chanelling mana";
    container[STR_CONT_LIGHTNING_STAFF] = "This unit is chanelling lightning";

    container[STR_IS_ON_COOLDOWN] = " is on cooldown";
    container[STR_IS_ENCHANTED] = "This unit is enchanted with ";
    
    
}