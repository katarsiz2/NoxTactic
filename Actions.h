#pragma once
#ifndef ACTIONS_DEF
#define ACITONS_DEF
//#include "Core.h"
#include "Constants.h"
#include <vector>
#include "Strings.h"
using std::vector;
class Battle;
class Gesture
{
    SoundHandler sound;
    int powernumber;
public:
    Gesture(const int number): powernumber(number) {}
    void SetSound(SoundHandler sound) { this->sound = sound; }
};

class Action_ChannelingEffect
{
protected:
    enumChannelingActions id;
    ptfChannelingAction Channeling_action; //func to be called 
    bool is_dispellable;
    enumStrings performing_tip;
public:
    Action_ChannelingEffect(enumChannelingActions id, ptfChannelingAction Channeling_action, enumStrings performing_tip, bool is_dispellable = true);
    enumChannelingActions ID() const { return id; }
    virtual bool ApplyEffect(Battle* battle, Entity* actor, Entity* target) const { return true; }
    bool IsDispellable() const { return is_dispellable; }
};
class Action_ProjectileLauncher
{
protected:
    enumEntities projectile_id;
    bool homing;
public:
    Action_ProjectileLauncher(enumEntities projectile_id, bool homing);
    bool IsHoming() const { return homing; }
    enumEntities ProjectileID() const { return projectile_id; }
};
class Action
{
protected:
    int range, action_points_cost, time_to_perform;
    SoundHandler sound;
    enumStrings name, msg_performed, msg_started, msg_stopped, msg_affected, msg_killed; //TODO: add logging to these
    long flags;
    TextureHandler icon_texture_id;
    TextureHandler active_icon_texture_id;
    ptfAction action; //func to be called 
public:
    Action(ptfAction action,
        int range, int action_points_cost, int performtime,
        enumStrings name, enumStrings msg_done, enumStrings msg_started,
        enumStrings msg_stopped, enumStrings msg_affected, enumStrings msg_killed, long flags);
    int Range() const { return range; }
    bool Flag(const long flag) const { return ((flags & flag) != 0); }
    bool IsLengthy() const { return time_to_perform != 0; }
    int TimeToPerform() const { return time_to_perform; }
    int APCost() const { return action_points_cost; }
    TextureHandler MyIcon() const { return icon_texture_id; }
    TextureHandler MyActiveIcon() const { return active_icon_texture_id; }
    SoundHandler MySound() const { return sound; }
    enumStrings Name() const { return name; }
    virtual enumStrings PerformingTip() const { return NO_STRING; }
    virtual enumChannelingActions ChannelingID() const { return CONT_NOTHING; }
    void SetIcon(TextureHandler texture) { icon_texture_id = texture; }
    void SetActiveIcon(TextureHandler texture) { active_icon_texture_id = texture; }
    void SetSound(SoundHandler sound) { this->sound = sound; }
    virtual void Perform(Battle* battle, Entity* actor, const CoordI& target) const;
};
class ChannelingAction: public Action, public Action_ChannelingEffect
{
public:
    ChannelingAction(const Action& action, Action_ChannelingEffect cont_action);
    virtual bool ApplyEffect(Battle* battle, Entity* actor, Entity* target) const;
    virtual enumStrings PerformingTip() const { return performing_tip; }
    virtual enumChannelingActions ChannelingID() const { return ID(); }
};

class Spell: public Action
{
protected:
    enumGestures gesture[8];
    const int manacost;
public:
    Spell(const Action& action, int manacost, enumGestures g1, enumGestures g2 = G_, enumGestures g3 = G_,
        enumGestures g4 = G_, enumGestures g5 = G_, enumGestures g6 = G_, enumGestures g7 = G_, enumGestures g8 = G_);
    int Manacost() const { return manacost; }
    virtual void Perform(Battle* battle, Entity* actor, const CoordI& target) const;
    friend Action;
};
class ChannelingSpell: public Spell, public Action_ChannelingEffect
{
public:
    ChannelingSpell(const Spell& spell, Action_ChannelingEffect cont_action);
    virtual bool ApplyEffect(Battle* battle, Entity* actor, Entity* target) const;
    virtual enumStrings PerformingTip() const { return performing_tip; }
    virtual enumChannelingActions ChannelingID() const { return ID(); }
};
class ProjectileSpell: public Spell, public Action_ProjectileLauncher
{
public:
    ProjectileSpell(const Spell& spell, enumEntities id, bool homing);
};
class ProjectileAction: public Action, public Action_ProjectileLauncher
{
public:
    ProjectileAction(const Action& action, enumEntities id, bool homing);
};
class EnchantSpell: public Spell
{
    enumEnchants id;
public:
    EnchantSpell(const Spell& spell, enumEnchants id);
    enumEnchants EnchantID() const { return id; }
};
class Ability: public Action
{
    const int cooldown;
public:
    Ability(const Action& action, int cooldown);
    int Cooldown() const { return cooldown; }
};

#endif