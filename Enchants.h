#pragma once
#include "Constants.h"
class Enchant
{
public:
    const enumStrings name;
    SoundHandler sound_on, sound_off, metasound;
    const int time_to_last, id;
    const bool does_wearoff_auto, is_dispellable, breaks_upon_moving, breaks_upon_aggressiveness;
    TextureHandler icon, texture;
    Enchant::Enchant(const enumStrings name, 
        const int id, const int time, bool is_dispellable = false, const bool wear_off_auto = true, 
        bool breaks_upon_moving = false, bool breaks_upon_aggressiveness = false):
        name(name), time_to_last(time), sound_off(EMPTY_SOUND), sound_on(EMPTY_SOUND),
    id(id), does_wearoff_auto(wear_off_auto), is_dispellable(is_dispellable),
    breaks_upon_moving(breaks_upon_moving), breaks_upon_aggressiveness(breaks_upon_aggressiveness),
    icon(EMPTY_TEXTURE), texture(EMPTY_TEXTURE) {}
    void SetIcon(TextureHandler icon) { this->icon = icon; }
    void SetTexture(TextureHandler texture) { this->texture = texture; }
    TextureHandler MyIcon() const { return icon; }
    TextureHandler MyTexture() const { return texture; }
};