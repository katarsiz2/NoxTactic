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


Ability::Ability(const Action& action, int cooldown):
Action(action), cooldown(cooldown) {}



Action_ChannelingEffect::Action_ChannelingEffect(enumChannelingActions id, ptfChannelingAction Channeling_action,
    enumStrings performing_tip, bool is_dispellable):
    id(id), Channeling_action(Channeling_action), performing_tip(performing_tip), is_dispellable(is_dispellable) {}
Action_ProjectileLauncher::Action_ProjectileLauncher(enumEntities projectile_id, bool homing) :
projectile_id(projectile_id), homing(homing) {}

ProjectileSpell::ProjectileSpell(const Spell& spell, enumEntities id, bool homing):
Spell(spell), Action_ProjectileLauncher(id, homing) {}

ProjectileAction::ProjectileAction(const Action& action, enumEntities id, bool homing) : Action(action), Action_ProjectileLauncher(id, homing) {}

EnchantSpell::EnchantSpell(const Spell& spell, enumEnchants id) :
Spell(spell), id(id) {}


ChannelingAction::ChannelingAction(const Action& action, Action_ChannelingEffect cont_action):
Action(action), Action_ChannelingEffect(cont_action) {}
bool ChannelingAction::ApplyEffect(Battle* battle, Entity* actor, Entity* target) const {
    (battle->*Channeling_action)(actor, target, this);
    return true;
}

ChannelingSpell::ChannelingSpell(const Spell& spell, Action_ChannelingEffect cont_action):
Spell(spell), Action_ChannelingEffect(cont_action) {}
bool ChannelingSpell::ApplyEffect(Battle* battle, Entity* actor, Entity* target) const {
    (battle->*Channeling_action)(actor, target, this);
    return true;
}
