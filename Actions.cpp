#include "Actions.h"
#include "Constants.h"
#include <algorithm>
using namespace ContainerDefs;
Action::Action(ptfAction action,
    int range, int action_points_cost, int performtime,
    enumStrings name, enumStrings msg_done, enumStrings msg_started,
    enumStrings msg_stopped, enumStrings msg_affected, enumStrings msg_killed, long flags):
    action(action), range(range), action_points_cost(action_points_cost),
    time_to_perform(performtime), name(name), msg_performed(msg_done),
    msg_started(msg_started), msg_stopped(msg_stopped), msg_affected(msg_affected),
    msg_killed(msg_killed), flags(flags), icon_texture_id(0), active_icon_texture_id(0) {}
void Action::Perform(Battle* battle, Entity* actor, const CoordI& target) const {
    (battle->*action)(actor, target, this);
}
bool Action::CheckForValidity(Battle* battle, Entity* actor, const CoordI& target) const {
    if (!actor->Flag(ENT_IS_UNIT)) { return false; }
    Unit* unit = dynamic_cast<Unit*>(actor);
    //const DefaultUnit& me = unit->GetPrototype();
    bool is_lengthy = (time_to_perform > 0);
    if (unit->getActionPoints() < action_points_cost) { return false; }

    if (flags & AFLAG_CANT_TARGET_SELF) {
        if (unit->coor == target) { return false; } //DONT TARGET YOURSELF >(
    }
    if (flags & AFLAG_MUST_TARGET_EMPTY) {
        if (battle->Grid()(target)) { return false; }
    }
    if (flags & AFLAG_MUST_TARGET_UNIT) {
        if (!battle->Grid()(target) || !battle->Grid()(target)->Flag(ENT_IS_UNIT)) { return false; }
    }
    if (flags & AFLAG_CANT_BE_ANCHORED) {
        if (unit->IsEnchanted(ENCHANT_ANCHOR)) { return false; }
    }
    if (flags & AFLAG_NEED_AMMO) {
        if (!unit->ammo) { return false; }
    }
    if (flags & (AFLAG_MUST_TURN_TO_TARGET | AFLAG_IS_MELEE)) {    //melee always requires turning
        Direction tmp = unit->coor.getDirection(target);
        if (tmp != NO_DIRECTION && tmp != unit->Dir() && !unit->movepoints) { return false; } //directions not match and no points left to turn
    }
    if (flags & AFLAG_IS_MELEE) {
        if (battle->Grid().Dist(unit->coor, target, true) != 1) { return false; } //too far away
    }
    if (flags & AFLAG_IS_TARGETLESS) {
        if (unit->coor != target) { return false; }
    }
    if (flags & AFLAG_RANGE_LIMITED) {
        if (battle->Grid().Dist(unit->coor, target, true) > range) { return false; }
    }
    if (flags & AFLAG_MUST_TARGET_VISIBLE) {
        if (!battle->Grid().IsSeenFrom(unit->Coor(), target)) { return false; }
    }
    return true;
}
void Action::ApplyPenalties(Battle* battle, Entity* actor, const CoordI& target) const {
    if (!actor->Flag(ENT_IS_UNIT)) { return; }
    Unit* unit = dynamic_cast<Unit*>(actor);
    unit->action_points -= action_points_cost;
    if (flags & AFLAG_NEED_AMMO) {
        unit->ammo--;
    }
    if (flags & (AFLAG_MUST_TURN_TO_TARGET | AFLAG_IS_MELEE)) {
        Direction dir = unit->coor.getDirection(target);
        if (dir != NO_DIRECTION && dir != unit->Dir()) {
            battle->Turn(unit, dir);
            unit->movepoints--;
        }
    }
    unit->is_in_blocking_state = false;

}

Spell::Spell(const Action& action, int manacost, enumGestures g1, enumGestures g2, enumGestures g3,
    enumGestures g4, enumGestures g5, enumGestures g6, enumGestures g7, enumGestures g8):
    Action(action), manacost(manacost) {
    gesture[0] = g1;
    gesture[1] = g2;
    gesture[2] = g3;
    gesture[3] = g4;
    gesture[4] = g5;
    gesture[5] = g6;
    gesture[6] = g7;
    gesture[7] = g8;
}
void Spell::Perform(Battle* battle, Entity* actor, const CoordI& target) const {
    //TODO: sound gestures
    (battle->*action)(actor, target, this);
}
bool Spell::CheckForValidity(Battle* battle, Entity* actor, const CoordI& target) const {
    if (!Action::CheckForValidity(battle, actor, target)) { return false; }
    if (!actor->Flag(ENT_IS_UNIT)) { return false; }
    Unit* unit = dynamic_cast<Unit*>(actor);

    if (unit->IsEnchanted(ENCHANT_NULLIFICATION) || unit->mp < manacost) { return false; }
    return true;
}
void Spell::ApplyPenalties(Battle* battle, Entity* actor, const CoordI& target) const {
    Action::ApplyPenalties(battle, actor, target);
    if (!actor->Flag(ENT_IS_UNIT)) { return; }
    Unit* unit = dynamic_cast<Unit*>(actor);

    unit->mp -= manacost;
}

Ability::Ability(const Action& action, int cooldown, enumCooldowns cd_index):
Action(action), cooldown(cooldown), cd_index(cd_index) {}
void Ability::Perform(Battle* battle, Entity* actor, const CoordI& target) const {
    (battle->*action)(actor, target, this);
}
bool Ability::CheckForValidity(Battle* battle, Entity* actor, const CoordI& target) const {
    if (!Action::CheckForValidity(battle, actor, target)) { return false; }
    if (!actor->Flag(ENT_IS_UNIT)) { return false; }
    Unit* unit = dynamic_cast<Unit*>(actor);

    if (unit->cooldowns[cd_index] > 0) { return false; }
    return true;
}
void Ability::ApplyPenalties(Battle* battle, Entity* actor, const CoordI& target) const {
    Action::ApplyPenalties(battle, actor, target);
    if (!actor->Flag(ENT_IS_UNIT)) { return; }
    Unit* unit = dynamic_cast<Unit*>(actor);

    unit->cooldowns[cd_index] = cooldown;
}

Action_ChannelingEffect::Action_ChannelingEffect(enumChannelingActions id, ptfChannelingAction Channeling_action,
    enumStrings performing_tip, bool is_dispellable):
    id(id), Channeling_action(Channeling_action), performing_tip(performing_tip), is_dispellable(is_dispellable) {}
Action_ProjectileLauncher::Action_ProjectileLauncher(enumEntities projectile_id, bool homing) :
projectile_id(projectile_id), homing(homing) {}
void Action_ProjectileLauncher::Launch(Battle* battle, Entity* actor, const CoordI& target) const {
    if (homing) {
        battle->LaunchHomingProjectile(projectile_id, actor, battle->Grid()(target));
    } else {
        battle->LaunchProjectile(projectile_id, actor, target);
    }
}

ProjectileSpell::ProjectileSpell(const Spell& spell, enumEntities id, bool homing):
Spell(spell), Action_ProjectileLauncher(id, homing) {}
void ProjectileSpell::Perform(Battle* battle, Entity* actor, const CoordI& target) const {
    Launch(battle, actor, target);
}
ProjectileAction::ProjectileAction(const Action& action, enumEntities id, bool homing) : Action(action), Action_ProjectileLauncher(id, homing) {}
void ProjectileAction::Perform(Battle* battle, Entity* actor, const CoordI& target) const {
    Launch(battle, actor, target);
}

EnchantSpell::EnchantSpell(const Spell& spell, enumEnchants id) :
Spell(spell), id(id) {}
void EnchantSpell::Perform(Battle* battle, Entity* actor, const CoordI& target) const {
    if (target == actor->Coor()) {
        battle->EnchantWith(actor, id);
    } else {
        battle->LaunchHomingProjectile(ENT_ENCHANTBALL, actor, battle->Grid()(target), id);
    }
}

ChannelingAction::ChannelingAction(const Action& action, Action_ChannelingEffect cont_action):
Action(action), Action_ChannelingEffect(cont_action) {}
bool ChannelingAction::ApplyEffect(Battle* battle, Entity* actor, Entity* target) const {
    (battle->*Channeling_action)(actor, target, this);
    return true;
}
void ChannelingAction::Perform(Battle* battle, Entity* actor, const CoordI& target) const {
    Unit* unit = dynamic_cast<Unit*>(actor);
    unit->GetC_Action(ID()).SetCasting(battle->Grid()(target));
}

ChannelingSpell::ChannelingSpell(const Spell& spell, Action_ChannelingEffect cont_action):
Spell(spell), Action_ChannelingEffect(cont_action) {}
bool ChannelingSpell::ApplyEffect(Battle* battle, Entity* actor, Entity* target) const {
    (battle->*Channeling_action)(actor, target, this);
    return true;
}
void ChannelingSpell::Perform(Battle* battle, Entity* actor, const CoordI& target) const {
    Unit* unit = dynamic_cast<Unit*>(actor);
    unit->c_action[ID()].SetCasting(battle->Grid()(target));
}

//===================loaders

Map::~Map() {
    for_each(entities.begin(), entities.end(), [](Entity *ent){
        delete ent;
    });
}
void Loader::loadMaps(MapContainer& container, EntityContainer& entities, WallContainer& walls, TileContainer& tiles) {
    vector<Entity*> tmpent;
    vector<vector<Tile*>> tmptiles(20, vector<Tile*>(14, tiles[TILE_GRASS]));
    vector<vector<Wall*>> tmpwalls(20, vector<Wall*>(14, nullptr));
    for (int i = 1; i <= 20; ++i) {
        tmpwalls[i-1][0] = walls[WALL_BRICK];
        tmpwalls[i-1][13] = walls[WALL_BRICK];
    }
    for (int i = 2; i < 14; ++i) {
        tmpwalls[0][i-1] = walls[WALL_BRICK];
        tmpwalls[19][i-1] = walls[WALL_BRICK];
    }
    tmpwalls[3][3] = walls[WALL_BRICK];
    tmpwalls[4][3] = walls[WALL_BRICK];
    tmpwalls[4][4] = walls[WALL_BRICK];
    tmpwalls[5][4] = walls[WALL_BRICK];
    tmpwalls[5][5] = walls[WALL_BRICK];
    tmpwalls[6][5] = walls[WALL_BRICK];
    tmpwalls[6][6] = walls[WALL_BRICK];
    tmpwalls[7][6] = walls[WALL_BRICK];
    tmpwalls[7][7] = walls[WALL_BRICK];

    tmpwalls[3][8] = walls[WALL_BRICK];

    tmpwalls[5][11] = walls[WALL_BRICK];
    tmpwalls[6][11] = walls[WALL_BRICK];
    tmpwalls[7][11] = walls[WALL_BRICK];
    tmpwalls[8][11] = walls[WALL_BRICK];
    tmpwalls[9][11] = walls[WALL_BRICK];
    tmpwalls[7][10] = walls[WALL_BRICK];

    tmpwalls[10][1] = walls[WALL_BRICK];
    tmpwalls[10][2] = walls[WALL_BRICK];
    tmpwalls[10][3] = walls[WALL_BRICK];
    tmpwalls[10][4] = walls[WALL_BRICK];
    tmpwalls[10][6] = walls[WALL_BRICK];
    tmpwalls[10][7] = walls[WALL_BRICK];

    tmpwalls[13][3] = walls[WALL_BRICK];
    tmpwalls[14][3] = walls[WALL_BRICK];
    tmpwalls[15][3] = walls[WALL_BRICK];
    tmpwalls[16][3] = walls[WALL_BRICK];
    tmpwalls[16][4] = walls[WALL_BRICK];
    tmpwalls[17][4] = walls[WALL_BRICK];

    tmpwalls[17][7] = walls[WALL_BRICK];
    tmpwalls[17][8] = walls[WALL_BRICK];

    tmpwalls[15][12] = walls[WALL_BRICK];

    tmpwalls[13][7] = walls[WALL_BRICK];
    tmpwalls[13][8] = walls[WALL_BRICK];
    tmpwalls[13][9] = walls[WALL_BRICK];
    tmpwalls[13][10] = walls[WALL_BRICK];
    tmpwalls[13][11] = walls[WALL_BRICK];
    tmpwalls[12][11] = walls[WALL_BRICK];
    tmpwalls[11][11] = walls[WALL_BRICK];


    tmpent.reserve(22);
    tmpent.push_back(entities[ENT_WAR_BLUE]->Create(CoordI(3, 3), TEAM_BLUE, RIGHT));
    tmpent.push_back(entities[ENT_WAR_RED]->Create(CoordI(3, 4), TEAM_BLUE, RIGHT));
    tmpent.push_back(entities[ENT_WIZ_YELLOW]->Create(CoordI(3, 8), TEAM_BLUE, RIGHT));
    tmpent.push_back(entities[ENT_WIZ_BLUE]->Create(CoordI(3, 9), TEAM_BLUE, RIGHT));
    tmpent.push_back(entities[ENT_WIZ_RED]->Create(CoordI(5, 6), TEAM_BLUE, DOWN));
    tmpent.push_back(entities[ENT_WIZ_RED]->Create(CoordI(3, 12), TEAM_BLUE, RIGHT));
    tmpent.push_back(entities[ENT_CONJ_ORANGE]->Create(CoordI(3, 13), TEAM_BLUE, RIGHT));

    tmpent.push_back(entities[ENT_WAR_BLUE]->Create(CoordI(12, 2), TEAM_RED, DOWN));
    tmpent.push_back(entities[ENT_WAR_RED]->Create(CoordI(13, 2), TEAM_RED, DOWN));
    tmpent.push_back(entities[ENT_WIZ_YELLOW]->Create(CoordI(17, 10), TEAM_RED, LEFT));
    tmpent.push_back(entities[ENT_WIZ_BLUE]->Create(CoordI(17, 11), TEAM_RED, LEFT));
    tmpent.push_back(entities[ENT_WIZ_RED]->Create(CoordI(17, 12), TEAM_RED, LEFT));
    tmpent.push_back(entities[ENT_WIZ_RED]->Create(CoordI(18, 6), TEAM_RED, LEFT));
    tmpent.push_back(entities[ENT_CONJ_ORANGE]->Create(CoordI(18, 7), TEAM_RED, LEFT));

    tmpent.push_back(entities[ENT_OBELISK_LOTD]->Create(CoordI(4, 5), NO_TEAM, LEFT));
    tmpent.push_back(entities[ENT_OBELISK_LOTD]->Create(CoordI(4, 8), NO_TEAM, LEFT));
    tmpent.push_back(entities[ENT_OBELISK_LOTD]->Create(CoordI(7, 5), NO_TEAM, LEFT));
    tmpent.push_back(entities[ENT_OBELISK_PRIMITIVE]->Create(CoordI(7, 11), NO_TEAM, LEFT));
    tmpent.push_back(entities[ENT_OBELISK_PRIMITIVE]->Create(CoordI(11, 9), NO_TEAM, LEFT));
    tmpent.push_back(entities[ENT_OBELISK_PRIMITIVE]->Create(CoordI(16, 5), NO_TEAM, LEFT));
    tmpent.push_back(entities[ENT_OBELISK]->Create(CoordI(17, 8), NO_TEAM, LEFT));
    tmpent.push_back(entities[ENT_OBELISK]->Create(CoordI(17, 9), NO_TEAM, LEFT));

    Map* tmpmap = new Map(CoordI(20, 14), tmpent, tmpwalls, tmptiles, 2);
    container.push_back(tmpmap);
}

void Loader::loadActions(ActionContainer& container,
    FunctionContainer& functions, ChannelingFunctionContainer& Channeling_functions) {
    container.resize(Counters::actions);
    ChannelingFunctionContainer::iterator Channeling_iter = Channeling_functions.begin();

    container[NO_ACTION] = new Action(functions[NO_ACTION], 0, 0, 0, NO_STRING,
        NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING, 0);

    container[ACTION_FIREBALL] = new ProjectileSpell(Spell(Action(functions[ACTION_FIREBALL], 0, 2, 0,
        STR_ACTION_FIREBALL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_CANT_TARGET_SELF + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TURN_TO_TARGET),
        30, G_ZO, G_ZO, G_UN), ENT_FIREBALL, false);
    container[ACTION_ANCHOR] = new EnchantSpell(Spell(Action(functions[ACTION_ANCHOR], visibility_range, 2, 0,
        STR_ACTION_ANCHOR, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_DEBUFF),
        10, G_ZO, G_ZO), ENCHANT_ANCHOR);
    container[ACTION_BURN] = new Spell(Action(functions[ACTION_BURN], visibility_range, 2, 0,
        STR_ACTION_BURN, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_MUST_TARGET_VISIBLE),
        10, G_ZO, G_ZO, G_UN, G_UN);
    container[ACTION_CHANNEL] = new ChannelingSpell(Spell(Action(functions[ACTION_CHANNEL], 0, 2, 0,
        STR_ACTION_CHANNEL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_Channeling + AFLAG_IS_BROKEN_BY_DAMAGE),
        0, G_KA, G_ZO, G_IN), Action_ChannelingEffect(CONT_CHANNEL_LIFE, Channeling_functions[CONT_CHANNEL_LIFE], STR_CONT_CHANNEL_LIFE));
    container[ACTION_COUNTERSPELL] = new Spell(Action(functions[ACTION_COUNTERSPELL], visibility_range / 2, 2, 0,
        STR_ACTION_COUNTERSPELL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_AGGRESSIVE),
        20, G_ZO, G_DO);
    container[ACTION_DISPELL_UNDEAD] = new Spell(Action(functions[ACTION_DISPELL_UNDEAD], 2, 2, 2,
        STR_ACTION_DISPELL_UNDEAD, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_AGGRESSIVE),
        60, G_UN, G_KA, G_ET, G_RO, G_ZO, G_DO, G_CHA, G_IN);
    container[ACTION_DEATHRAY] = new Spell(Action(functions[ACTION_DEATHRAY], visibility_range, 2, 0,
        STR_ACTION_DEATHRAY, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_IS_AGGRESSIVE + AFLAG_CANT_TARGET_SELF + AFLAG_MUST_TURN_TO_TARGET),
        60, G_DO, G_DO);
    container[ACTION_ENERGY_BOLT] = new ChannelingSpell(Spell(Action(functions[ACTION_ENERGY_BOLT], visibility_range/2 + 1, 2, 0,
        STR_ACTION_ENERGY_BOLT, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_Channeling + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF + AFLAG_IS_BROKEN_BY_DAMAGE),
        0, G_ZO, G_CHA, G_ET, G_UN), Action_ChannelingEffect(CONT_ENERGY_BOLT, Channeling_functions[CONT_ENERGY_BOLT], STR_CONT_ENERGY_BOLT));
    container[ACTION_EARTHQUAKE] = new Spell(Action(functions[ACTION_EARTHQUAKE], visibility_range/2 + 1, 2, 2,
        STR_ACTION_EARTHQUAKE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_AGGRESSIVE),
        60, G_DO, G_ET, G_IN, G_RO, G_CHA, G_KA);
    container[ACTION_LIGHTNING] = new ChannelingSpell(Spell(Action(functions[ACTION_LIGHTNING], visibility_range/2 + 1, 2, 2,
        STR_ACTION_LIGHTNING, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_Channeling + AFLAG_CANT_TARGET_SELF + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_BROKEN_BY_DAMAGE),
        0, G_ZO, G_CHA, G_ET, G_UN, G_ET, G_CHA, G_ZO), Action_ChannelingEffect(CONT_LIGHTNING, Channeling_functions[CONT_LIGHTNING], STR_CONT_LIGHTNING));
    container[ACTION_PROTECTION_FIRE] = new EnchantSpell(Spell(Action(functions[ACTION_PROTECTION_FIRE], visibility_range, 2, 0,
        STR_ACTION_PROTECTION_FIRE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_BUFF),
        30, G_ET, G_CHA, G_DO, G_KA), ENCHANT_PROTECTION_FIRE);
    container[ACTION_PROTECTION_SHOCK] = new EnchantSpell(Spell(Action(functions[ACTION_PROTECTION_SHOCK], visibility_range, 2, 0,
        STR_ACTION_PROTECTION_SHOCK, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_BUFF),
        30, G_CHA, G_ET, G_DO, G_KA), ENCHANT_PROTECTION_SHOCK);
    container[ACTION_PROTECTION_POISON] = new EnchantSpell(Spell(Action(functions[ACTION_PROTECTION_POISON], visibility_range, 2, 0,
        STR_ACTION_PROTECTION_VENOM, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_BUFF),
        30, G_ET, G_CHA, G_RO, G_IN), ENCHANT_PROTECTION_POISON);
    container[ACTION_FORCEFIELD] = new EnchantSpell(Spell(Action(functions[ACTION_FORCEFIELD], visibility_range, 2, 2,
        STR_ACTION_FORCEFIELD, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_BUFF),
        60, G_UN, G_ET, G_ZO, G_CHA, G_UN, G_ET, G_ZO, G_CHA), ENCHANT_FORCE_FIELD);
    container[ACTION_HEAL_LESSER] = new Spell(Action(functions[ACTION_HEAL_LESSER], visibility_range, 2, 0,
        STR_ACTION_HEAL_LESSER, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        30, G_DO, G_UN, G_RO);
    container[ACTION_HEAL_GREATER] = new ChannelingSpell(Spell(Action(functions[ACTION_HEAL_GREATER], visibility_range/2 + 1, 2, 0,
        STR_ACTION_HEAL_GREATER, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_UNIT + AFLAG_IS_Channeling + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_BROKEN_BY_DAMAGE),
        0, G_RO, G_UN, G_DO), Action_ChannelingEffect(CONT_GREATER_HEAL, Channeling_functions[CONT_GREATER_HEAL], STR_CONT_GREATER_HEAL));
    container[ACTION_HASTE] = new EnchantSpell(Spell(Action(functions[ACTION_HASTE], visibility_range, 2, 0,
        STR_ACTION_HASTE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_BUFF),
        10, G_ET, G_CHA, G_CHA), ENCHANT_HASTE);
    container[ACTION_INVERSION] = new Spell(Action(functions[ACTION_INVERSION], visibility_range/2, 2, 0,
        STR_ACTION_INVERSION, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS),
        10, G_KA, G_IN);
    container[ACTION_MAGIC_MISSILE] = new ProjectileSpell(Spell(Action(functions[ACTION_MAGIC_MISSILE], visibility_range, 2, 0,
        STR_ACTION_MAGIC_MISSILE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF),
        20, G_ET, G_UN, G_CHA, G_UN), ENT_MAGIC_MISSILE, true);
    container[ACTION_MANA_DRAIN] = new ChannelingSpell(Spell(Action(functions[ACTION_MANA_DRAIN], visibility_range/2 + 1, 2, 0,
        STR_ACTION_MANA_DRAIN, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS + AFLAG_IS_Channeling + AFLAG_IS_BROKEN_BY_DAMAGE),
        0, G_UN, G_KA, G_ZO, G_IN), Action_ChannelingEffect(CONT_DRAIN_MANA, Channeling_functions[CONT_DRAIN_MANA], STR_CONT_DRAIN_MANA));
    container[ACTION_PULL] = new Spell(Action(functions[ACTION_PULL], visibility_range/2, 2, 0,
        STR_ACTION_PULL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS),
        10, G_UN, G_UN, G_ZO);
    container[ACTION_PUSH] = new Spell(Action(functions[ACTION_PUSH], visibility_range/2, 2, 0,
        STR_ACTION_PUSH, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_TARGETLESS),
        10, G_UN, G_UN, G_UN);
    container[ACTION_REFLECTIVE_SHIELD] = new EnchantSpell(Spell(Action(functions[ACTION_REFLECTIVE_SHIELD], 0, 2, 0,
        STR_ACTION_REFLECTIVE_SHIELD, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_IS_TARGETLESS),
        30, G_UN, G_ZO, G_ZO, G_RO, G_DO), ENCHANT_REFLECTIVE_SHIELD);
    container[ACTION_FIRE_RING] = new Spell(Action(functions[ACTION_FIRE_RING], 2, 2, 0,
        STR_ACTION_FIRE_RING, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_TARGETLESS),
        60, G_DO, G_ZO, G_RO, G_UN);
    container[ACTION_SHOCK] = new EnchantSpell(Spell(Action(functions[ACTION_SHOCK], visibility_range, 2, 0,
        STR_ACTION_SHOCK, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_BUFF),
        30, G_ZO, G_CHA, G_ET, G_ET), ENCHANT_SHOCK);
    container[ACTION_SLOW] = new EnchantSpell(Spell(Action(functions[ACTION_SLOW], visibility_range, 2, 0,
        STR_ACTION_SLOW, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_DEBUFF),
        15, G_ZO, G_ZO, G_ZO), ENCHANT_SLOW);
    container[ACTION_SWAP] = new ProjectileSpell(Spell(Action(functions[ACTION_SWAP], visibility_range, 2, 0,
        STR_ACTION_SWAP, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_BE_ANCHORED + AFLAG_CANT_TARGET_SELF),
        15, G_UN, G_UN, G_ZO, G_ZO), ENT_SWAPBALL, true);
    container[ACTION_TELEPORT] = new Spell(Action(functions[ACTION_TELEPORT], visibility_range, 2, 0,
        STR_ACTION_TELEPORT, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_MUST_TARGET_EMPTY + AFLAG_CANT_BE_ANCHORED),
        20, G_ZO, G_UN, G_ET, G_CHA);
    container[ACTION_WALL] = new Spell(Action(functions[ACTION_WALL], visibility_range, 2, 0,
        STR_ACTION_WALL, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_EMPTY + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET),
        30, G_KA, G_UN, G_IN);
    container[ACTION_FIST_OF_VENGEANCE] = new Spell(Action(functions[ACTION_FIST_OF_VENGEANCE], visibility_range, 2, 0,
        STR_ACTION_FIST_OF_VENGEANCE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE),
        60, G_KA, G_IN, G_UN, G_ZO);
    container[ACTION_VAMPIRISM] = new EnchantSpell(Spell(Action(functions[ACTION_VAMPIRISM], visibility_range, 2, 0,
        STR_ACTION_VAMPIRISM, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_BUFF),
        30, G_UN, G_ZO, G_ET, G_CHA), ENCHANT_VAMPIRISM);
    container[ACTION_STUN] = new EnchantSpell(Spell(Action(functions[ACTION_STUN], visibility_range, 2, 0,
        STR_ACTION_STUN, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_DEBUFF),
        10, G_KA, G_ZO), ENCHANT_STUN);
    container[ACTION_TOXIC_CLOUD] = new Spell(Action(functions[ACTION_TOXIC_CLOUD], visibility_range, 2, 0,
        STR_ACTION_TOXIC_CLOUD, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE),
        60, G_IN, G_RO, G_KA);
    container[ACTION_FORCE_OF_NATURE] = new ProjectileSpell(Spell(Action(functions[ACTION_FORCE_OF_NATURE], 0, 2, 2,
        STR_ACTION_FORCE_OF_NATURE, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE + AFLAG_CANT_TARGET_SELF),
        60, G_DO, G_RO, G_ZO, G_, G_, G_, G_, G_), ENT_FON, false);
    container[ACTION_PIXIES] = new EnchantSpell(Spell(Action(functions[ACTION_PIXIES], visibility_range, 2, 0,
        STR_ACTION_PIXIES, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_ENCHANT + AFLAG_IS_AGGRESSIVE + AFLAG_IS_TARGETLESS),
        30, G_ET, G_ZO, G_CHA, G_ZO), ENCHANT_PIXIES);
    container[ACTION_CHARM] = new ChannelingSpell(Spell(Action(functions[ACTION_CHARM], visibility_range/2 + 1, 2, 0,
        STR_ACTION_CHARM, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_Channeling + AFLAG_MUST_TARGET_UNIT + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF),
        10, G_KA, G_IN, G_ZO), Action_ChannelingEffect(CONT_CHARM, Channeling_functions[CONT_CHARM], STR_CONT_CHARM));
    container[ACTION_METEOR] = new Spell(Action(functions[ACTION_METEOR], visibility_range, 2, 0,
        STR_ACTION_METEOR, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_MUST_TARGET_VISIBLE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_IS_AGGRESSIVE),
        30, G_RO, G_RO);
    container[ACTION_POISON] = new EnchantSpell(Spell(Action(functions[ACTION_POISON], visibility_range, 2, 0,
        STR_ACTION_POISON, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_DEBUFF),
        10, G_IN, G_RO), ENCHANT_POISON);
    container[ACTION_NULLIFY] = new EnchantSpell(Spell(Action(functions[ACTION_POISON], visibility_range, 2, 0,
        STR_ACTION_NULLIFY, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_DEBUFF),
        10, G_IN, G_RO), ENCHANT_NULLIFICATION);
    container[ACTION_OBLITERATION] = new ChannelingSpell(Spell(Action(functions[ACTION_OBLITERATION], visibility_range, 2, 2,
        STR_ACTION_OBLITERATION, STR_CASTED_SPELL, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_SPELL + AFLAG_IS_AGGRESSIVE + AFLAG_IS_Channeling + AFLAG_IS_TARGETLESS),
        10, G_UN, G_UN, G_ET, G_ET, G_ZO, G_ZO, G_CHA, G_CHA), Action_ChannelingEffect(CONT_OBLITERATION, Channeling_functions[CONT_OBLITERATION], STR_CONT_OBLITERATION, false));
    container[ACTION_BERSERKER] = new Ability(Action(functions[ACTION_BERSERKER], visibility_range, 2, 0,
        STR_ACTION_BERSERKER, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_ABILITY + AFLAG_IS_AGGRESSIVE + AFLAG_MUST_TURN_TO_TARGET + AFLAG_CANT_TARGET_SELF),
        5, CD_BERSERKER);
    container[ACTION_HARPOON] = new Ability(Action(functions[ACTION_HARPOON], visibility_range/2, 2, 0,
        STR_ACTION_HARPOON, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_ABILITY + AFLAG_IS_AGGRESSIVE + AFLAG_CANT_TARGET_SELF + AFLAG_MUST_TURN_TO_TARGET),
        4, CD_HARPOON);
    container[ACTION_WARCRY] = new Ability(Action(functions[ACTION_WARCRY], visibility_range/2 + 1, 2, 0,
        STR_ACTION_WARCRY, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_ABILITY + AFLAG_IS_AGGRESSIVE + AFLAG_IS_TARGETLESS),
        5, CD_WARCRY);
    container[ACTION_HAMMER] = new Action(functions[ACTION_HAMMER], 1, 2, 2,
        STR_ACTION_HAMMER, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_IS_MELEE);
    container[ACTION_LONGSWORD] = new Action(functions[ACTION_LONGSWORD], 1, 2, 0,
        STR_ACTION_LONGSWORD, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_IS_MELEE);
    container[ACTION_MACE] = new Action(functions[ACTION_MACE], 1, 2, 0,
        STR_ACTION_MACE, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_IS_MELEE);
    container[ACTION_BOW] = new ProjectileAction(Action(functions[ACTION_BOW], 0, 2, 0,
        STR_ACTION_BOW, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO), ENT_ARROW, false);
    container[ACTION_FORCE_STAFF] = new ChannelingAction(Action(functions[ACTION_FORCE_STAFF], visibility_range/2 + 1, 2, 0,
        STR_ACTION_FORCE_STAFF, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO+AFLAG_MUST_TARGET_VISIBLE+AFLAG_MUST_TARGET_UNIT + AFLAG_IS_BROKEN_BY_DAMAGE),
        Action_ChannelingEffect(CONT_LIGHTNING_STAFF, Channeling_functions[CONT_LIGHTNING_STAFF], STR_CONT_LIGHTNING_STAFF));
    container[ACTION_FIRE_STAFF] = new ProjectileAction(Action(functions[ACTION_FIRE_STAFF], 0, 2, 0,
        STR_ACTION_FIRE_STAFF, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO), ENT_FIREBALL_SMALL, false);
    container[ACTION_HELLFIRE_STAFF] = new Action(functions[ACTION_HELLFIRE_STAFF], 0, 2, 0,
        STR_ACTION_HELLFIRE_STAFF, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO);
    container[ACTION_SHURIKEN] = new ProjectileAction(Action(functions[ACTION_SHURIKEN], 0, 2, 0,
        STR_ACTION_SHURIKEN, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO+AFLAG_IS_QUICK), ENT_SHURIKEN, false);
    container[ACTION_FIRESWORD] = new Action(functions[ACTION_FIRESWORD], 1, 2, 0,
        STR_ACTION_FIRESWORD, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_IS_MELEE+AFLAG_IS_QUICK);
    container[ACTION_FON_STAFF] = new ProjectileAction(Action(functions[ACTION_FON_STAFF], 0, 2, 2,
        STR_ACTION_FON_STAFF, NO_STRING, NO_STRING, NO_STRING, NO_STRING, NO_STRING,
        AFLAG_IS_AGGRESSIVE+AFLAG_CANT_TARGET_SELF+AFLAG_MUST_TURN_TO_TARGET+AFLAG_NEED_AMMO), ENT_FON, false);
}
void Loader::loadGestures(GestureContainer& container) {
    container.reserve(Counters::gestures);
    container.push_back(new Gesture(0));
    container.push_back(new Gesture(1));
    container.push_back(new Gesture(2));
    container.push_back(new Gesture(3));
    container.push_back(new Gesture(4));
    container.push_back(new Gesture(5));
    container.push_back(new Gesture(6));
    container.push_back(new Gesture(7));
    container.push_back(new Gesture(8));
}
void Loader::loadEnchants(EnchantContainer& container) {
    container.resize(Counters::enchants);

    container[NO_ENCHANT] = new Enchant(NO_STRING, NO_ENCHANT, 0);
    container[ENCHANT_ANCHOR] = new Enchant(STR_ENCHANT_ANCHOR, ENCHANT_ANCHOR, 4);
    container[ENCHANT_FORCE_FIELD] = new Enchant(STR_ENCHANT_FORCE_FIELD, ENCHANT_FORCE_FIELD, 50, false, false);
    container[ENCHANT_HASTE] = new Enchant(STR_ENCHANT_HASTE, ENCHANT_HASTE, 4);
    container[ENCHANT_PIXIES] = new Enchant(STR_ENCHANT_PIXIES, ENCHANT_PIXIES, 255, true, false);
    container[ENCHANT_POISON] = new Enchant(STR_ENCHANT_POISON, ENCHANT_POISON, 4);
    container[ENCHANT_PROTECTION_FIRE] = new Enchant(STR_ENCHANT_PROTECTION_FIRE, ENCHANT_PROTECTION_FIRE, 6);
    container[ENCHANT_PROTECTION_POISON] = new Enchant(STR_ENCHANT_PROTECTION_POISON, ENCHANT_PROTECTION_POISON, 6);
    container[ENCHANT_PROTECTION_SHOCK] = new Enchant(STR_ENCHANT_PROTECTION_SHOCK, ENCHANT_PROTECTION_SHOCK, 6);
    container[ENCHANT_REFLECTIVE_SHIELD] = new Enchant(STR_ENCHANT_REFLECTIVE_SHIELD, ENCHANT_REFLECTIVE_SHIELD, 255, false, false, true, true);
    container[ENCHANT_SLOW] = new Enchant(STR_ENCHANT_SLOW, ENCHANT_SLOW, 3);
    container[ENCHANT_STUN] = new Enchant(STR_ENCHANT_STUN, ENCHANT_STUN, 1);
    container[ENCHANT_VAMPIRISM] = new Enchant(STR_ENCHANT_VAMPIRISM, ENCHANT_VAMPIRISM, 6);
    container[ENCHANT_SHOCK] = new Enchant(STR_ENCHANT_SHOCK, ENCHANT_SHOCK, 6, true);
    container[ENCHANT_NULLIFICATION] = new Enchant(STR_ENCHANT_NULLIFICATION, ENCHANT_NULLIFICATION, 3);
}
