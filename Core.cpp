#include "Core.h"
#include "Entity.h"
#include "Constants.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <Windows.h>
#include <WindowsX.h>
#pragma once
#pragma warning (disable:4244)
extern EngineCore *core;
using namespace ContainerDefs;
using namespace GameBaseConstants;
#undef PlaySound    //windows define, not needed
Game* game;         //not sure if it should be global
enumActions ContToCommon(enumChannelingActions id) {
    switch (id) {
    case CONT_LIGHTNING:
        return ACTION_LIGHTNING;
    case CONT_ENERGY_BOLT:
        return ACTION_ENERGY_BOLT;
    case CONT_GREATER_HEAL:
        return ACTION_HEAL_GREATER;
    case CONT_CHARM:
        return ACTION_CHARM;
    case CONT_DRAIN_MANA:
        return ACTION_MANA_DRAIN;
    case CONT_OBLITERATION:
        return ACTION_OBLITERATION;
    case CONT_CHANNEL_LIFE:
        return ACTION_CHANNEL;
    case CONT_LIGHTNING_STAFF:
        return ACTION_FORCE_STAFF;
    default:
        return NO_ACTION;
    };
}
CoordD ProjectileStep(const Angle& angle) {   
    switch (angle.getDegrees()) {
    case 0:
        return CoordD(1.0, 0);
    case 90:
        return CoordD(0, 1.0);
    case 180:
        return CoordD(-1.0, 0);
    case 270:
        return CoordD(0, -1.0);
    };
    switch (angle.getMainDirection()) {
    case RIGHT:
        return CoordD(1.0, angle.tg());
    case LEFT:
        return CoordD(-1.0, -angle.tg());
    case UP:
        return CoordD(-1/angle.tg(), -1.0);
    case DOWN:
        return CoordD(1/angle.tg(), 1.0);
    }
    return CoordD();
}
//#define EPS 1e-7
CoordI GetRoundedCoor(const CoordD& excoor) {
    CoordI tmp;
    if (abs(excoor.x - ceil(excoor.x)) < EPS) {
        tmp.x = ceil(excoor.x);
    } else {
        tmp.x = floor(excoor.x);
    }
    if (abs(excoor.y - ceil(excoor.y)) < EPS) {
        tmp.y = ceil(excoor.y);
    } else {
        tmp.y = floor(excoor.y);
    }
    return tmp;
}

void Game::MainLoop() {
    MSG msg;
    bool is_done = false;
    while (!is_done) {
        frame();
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            switch (msg.message) {
            case WM_QUIT:
                is_done = true;
            };
        }
    }
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nShow){
    try {
        core = new Game(EngineCore(hInstance, "NoxTactic", "NoxTactic", Paths::PicturesPath + Paths::CursorPath, Paths::PicturesPath + Paths::IconPath,
            800, 600, 100, 100));

        game = dynamic_cast<Game*>(core);
        core->setGraphicCore(new Graphics(game));
        game->Graphic()->init();
        game->SetInput(new Input());
        game->Input()->setDefaultKeyBindings();
        game->setSoundCore(new SoundCore());

        core->init();
        if (use_mapeditor) {
            game->StartMapEditor(new MapEditor(CoordI(10, 10), game));
            game->MainLoop();
            game->EndMapEditor();
        } else {
            game->StartBattle(new Battle(game->DefMap(0), game));
            game->MainLoop();
            game->EndBattle(); //HACK: this will be wrong later
        }

        game->shutdown();
        delete core;
        PostQuitMessage(0);
    }
    catch (string c) {
        system("pause");
    }
}

void Game::InitEnt(DefaultEntity* defent) {
    std::vector<TextureHandler> textures;
    std::vector<SoundHandler> sounds;
    if (defent->Name() != NO_STRING && !defent->Flag(ENT_IS_INVISIBLE)) {
        if (defent->Flag(ENT_IS_OMNIDIRECTIONAL)) {
            if (defent->Flag(ENT_IS_OBELISK)) {
                for (int i = 1; i <= obelisk_picture_states; ++i) {
                    textures.push_back(Graphic()->addTexture(Paths::EntitiesPictures + Strings[defent->Name()] + int_to_string(i) + common_image_extension, ColorWhite));
                }
            } else {
                if (defent->getAnimationFrames() != 0) {
                    textures.push_back(Graphic()->addAnimatedTexture(Paths::EntitiesPictures + Strings[defent->Name()]+"\\",
                        common_image_extension, defent->getAnimationFrames(), 100, ColorWhite));
                } else {
                    textures.push_back(Graphic()->addTexture(Paths::EntitiesPictures + Strings[defent->Name()] + common_image_extension, ColorWhite));
                }
            }
        } else {
            for (int i = 1; i <= Counters::directions; ++i) {
                textures.push_back(Graphic()->addTexture(Paths::EntitiesPictures + Strings[defent->Name()] + int_to_string(i) + common_image_extension, ColorWhite));
            }
        }
    }
    sounds.resize(2, EMPTY_SOUND);
    defent->SetTextures(textures);
    defent->SetSounds(sounds); //TODO: set proper sounds
}
void Game::InitAction(Action* action) {
    if (action->Name() != NO_STRING) {
        action->SetIcon(Graphic()->addTexture(Paths::ActionsIcons + String(action->Name()) + common_image_extension, ColorWhite));
        action->SetActiveIcon(Graphic()->addTexture(Paths::ActionsActiveIcons + String(action->Name()) + common_image_extension, ColorWhite));
        action->SetSound(Sound()->AddSound(1, getAppPath() + Paths::ActionSounds + String(action->Name()), common_sound_extension));
    }
}
void Game::InitEnchant(Enchant* enchant) {
    enchant->SetTexture(Graphic()->addTexture(Paths::EnchantsPictures + String(enchant->name) + common_image_extension, ColorWhite));
    enchant->SetIcon(Graphic()->addTexture(Paths::EnchantsIcons + String(enchant->name) + common_image_extension, ColorBlack));
    //TODO: implement enchants sounds
}
void Game::InitWall(Wall* wall) {
    wall->SetTexture(Graphic()->addTexture(Paths::WallsPictures + String(wall->Name()) + common_image_extension, ColorWhite));
}
void Game::InitTile(Tile* tile) {
    tile->SetTexture(Graphic()->addTexture(Paths::TilesPictures + String(tile->Name()) + common_image_extension, ColorWhite));
}
void Game::InitButton(InterfaceStructure::Button& button)
{
    button.pic = Graphic()->addPicture(button.position, button.size, Graphics::ELEMENT_BUTTON);
    button.label = Graphic()->addLabel(button.label_position, CoordI(200, 20));
    Input()->addButton(button.position, button.size, button.button_id);

    Graphic()->setPictureTexture(button.pic, button.texture->base);
    Graphic()->setPictureActiveTexture(button.pic, button.texture->active);
    Graphic()->setDisabledTexture(button.pic, button.texture->disabled);
    Graphic()->setLabelText(button.label, button.caption);
}
void Game::RefreshScreen() {
    if (battle) {
        Graphic()->paint(battle->renderer.MapScreenPosition, battle->renderer.MapScreenPosition + battle->renderer.FieldWindowSize - CoordI(1, 1));
    } else {
        Graphic()->paint(mapeditor->renderer.MapScreenPosition, mapeditor->renderer.MapScreenPosition + mapeditor->renderer.FieldWindowSize - CoordI(1, 1));
    }
}
void Game::Delay(long milliseconds) {
    RefreshScreen();
    Sleep(milliseconds);
}

void Game::StartBattle(Battle* battle) {
    this->battle = battle;
    Graphic()->setMapSize(battle->Grid().Size());
    Input()->setField(picsize, CoordL(0, 0), field_window_size);
    battle->renderer.RefreshScreen();
}
void Game::EndBattle() {
    delete battle;
}
void Game::EndMapEditor() {
    delete mapeditor;
}
void Game::StartMapEditor(MapEditor* mapeditor) {
    this->mapeditor = mapeditor;
    Graphic()->setPictureVisibility(game->Interface.EndTurnButton.pic, false);
    Graphic()->setLabelVisibility(game->Interface.EndTurnButton.label, false);
    Graphic()->setPictureVisibility(game->Interface.Flag, false);
    //Graphic()->setPictureVisibility(game->Interface.objectlist_selector, true);

    Graphic()->setMapSize(mapeditor->Grid().Size());
    Input()->setField(picsize, CoordL(0, 0), field_window_size);
    Input()->activateButton(BUTTON_OBJECTLIST);
    Input()->deactivateButton(BUTTON_ENDTURN);
    Input()->deactivateButton(BUTTON_CHARMICON);
    mapeditor->renderer.RefreshScreen();
}

void Game::SetInput(::Input* input) {
    this->input = input;
}
Game::Game(EngineCore engine): EngineCore(engine), battle(nullptr), mapeditor(nullptr), input(nullptr) {}
template <class Container>
void FreeContainer(Container& container) {
   for (Container::iterator it = container.begin(); it != container.end(); ++it) {
       delete (*it);
   }
}

typedef Graphics::Rectangle Rect;
namespace InterfaceConstants
{
    const CoordI spellbar_coor = CoordI(100, 440);
    const Rect spellbar_base = Rect(spellbar_coor+CoordI(40, 20), CoordI(200, 54));
    const Rect spellbar_right = Rect(spellbar_coor+CoordI(237, 20), CoordI(43, 50));
    const Rect spellbar_title = Rect(spellbar_coor+CoordI(80, 0), CoordI(124, 21));
    const Rect spellbar_title_caption = Rect(spellbar_coor+CoordI(100, 0), CoordI(200, 20));
    const Rect spellbar_left = Rect(spellbar_coor+CoordI(0, 20), CoordI(43, 50));
    const Rect spellbar_scrollup_button = Rect(spellbar_left.coor, CoordI(33, 32));
    const Rect spellbar_scrolldown_button = Rect(spellbar_left.coor+CoordI(0,32), CoordI(33, 32));

    const CoordI spellbar_icons_startpos = CoordI(50, 33);
    const CoordI spellbar_icons_size = CoordI(32, 32);
    const CoordI spellbar_nuggets_size = CoordI(17, 55);
    const CoordI spellbar_icons_indent = CoordI(5, 0);
    const CoordI spellbar_nuggets_shift_from_icons = CoordI(8, -13);

    const CoordI EntInfoStart = CoordI(510, 30);
    const Rect entinfo_portrait = Rect(EntInfoStart, CoordI(50, 50));

    const CoordI EntInfoLabelsStart = EntInfoStart + CoordI(0, entinfo_portrait.size.y);
    const CoordI EntInfoLabelIndent = CoordI(0, 20);
    const CoordI EntInfoLabelSize = CoordI(300, 20);

    const int EndturnButton_type = 0;
    const Rect EndTurnButton = Rect(CoordI(500, 500), CoordI(100, 40));
    const Rect EndTurnButtonLabel = Rect(CoordI(520, 510), CoordI(200, 20));

    const Rect Flag = Rect(CoordI(610, 500), CoordI(50, 50));

    const CoordI charms_coor = CoordI(510, 300);
    const CoordI charms_elementsize = CoordI(32, 32);

    const CoordI miniobjectlist_coor = CoordI(510, 0);
    const CoordI miniobjectlist_elementsize = CoordI(50, 50);
    const CoorArray2D<int> miniobjectlist(miniobjectlist_coor, miniobjectlist_elementsize, objectlist_mini_elements_per_row, RIGHT, DOWN, 0, 0);
};

void Game::init(){
    auto gr = Graphic();
    gr->setTexturesPath(getAppPath() + Paths::PicturesPath);
    gr->addTexture(Paths::NoIcon);
    TextureHolder.Selection_Texture = gr->addTexture(Paths::SelectionFrame, ColorWhite);

    for (char i = 0; i < Counters::directions; ++i) {
        TextureHolder.MoveArrows_Texture[i] = gr->addTexture(Paths::MoveArrow + int_to_string(i + 1) + common_image_extension, ColorWhite);
    }
    {
        TextureHolder.Actionbar_base = gr->addTexture(Paths::ActionBar_base, ColorWhite, 1.0, 0.2578125);
        Interface.spellbar_base = gr->addPicture(InterfaceConstants::spellbar_base, Graphics::ELEMENT_PICTURE);
        gr->setPictureTexture(Interface.spellbar_base, TextureHolder.Actionbar_base);

        TextureHolder.Actionbar_title = gr->addTexture(Paths::ActionBar_title, ColorWhite, 1.0, 0.1640625);
        Interface.spellbar_titlebar = gr->addPicture(InterfaceConstants::spellbar_title, Graphics::ELEMENT_PICTURE);
        gr->setPictureTexture(Interface.spellbar_titlebar, TextureHolder.Actionbar_title);
        Interface.spellbar_title = gr->addLabel(InterfaceConstants::spellbar_title_caption);
        
        for (int i = 0; i < actions_per_set; i++) {
            TextureHolder.Actionbar_nuggets[i] = gr->addTexture(Paths::ActionBar_nugget+int_to_string(i+1)+common_image_extension, ColorWhite, 0.296875, 1.0);
            Interface.spell_icons[i] = gr->addPicture(InterfaceConstants::spellbar_coor + InterfaceConstants::spellbar_icons_startpos + 
                i * (CoordI(InterfaceConstants::spellbar_icons_size.x, 0) + InterfaceConstants::spellbar_icons_indent), InterfaceConstants::spellbar_icons_size, Graphics::ELEMENT_SPELLICON);
            Input()->addButton(InterfaceConstants::spellbar_coor + InterfaceConstants::spellbar_icons_startpos +
                i * (CoordI(InterfaceConstants::spellbar_icons_size.x, 0) + InterfaceConstants::spellbar_icons_indent), 
                InterfaceConstants::spellbar_icons_size, BUTTON_SPELLICON, i+1);
            Interface.nuggets[i] = gr->addPicture(InterfaceConstants::spellbar_coor + InterfaceConstants::spellbar_icons_startpos + InterfaceConstants::spellbar_nuggets_shift_from_icons +
                i * (CoordI(InterfaceConstants::spellbar_icons_size.x, 0) + InterfaceConstants::spellbar_icons_indent), InterfaceConstants::spellbar_nuggets_size, Graphics::ELEMENT_PICTURE);
            gr->setPictureTexture(Interface.nuggets[i], TextureHolder.Actionbar_nuggets[i]);
        }

        Input()->addButton(InterfaceConstants::spellbar_scrollup_button.coor, InterfaceConstants::spellbar_scrollup_button.size, BUTTON_SPELLSCROLL_UP);
        Input()->addButton(InterfaceConstants::spellbar_scrolldown_button.coor, InterfaceConstants::spellbar_scrolldown_button.size, BUTTON_SPELLSCROLL_DOWN);

        TextureHolder.Actionbar_bomber = gr->addTexture(Paths::ActionBar_right_bomber, ColorWhite, 0.859375, 1.0);
        TextureHolder.Actionbar_bomberactive = gr->addTexture(Paths::ActionBar_right_bomber_active, ColorWhite, 0.859375, 1.0);
        TextureHolder.Actionbar_leftblank = gr->addTexture(Paths::ActionBar_left_blank, ColorWhite, 0.84, 1.0);
        TextureHolder.Actionbar_rightblank = gr->addTexture(Paths::ActionBar_right_blank, ColorWhite, 0.859375, 1.0);
        TextureHolder.Actionbar_leftscroll = gr->addTexture(Paths::ActionBar_left_scroll, ColorWhite, 0.84, 1.0);
        TextureHolder.Actionbar_leftscrolldown = gr->addTexture(Paths::ActionBar_left_scroll_active_down, ColorWhite, 0.84, 1.0);
        TextureHolder.Actionbar_leftscrollup = gr->addTexture(Paths::ActionBar_left_scroll_active_up, ColorWhite, 0.84, 1.0);
        TextureHolder.Actionbar_spellbook = gr->addTexture(Paths::ActionBar_spellbook, ColorWhite);
        TextureHolder.Actionbar_spellbookactive = gr->addTexture(Paths::ActionBar_spellbook_active, ColorWhite);
        TextureHolder.Actionbar_trap = gr->addTexture(Paths::ActionBar_right_trap, ColorWhite, 0.859375, 1.0);
        TextureHolder.Actionbar_trapactive = gr->addTexture(Paths::ActionBar_right_trap_active, ColorWhite, 0.859375, 1.0);

        Interface.spellbar_left = gr->addPicture(InterfaceConstants::spellbar_left, Graphics::ELEMENT_PICTURE);
        gr->setPictureTexture(Interface.spellbar_left, TextureHolder.Actionbar_leftscroll);
        Interface.spellbar_right = gr->addPicture(InterfaceConstants::spellbar_right, Graphics::ELEMENT_PICTURE);
        gr->setPictureTexture(Interface.spellbar_right, TextureHolder.Actionbar_rightblank);

        {
            TextureHolder.buttons_common[0].base = gr->addTexture(Paths::ButtonsFolder + Paths::ButtonCommon+"1"+common_image_extension, ColorWhite, 1, 0.23158);
            TextureHolder.buttons_common[0].active = gr->addTexture(Paths::ButtonsActiveFolder + Paths::ButtonCommon+"1"+common_image_extension, ColorWhite, 1, 0.23158);
            TextureHolder.buttons_common[0].disabled = gr->addTexture(Paths::ButtonsDisabledFolder + Paths::ButtonCommon+"1"+common_image_extension, ColorWhite, 1, 0.23158);
            TextureHolder.buttons_common[1].base = gr->addTexture(Paths::ButtonsFolder + Paths::ButtonCommon+"2"+common_image_extension, ColorWhite, 1, 0.2);
            TextureHolder.buttons_common[1].active = gr->addTexture(Paths::ButtonsActiveFolder + Paths::ButtonCommon+"2"+common_image_extension, ColorWhite, 1, 0.2);
            TextureHolder.buttons_common[1].disabled = gr->addTexture(Paths::ButtonsDisabledFolder + Paths::ButtonCommon+"2"+common_image_extension, ColorWhite, 1, 0.2);

            TextureHolder.buttons_radio[0].base = gr->addTexture(Paths::ButtonsFolder + Paths::ButtonRadio+"1"+common_image_extension, ColorWhite);
            TextureHolder.buttons_radio[0].active = gr->addTexture(Paths::ButtonsActiveFolder + Paths::ButtonRadio+"1"+common_image_extension, ColorWhite);
            TextureHolder.buttons_radio[0].disabled = gr->addTexture(Paths::ButtonsDisabledFolder + Paths::ButtonRadio+"1"+common_image_extension, ColorWhite);
            TextureHolder.buttons_radio[1].base = gr->addTexture(Paths::ButtonsFolder + Paths::ButtonRadio+"2"+common_image_extension, ColorWhite, 1, 0.64);
            TextureHolder.buttons_radio[1].active = gr->addTexture(Paths::ButtonsActiveFolder + Paths::ButtonRadio+"2"+common_image_extension, ColorWhite, 1, 0.64);
            TextureHolder.buttons_radio[1].disabled = gr->addTexture(Paths::ButtonsDisabledFolder + Paths::ButtonRadio+"2"+common_image_extension, ColorWhite, 1, 0.64);

            TextureHolder.button_cross.base = gr->addTexture(Paths::ButtonsFolder + Paths::ButtonCross, ColorWhite);
            TextureHolder.button_cross.active = gr->addTexture(Paths::ButtonsActiveFolder + Paths::ButtonCross, ColorWhite);
            TextureHolder.button_cross.disabled = gr->addTexture(Paths::ButtonsDisabledFolder + Paths::ButtonCross, ColorWhite);
        }
        

        Interface.EndTurnButton = InterfaceStructure::Button(&TextureHolder.buttons_common[InterfaceConstants::EndturnButton_type], 
            InterfaceConstants::EndTurnButton.coor, InterfaceConstants::EndTurnButton.size, CoordI(20, 10), "End Turn", BUTTON_ENDTURN);
        InitButton(Interface.EndTurnButton);

        

        for (int i = 0; i < max_flags; ++i) {
            TextureHolder.Flags[i] = gr->addTexture(Paths::FlagsFolder+int_to_string(i+1)+common_image_extension, ColorWhite);
            TextureHolder.Miniflags[i] = gr->addTexture(Paths::MiniFlagsFolder+int_to_string(i+1)+common_image_extension, ColorWhite);
        }
    }
    Interface.EntInfo.portrait = gr->addPicture(InterfaceConstants::entinfo_portrait, Graphics::ELEMENT_PICTURE);
    Interface.EntInfo.name = gr->addLabel(InterfaceConstants::EntInfoLabelsStart + InterfaceConstants::EntInfoLabelIndent, InterfaceConstants::EntInfoLabelSize);
    Interface.EntInfo.hp = gr->addLabel(InterfaceConstants::EntInfoLabelsStart + InterfaceConstants::EntInfoLabelIndent*2, InterfaceConstants::EntInfoLabelSize, Color(255, 0, 0));
    Interface.EntInfo.mp = gr->addLabel(InterfaceConstants::EntInfoLabelsStart + InterfaceConstants::EntInfoLabelIndent*3, InterfaceConstants::EntInfoLabelSize, Color(0, 0, 255));
    Interface.EntInfo.speed = gr->addLabel(InterfaceConstants::EntInfoLabelsStart + InterfaceConstants::EntInfoLabelIndent*4, InterfaceConstants::EntInfoLabelSize, Color(200, 200, 0));
    Interface.EntInfo.can_act = gr->addLabel(InterfaceConstants::EntInfoLabelsStart + InterfaceConstants::EntInfoLabelIndent*5, InterfaceConstants::EntInfoLabelSize);
    Interface.EntInfo.ammo = gr->addLabel(InterfaceConstants::EntInfoLabelsStart + InterfaceConstants::EntInfoLabelIndent*6, InterfaceConstants::EntInfoLabelSize);

    {
        CoorArray2D<int> enchants(InterfaceConstants::charms_coor, InterfaceConstants::charms_elementsize, charms_per_row, RIGHT, DOWN, 0, 0);
        for (int i = 0; i < max_charms; ++i) {
            Interface.EntInfo.enchantbar.push_back(gr->addPicture(enchants.getCoor(i), InterfaceConstants::charms_elementsize, Graphics::ELEMENT_PICTURE));
            Input()->addButton(enchants.getCoor(i), InterfaceConstants::charms_elementsize, BUTTON_CHARMICON, i+1);
        }
    }
    Interface.EntInfo.status_tip_icon = gr->addPicture(InterfaceConstants::charms_coor + CoordI(0, InterfaceConstants::charms_elementsize.y*max_charms/charms_per_row + 11), InterfaceConstants::charms_elementsize, Graphics::ELEMENT_PICTURE);
    Interface.EntInfo.status_tip = gr->addLabel(InterfaceConstants::charms_coor + CoordI(0, InterfaceConstants::charms_elementsize.y*(max_charms/charms_per_row+1) + 11), CoordI(300, 20));

    Interface.Flag = gr->addPicture(InterfaceConstants::Flag, Graphics::ELEMENT_PICTURE);
    gr->setPictureTexture(Interface.Flag, TextureHolder.Flags[0]);

    {
        for (int i = 0; i < objectlist_mini_size; ++i) {
            Interface.objectlist_mini.push_back(gr->addPicture(InterfaceConstants::miniobjectlist.getCoor(i), InterfaceConstants::miniobjectlist_elementsize, Graphics::ELEMENT_PICTURE));
            Input()->addButton(InterfaceConstants::miniobjectlist.getCoor(i), InterfaceConstants::miniobjectlist_elementsize, BUTTON_OBJECTLIST, i+1);
        }
        Input()->deactivateButton(BUTTON_OBJECTLIST);
    }
    Interface.objectlist_selector = gr->addPicture(Coord0, picsize, Graphics::ELEMENT_PICTURE);
    gr->setPictureTexture(Interface.objectlist_selector, TextureHolder.Selection_Texture);
    gr->setPictureVisibility(Interface.objectlist_selector, false);

    FunctionContainer actions;
    ChannelingFunctionContainer Channeling_actions;
    CollisionFunctionContainer collisions;
    TriggerFunctionContainer triggers;
    DieFunctionContainer diefuncs;
    {
        actions.resize(Counters::actions);
        Channeling_actions.resize(Counters::Channeling_actions);
        collisions.resize(Counters::projectiles);
        triggers.resize(Counters::triggers);
        diefuncs.resize(Counters::diefuncs);

        actions[NO_ACTION] = nullptr;

        actions[ACTION_FIREBALL] = nullptr;
        actions[ACTION_ANCHOR] = nullptr;
        actions[ACTION_BURN] = &Battle::Burn;
        actions[ACTION_CHANNEL] = nullptr;
        actions[ACTION_COUNTERSPELL] = &Battle::Counterspell;
        actions[ACTION_DEATHRAY] = &Battle::Deathray;
        actions[ACTION_DISPELL_UNDEAD] = &Battle::DispellUndead;
        actions[ACTION_ENERGY_BOLT] = nullptr;
        actions[ACTION_EARTHQUAKE] = &Battle::Earthquake;
        actions[ACTION_LIGHTNING] = nullptr;
        actions[ACTION_PROTECTION_FIRE] = nullptr;
        actions[ACTION_PROTECTION_SHOCK] = nullptr;
        actions[ACTION_PROTECTION_POISON] = nullptr;
        actions[ACTION_FORCEFIELD] = nullptr;
        actions[ACTION_HEAL_LESSER] = &Battle::LesserHeal;
        actions[ACTION_HEAL_GREATER] = nullptr;
        actions[ACTION_HASTE] = nullptr;
        actions[ACTION_INVERSION] = &Battle::Inversion;
        actions[ACTION_MAGIC_MISSILE] = nullptr;
        actions[ACTION_MANA_DRAIN] = nullptr;
        actions[ACTION_PULL] = &Battle::Pull;
        actions[ACTION_PUSH] = &Battle::Push;
        actions[ACTION_REFLECTIVE_SHIELD] = &Battle::ReflectiveShield;
        actions[ACTION_FIRE_RING] = &Battle::RingOfFire;
        actions[ACTION_SHOCK] = nullptr;
        actions[ACTION_SLOW] = nullptr;
        actions[ACTION_SWAP] = nullptr;
        actions[ACTION_TELEPORT] = &Battle::TeleportToTarget;
        actions[ACTION_WALL] = &Battle::MagicWall;
        actions[ACTION_FIST_OF_VENGEANCE] = &Battle::FistOfVengeance;
        actions[ACTION_VAMPIRISM] = nullptr;
        actions[ACTION_STUN] = nullptr;
        actions[ACTION_TOXIC_CLOUD] = &Battle::ToxicCloud;
        actions[ACTION_FORCE_OF_NATURE] = nullptr;
        actions[ACTION_PIXIES] = nullptr;
        actions[ACTION_CHARM] = nullptr;
        actions[ACTION_METEOR] = &Battle::Meteor;
        actions[ACTION_POISON] = nullptr;
        actions[ACTION_OBLITERATION] = nullptr;
        actions[ACTION_NULLIFY] = nullptr;
        actions[ACTION_BERSERKER] = &Battle::Berserker;
        actions[ACTION_HARPOON] = &Battle::Harpoon;
        actions[ACTION_WARCRY] = &Battle::Warcry;
        actions[ACTION_HAMMER] = &Battle::HammerStrike;
        actions[ACTION_LONGSWORD] = &Battle::MeleeStrike;
        actions[ACTION_MACE] = &Battle::MeleeStrike;
        actions[ACTION_BOW] = nullptr;
        actions[ACTION_FORCE_STAFF] = nullptr;
        actions[ACTION_FIRE_STAFF] = nullptr;
        actions[ACTION_HELLFIRE_STAFF] = &Battle::HellfireStaff;
        actions[ACTION_SHURIKEN] = nullptr;
        actions[ACTION_FIRESWORD] = &Battle::MeleeStrike;
        actions[ACTION_FON_STAFF] = nullptr;

        Channeling_actions[CONT_LIGHTNING] = &Battle::LightningApply;
        Channeling_actions[CONT_ENERGY_BOLT] = &Battle::EnergyBoltApply;
        Channeling_actions[CONT_GREATER_HEAL] = &Battle::GreaterHealApply;
        Channeling_actions[CONT_CHARM] = &Battle::CharmApply;
        Channeling_actions[CONT_DRAIN_MANA] = &Battle::DrainManaApply;
        Channeling_actions[CONT_OBLITERATION] = &Battle::ObliterationApply;
        Channeling_actions[CONT_CHANNEL_LIFE] = &Battle::ChannelApply;
        Channeling_actions[CONT_LIGHTNING_STAFF] = &Battle::LightningStaffApply;

        collisions[PROJECTILE_ARROW] = &Battle::ArrowCollide;
        collisions[PROJECTILE_DISPELLER_LIGHT] = &Battle::DispellerCollide;
        collisions[PROJECTILE_FON] = &Battle::EmptyCollide;
        collisions[PROJECTILE_FON_SMALL] = &Battle::EmptyCollide;
        collisions[PROJECTILE_FIREBALL] = &Battle::FireballCollide;
        collisions[PROJECTILE_FIREBALL_SMALL] = &Battle::FireballCollide;
        collisions[PROJECTILE_MAGIC_MISSILE] = &Battle::MissileCollide;
        collisions[PROJECTILE_SHURIKEN] = &Battle::ShurikenCollide;
        collisions[PROJECTILE_ENCHANTBALL] = &Battle::EnchantballCollide;
        collisions[PROJECTILE_HEALBALL] = &Battle::HealballCollide;
        collisions[PROJECTILE_SWAPBALL] = &Battle::SwapballCollide;
        collisions[PROJECTILE_PIXIE_SWARM] = &Battle::PixieCollide;
        collisions[PROJECTILE_FIRERING_FLAME] = &Battle::FireringCollide;

        diefuncs[DIEFUNC_FIREBALL] = &Battle::FireballDie;
        diefuncs[DIEFUNC_FIREBALLSMALL] = &Battle::FireballSmallDie;
        diefuncs[DIEFUNC_METEORSHADOW] = &Battle::MeteorShadowDie;
        diefuncs[DIEFUNC_FISTSHADOW] = &Battle::FistShadowDie;
        diefuncs[DIEFUNC_FON] = &Battle::FONDie;

        triggers[TRIGGER_TELEPORT_PENTAGRAM] = &Battle::TeleportPentagramTrigger;
        triggers[TRIGGER_TOXIC_CLOUD] = &Battle::ToxicCloudTrigger;
        triggers[TRIGGER_DISPELLER] = &Battle::DispellerTrigger;
    }
    Loader::loadWalls(Walls);
    Loader::loadTiles(Tiles);
    Loader::loadStrings(Strings);
    Loader::loadGestures(Gestures);
    Loader::loadEnchants(Enchants);
    Loader::loadActions(Actions, actions, Channeling_actions);
    Loader::loadWeapons(Weapons, Actions);
    Loader::loadDefaultEntities(Entities, Actions, Weapons, collisions, triggers, diefuncs);
    Loader::loadMaps(Maps, Entities, Walls, Tiles);
    for (unsigned int i = 0; i < Entities.size(); ++i) {
        InitEnt(Entities[i]);
    }
    for (unsigned int i = 0; i < Tiles.size(); ++i) {
        InitTile(Tiles[i]);
    }
    for (unsigned int i = 0; i < Walls.size(); ++i) {
        InitWall(Walls[i]);
    }
    for (unsigned int i = 0; i < Enchants.size(); ++i) {
        InitEnchant(Enchants[i]);
    }
    for (int i = 0; i < Counters::actions; ++i) {
        InitAction(Actions[i]);
    }
}
void Game::shutdown(){
    Graphic()->shutdown();
    delete graphic;
    delete input;
    delete sound;
    FreeContainer(Gestures);
    FreeContainer(Enchants);
    FreeContainer(Actions);
    FreeContainer(Weapons);
    FreeContainer(Maps);
    FreeContainer(Walls);
    FreeContainer(Tiles);
}
void Game::frame() {
    RefreshScreen();
}
LRESULT WINAPI Game::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)  {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
        DecodeInput(hWnd, uMsg, wParam, lParam);
        if (battle) {
            battle->renderer.RefreshScreen();
        } else if (mapeditor) {
            mapeditor->renderer.RefreshScreen();
        }
        RefreshScreen();
        return 0;
    case WM_PAINT:
        frame();
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    };
}

TextureHandler Game::MoveArrow_Texture(Direction dir) const {
    if (dir == NO_DIRECTION) { 
        return 0;
    } else {
        return TextureHolder.MoveArrows_Texture[static_cast<int>(dir) - 1];
    }
}
TextureHandler Game::SelectionFrame_Texture() const {
    return TextureHolder.Selection_Texture;
}
TextureHandler Game::ButtonCommon_texture(int type, ButtonState state) const {
    switch (state) {
    case BUTTONSTATE_BASE:
        return TextureHolder.buttons_common[type].base;
    case BUTTONSTATE_ACTIVE:
        return TextureHolder.buttons_common[type].active;
    case BUTTONSTATE_DISABLED:
        return TextureHolder.buttons_common[type].disabled;
    };
    return EMPTY_TEXTURE;
}
TextureHandler Game::ButtonRadio_texture(int type, ButtonState state) const {
    switch (state) {
    case BUTTONSTATE_BASE:
        return TextureHolder.buttons_radio[type].base;
    case BUTTONSTATE_ACTIVE:
        return TextureHolder.buttons_radio[type].active;
    case BUTTONSTATE_DISABLED:
        return TextureHolder.buttons_radio[type].disabled;
    };
    return EMPTY_TEXTURE;
}
TextureHandler Game::ButtonCross_texture(ButtonState state) const {
    switch (state) {
    case BUTTONSTATE_BASE:
        return TextureHolder.button_cross.base;
    case BUTTONSTATE_ACTIVE:
        return TextureHolder.button_cross.active;
    case BUTTONSTATE_DISABLED:
        return TextureHolder.button_cross.disabled;
    };
    return EMPTY_TEXTURE;
}
TextureHandler Game::ActionbarSpellbook(ButtonState state) const {
    switch (state) {
    case BUTTONSTATE_BASE:
        return TextureHolder.Actionbar_spellbook;
    case BUTTONSTATE_ACTIVE:
        return TextureHolder.Actionbar_spellbookactive;
    };
    return EMPTY_TEXTURE;
}
TextureHandler Game::ActionbarRightTrap(ButtonState state) const {
    switch (state) {
    case BUTTONSTATE_BASE:
        return TextureHolder.Actionbar_trap;
    case BUTTONSTATE_ACTIVE:
        return TextureHolder.Actionbar_trapactive;
    };
    return EMPTY_TEXTURE;
}
TextureHandler Game::ActionbarRightBomber(ButtonState state) const {
    switch (state) {
    case BUTTONSTATE_BASE:
        return TextureHolder.Actionbar_bomber;
    case BUTTONSTATE_ACTIVE:
        return TextureHolder.Actionbar_bomberactive;
    };
    return EMPTY_TEXTURE;
}
TextureHandler Game::ActionbarRightBlank() const {
    return TextureHolder.Actionbar_rightblank;
}
TextureHandler Game::ActionbarLeftBlank() const {
    return TextureHolder.Actionbar_leftblank;
}
TextureHandler Game::ActionbarLeftScroll(ScrollState state) const {
    switch (state) {
    case SCROLLSTATE_BASE:
        return TextureHolder.Actionbar_leftscroll;
    case SCROLLSTATE_UP:
        return TextureHolder.Actionbar_leftscrollup;
    case SCROLLSTATE_DOWN:
        return TextureHolder.Actionbar_leftscrolldown;
    };
    return EMPTY_TEXTURE;
}
TextureHandler Game::ActionbarBase() const {
    return TextureHolder.Actionbar_base;
}
TextureHandler Game::ActionbarTitle() const {
    return TextureHolder.Actionbar_title;
}
TextureHandler Game::Flag(Team team, bool mini) const {
    int index = team-1;
    if (mini) {
        return TextureHolder.Miniflags[index];
    } else {
        return TextureHolder.Flags[index];
    }
}

void Game::CellClick(const CoordI& coor, const long flags) {
    if (battle) {
        battle->CellClick(coor+battle->renderer.MapScreenPosition-Coord1, flags);
    } else if (mapeditor) {
        mapeditor->CellClick(coor+mapeditor->renderer.MapScreenPosition-Coord1, flags);
    }
}
void Game::KeyClick(const Keys key) {
    if (battle) { 
        battle->KeyClick(key);
    } else if (mapeditor) {
        mapeditor->KeyClick(key);
    }
}
void Game::ButtonClick(const Buttons button, int index)
{
    if (battle) { 
        battle->ButtonClick(button, index);
    } else if (mapeditor) {
        mapeditor->ButtonClick(button, index);
    }
}
void Game::DecodeInput(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    ::Input* input = this->Input();
    switch (uMsg) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        KeyClick(input->findKey(wParam));
        break;
    default: //mouse events
        CoordL coor = CoordL(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        coor = input->deglitchCoor(coor);
        auto tmp = input->findButton(coor);
        if (tmp.first != NO_BUTTON) {
            ButtonClick(tmp.first, tmp.second);
        } else {
            CoordI tmp = input->findCell(coor);
            if (tmp != Coord0) {
                int flags = 0;
                if (MK_CONTROL & wParam) { flags |= CLICK_CTRL; }
                if (MK_LBUTTON & wParam) { flags |= CLICK_LEFT; }
                if (MK_RBUTTON & wParam) { flags |= CLICK_RIGHT; }
                if (MK_SHIFT & wParam) { flags |= CLICK_SHIFT; }
                CellClick(tmp, flags);
            }
        }
        break;
    };
}

::Input*    Game::Input() { return input; }
Graphics* Game::Graphic() { return static_cast<Graphics*>(graphic); }
SoundCore* Game::Sound() { return sound; }
//====================RENDERER=====
Renderer::Renderer(Game* game, Graphics* graphics, Battle* battle, MapEditor* mapeditor): 
game(game), graphics(graphics), battle(battle), mapeditor(mapeditor), 
FieldWindowSize(field_window_size), MapScreenPosition(Coord1), grid_is_changed(true), interface_is_changed(true) {
    if (battle) {
        grid = &(battle->grid);
    } else if (mapeditor) {
        grid = &(mapeditor->grid);
    }
}

void Renderer::RenderCell(const CoordI& coor) {
    if (!grid) { return; }
    EntContainer ents = grid->GetCell(coor, GridComparer_Visible());
    ents.sort(EntityZOrderSorter());
    graphics->clearCell(coor);

    graphics->pushTexture(coor, grid->GetTileTexture(coor));
    graphics->pushTexture(coor, grid->GetWallTexture(coor));
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        RenderEntity(*it);
    }
}
void Renderer::RenderEntity(const Entity* ent, bool ClearCell) {
    CoordI coor = ent->Coor();
    if (ClearCell) {
        graphics->clearCell(coor);
    }
    if (ent->Flag(ENT_IS_INVISIBLE)) { return; }
    const Unit* unit = nullptr;
    if (ent->Flag(ENT_IS_UNIT)) {
        unit = dynamic_cast<const Unit*>(ent);
    }
    //under unit
    if (unit != nullptr) {
        if (unit->IsEnchanted(ENCHANT_ANCHOR)) {
            graphics->pushTexture(coor, game->DefEnchant(ENCHANT_ANCHOR)->MyTexture());
        }
        if (unit->IsEnchanted(ENCHANT_HASTE)) {
            graphics->pushTexture(coor, game->DefEnchant(ENCHANT_HASTE)->MyTexture());
        }
        if (unit->IsEnchanted(ENCHANT_SLOW)) {
            graphics->pushTexture(coor, game->DefEnchant(ENCHANT_SLOW)->MyTexture());
        }
        if (unit->IsEnchanted(ENCHANT_REFLECTIVE_SHIELD)) {
            if (ent->Dir() == UP_LEFT || ent->Dir() == UP_RIGHT || ent->Dir() == UP) {
                graphics->pushTexture(coor, game->DefEnchant(ENCHANT_REFLECTIVE_SHIELD)->MyTexture());
            }
        }
    }
    if (ent->Flag(ENT_IS_PROJECTILE)) {
        graphics->pushTexture(coor, ent->MyTexture(), ent->ExCoor() - (CoordD(0.5, 0.5) + ent->Coor()));
    } else {
        graphics->pushTexture(coor, ent->MyTexture());
    }
    //over unit
    if (unit != nullptr) {
        if (unit->IsEnchanted(ENCHANT_STUN)) {
            graphics->pushTexture(coor, game->DefEnchant(ENCHANT_STUN)->MyTexture());
        }
        if (unit->IsEnchanted(ENCHANT_VAMPIRISM)) {
            graphics->pushTexture(coor, game->DefEnchant(ENCHANT_VAMPIRISM)->MyTexture());
        }
        if (unit->IsEnchanted(ENCHANT_SHOCK)) {
            graphics->pushTexture(coor, game->DefEnchant(ENCHANT_SHOCK)->MyTexture());
        }
        if (unit->IsEnchanted(ENCHANT_PROTECTION_FIRE)) {
            graphics->pushTexture(coor, game->DefEnchant(ENCHANT_PROTECTION_FIRE)->MyTexture());
        }
        if (unit->IsEnchanted(ENCHANT_PROTECTION_SHOCK)) {
            graphics->pushTexture(coor, game->DefEnchant(ENCHANT_PROTECTION_SHOCK)->MyTexture());
        }
        if (unit->IsEnchanted(ENCHANT_PROTECTION_POISON)) {
            graphics->pushTexture(coor, game->DefEnchant(ENCHANT_PROTECTION_POISON)->MyTexture());
        }
        if (unit->IsEnchanted(ENCHANT_FORCE_FIELD)) {
            graphics->pushTexture(coor, game->DefEnchant(ENCHANT_FORCE_FIELD)->MyTexture());
        }
        if (unit->IsEnchanted(ENCHANT_PIXIES)) {
            graphics->pushTexture(coor, game->DefEnchant(ENCHANT_PIXIES)->MyTexture());
        }
        if (unit->IsEnchanted(ENCHANT_REFLECTIVE_SHIELD)) {
            if (ent->Dir() != UP_LEFT && ent->Dir() != UP_RIGHT && ent->Dir() != UP) {
                graphics->pushTexture(coor, game->DefEnchant(ENCHANT_REFLECTIVE_SHIELD)->MyTexture());
            }
        }
    }
    if (ent->Team() != NO_TEAM) {
        graphics->pushTexture(coor, game->Flag(ent->Team()));
    }
}
void Renderer::RefreshScreen() {
    if (grid_is_changed) {
        for (RectangleIterator it(MapScreenPosition, minCoord(grid->Size(), MapScreenPosition+FieldWindowSize-Coord1)); it.isInside(); ++it) {
            RenderCell(it.getCoor());
        }
        RenderMoveTrace();
        RenderSelection();
        grid_is_changed = false;
    }
    if (interface_is_changed) {
        RenderEntityInfo();
        if (use_mapeditor) {
            RenderObjectList();
            RenderModeBar();
        }
        interface_is_changed = false;
    }
    graphics->paint(MapScreenPosition, MapScreenPosition + FieldWindowSize - CoordI(1, 1));
}
void Renderer::MoveScreen(const Direction dir) {
    if (dir != NO_DIRECTION) {
        MoveScreenTo(MapScreenPosition + CoordI(dir));
    }
}
void Renderer::MoveScreenTo(const CoordI& coor) {
    if (grid->IsInside(coor) && grid->IsInside(coor+FieldWindowSize-Coord1)) {
        MapScreenPosition = coor;
        grid_is_changed = true;
    }
}
void Renderer::RenderSelection(const CoordI& coor) {
    graphics->pushTexture(coor, game->SelectionFrame_Texture());
}
void Renderer::RenderSelection() {
    CoordI selection;
    if (battle) {
        selection = battle->CurSel.coor;
    } else if (mapeditor) {
        selection = mapeditor->CurSel.coor;
    }
    if (selection != Coord0) {
        RenderSelection(selection);
    }
}
void Renderer::RenderEntityInfo(const Entity* ent) {
    graphics->setPictureTexture(game->Interface.EntInfo.portrait);
    graphics->setPictureTexture(game->Interface.spellbar_right, game->ActionbarRightBlank());
    graphics->setLabelText(game->Interface.EntInfo.name);
    graphics->setLabelText(game->Interface.EntInfo.hp);
    graphics->setLabelText(game->Interface.EntInfo.mp);
    graphics->setLabelText(game->Interface.EntInfo.speed);
    graphics->setLabelText(game->Interface.EntInfo.can_act);
    graphics->setLabelText(game->Interface.EntInfo.ammo);
    graphics->setLabelText(game->Interface.EntInfo.status_tip);
    graphics->setPictureTexture(game->Interface.EntInfo.status_tip_icon);
    for (int i = 0; i < max_charms; ++i) {
        graphics->setPictureTexture(game->Interface.EntInfo.enchantbar[i]);
    }
    //clearing
    RenderSpellbar(ent);
    if (!ent) { return; }
    const Unit* unit = nullptr;
    if (ent->Flag(ENT_IS_UNIT)) {
        unit = dynamic_cast<const Unit*>(ent);
    }
    graphics->setPictureTexture(game->Interface.EntInfo.portrait, ent->MyTexture());

    graphics->setLabelText(game->Interface.EntInfo.name, game->String(ent->GetPrototype().Name()));
    graphics->setLabelText(game->Interface.EntInfo.hp, int_to_string(ent->HP()) + "/" + int_to_string(ent->MaxHP()));
    graphics->setLabelText(game->Interface.EntInfo.mp, int_to_string(ent->MP()) + "/" + int_to_string(ent->MaxMP()));
    if (unit) {
        graphics->setLabelText(game->Interface.EntInfo.speed, int_to_string(unit->MovePoints()) + "/" + int_to_string(unit->Speed()));
        if (unit->getActionPoints() == 2) {
            graphics->setLabelText(game->Interface.EntInfo.can_act, game->String(STR_CAN_ACT_2));
            graphics->setLabelColor(game->Interface.EntInfo.can_act, Color(0, 200, 0));
        } else if (unit->getActionPoints() == 1) {
            graphics->setLabelText(game->Interface.EntInfo.can_act, game->String(STR_CAN_ACT_1));
            graphics->setLabelColor(game->Interface.EntInfo.can_act, Color(200, 200, 0));
        } else {
            graphics->setLabelText(game->Interface.EntInfo.can_act, game->String(STR_CAN_ACT_0));
            graphics->setLabelColor(game->Interface.EntInfo.can_act, Color(200, 0, 0));
        }
        if (unit->getWeaponManaBuffer() != 0) {
            graphics->setLabelText(game->Interface.EntInfo.ammo, int_to_string(unit->Ammo()) + " +(" + int_to_string(unit->getWeaponManaBuffer()) +")");
        } else {
            graphics->setLabelText(game->Interface.EntInfo.ammo, int_to_string(unit->Ammo()));
        }
        //right bar
        if (unit->CanUseTrap()) {
            graphics->setPictureTexture(game->Interface.spellbar_right, game->ActionbarRightTrap(BUTTONSTATE_BASE));
        } else if (unit->CanUseBomber()) {
            graphics->setPictureTexture(game->Interface.spellbar_right, game->ActionbarRightBomber(BUTTONSTATE_BASE));
        }
        //Setting status bar==============================
        int num = 0;
        //long action
        int statusbar_selection;
        if (battle) {
            statusbar_selection = battle->CharmSelected;
        } else if (mapeditor) {
            statusbar_selection = mapeditor->CharmSelected;
        }
        if (unit->isBlocking() && unit->BlockType() != NO_BLOCK) {
            graphics->setPictureTexture(game->Interface.EntInfo.enchantbar[num++], game->DefAction(ACTION_REFLECTIVE_SHIELD)->MyActiveIcon());
            if (statusbar_selection == num) {
                graphics->setPictureTexture(game->Interface.EntInfo.status_tip_icon, game->DefAction(ACTION_REFLECTIVE_SHIELD)->MyActiveIcon());
                graphics->setLabelText(game->Interface.EntInfo.status_tip, game->String(STR_IS_BLOCKING));
            }
        }
        if (unit->getLAction().action) {
            graphics->setPictureTexture(game->Interface.EntInfo.enchantbar[num++], unit->getLAction().action->MyActiveIcon());
            if (statusbar_selection == num) {
                graphics->setPictureTexture(game->Interface.EntInfo.status_tip_icon, unit->getLAction().action->MyActiveIcon());
                graphics->setLabelText(game->Interface.EntInfo.status_tip, game->String(STR_IS_PERFORMING) + game->String(unit->getLAction().action->Name()));
            }
        }
        //c_actions
        for (int i = 0; i < Counters::Channeling_actions && num < max_charms; ++i) {
            enumChannelingActions id = static_cast<enumChannelingActions>(i);
            if (unit->GetC_Action(id).Is_Cast) {
                graphics->setPictureTexture(game->Interface.EntInfo.enchantbar[num++], game->DefAction(ContToCommon(id))->MyIcon());
                if (statusbar_selection == num) {
                    graphics->setPictureTexture(game->Interface.EntInfo.status_tip_icon, game->DefAction(ContToCommon(id))->MyIcon());
                    graphics->setLabelText(game->Interface.EntInfo.status_tip, game->String(game->DefAction(ContToCommon(id))->PerformingTip()));
                }
            }
        }
        //cooldowns
        if (unit->getCooldown(CD_WARCRY)) {
            graphics->setPictureTexture(game->Interface.EntInfo.enchantbar[num++], game->DefAction(ACTION_WARCRY)->MyIcon());
            if (statusbar_selection == num) {
                graphics->setPictureTexture(game->Interface.EntInfo.status_tip_icon, game->DefAction(ACTION_WARCRY)->MyIcon());
                graphics->setLabelText(game->Interface.EntInfo.status_tip, game->String(game->DefAction(ACTION_WARCRY)->Name()) + game->String(STR_IS_ON_COOLDOWN));
            }
        }
        if (unit->getCooldown(CD_BERSERKER)) {
            graphics->setPictureTexture(game->Interface.EntInfo.enchantbar[num++], game->DefAction(ACTION_BERSERKER)->MyIcon());
            if (statusbar_selection == num) {
                graphics->setPictureTexture(game->Interface.EntInfo.status_tip_icon, game->DefAction(ACTION_BERSERKER)->MyIcon());
                graphics->setLabelText(game->Interface.EntInfo.status_tip, game->String(game->DefAction(ACTION_BERSERKER)->Name()) + game->String(STR_IS_ON_COOLDOWN));
            }
        }
        if (unit->getCooldown(CD_HARPOON)) {
            graphics->setPictureTexture(game->Interface.EntInfo.enchantbar[num++], game->DefAction(ACTION_HARPOON)->MyIcon());
            if (statusbar_selection == num) {
                graphics->setPictureTexture(game->Interface.EntInfo.status_tip_icon, game->DefAction(ACTION_HARPOON)->MyIcon());
                graphics->setLabelText(game->Interface.EntInfo.status_tip, game->String(game->DefAction(ACTION_HARPOON)->Name()) + game->String(STR_IS_ON_COOLDOWN));
            }
        }
        //enchants
        for (int i = 1; i < Counters::enchants && num < max_charms; ++i) {
            enumEnchants id = static_cast<enumEnchants>(i);
            if (ent->IsEnchanted(id)) {
                graphics->setPictureTexture(game->Interface.EntInfo.enchantbar[num++], game->DefEnchant(id)->MyIcon());
                if (statusbar_selection == num) {
                    graphics->setPictureTexture(game->Interface.EntInfo.status_tip_icon, game->DefEnchant(id)->MyIcon());
                    graphics->setLabelText(game->Interface.EntInfo.status_tip,
                        game->String(STR_IS_ENCHANTED) + game->String(game->DefEnchant(id)->name) + " (" + int_to_string(unit->EnchantTime(id)) + ")");
                }
            }
        }

    }
}
void Renderer::RenderEntityInfo() {
    if (battle) {
        RenderEntityInfo(battle->CurSel.ent);
    } else if(mapeditor) {
        RenderEntityInfo(mapeditor->CurSel.ent);
    }
}
void Renderer::RenderSpellbar(const Entity* ent) {
    if (battle) {
        graphics->setLabelText(game->Interface.spellbar_title, "Set #" + int_to_string(battle->Spellbar.CurSet+1));
        if (ent && ent->Flag(ENT_IS_UNIT)) {
            for (int i = 0; i < actions_per_set; i++) {
                const Action* act = ent->Spellbar(battle->Spellbar.CurSet * actions_per_set+i+1);
                graphics->setPictureTexture(game->Interface.spell_icons[i], act->MyIcon());
                graphics->setPictureActiveTexture(game->Interface.spell_icons[i], act->MyActiveIcon());
            }
        } else {
            for (int i = 0; i < actions_per_set; i++) {
                graphics->setPictureTexture(game->Interface.spell_icons[i], EMPTY_TEXTURE);
                graphics->setPictureActiveTexture(game->Interface.spell_icons[i], EMPTY_TEXTURE);
            }
        }
    }
}
void Renderer::RenderModeBar() {
    if (mapeditor) {
        graphics->setLabelText(game->Interface.spellbar_title, "Set #" + int_to_string(mapeditor->Spellbar.CurSet+1));
        for (int i = 0; i < actions_per_set; i++) {
            int index = i + actions_per_set*(mapeditor->Spellbar.CurSet);
            if (index < Counters::map_editor_modes) {
                graphics->setPictureTexture(game->Interface.spell_icons[i], mapeditor->modes_icons[index].first);
                graphics->setPictureActiveTexture(game->Interface.spell_icons[i], mapeditor->modes_icons[index].second);
            }
        }
    }
}
void Renderer::RenderMoveTrace() {
    if (battle) {
        for (auto it = battle->MoveTrace.trace.begin(); it != battle->MoveTrace.trace.end(); ++it) {
            RenderMoveArrow(*it);
        }
    }
}
void Renderer::RenderMoveArrow(const TraceStep& step) {
    graphics->pushTexture(step.coor, game->MoveArrow_Texture(step.dir));
}
void Renderer::RenderObjectList() {
    if (mapeditor) {
        if (mapeditor->objectlist.selected != -1) {
            game->Graphic()->movePicture(game->Interface.objectlist_selector, InterfaceConstants::miniobjectlist.getCoor(mapeditor->objectlist.selected));
            game->Graphic()->setPictureVisibility(game->Interface.objectlist_selector, true);
        } else {
            game->Graphic()->setPictureVisibility(game->Interface.objectlist_selector, false);
        }
        switch (mapeditor->objectlist.mode) {
        case OBJLIST_ENTITIES:
            for (int i = 0; i < mapeditor->objectlist.size; ++i) {
                if (mapeditor->objectlist.content[i] != 0) {
                    game->Graphic()->setPictureTexture(game->Interface.objectlist_mini[i],
                        game->DefEntity(static_cast<enumEntities>(mapeditor->objectlist.content[i]))->MyTexture());
                } else {
                    game->Graphic()->setPictureTexture(game->Interface.objectlist_mini[i], EMPTY_TEXTURE);
                }
            }
            break;
        case OBJLIST_WALLS:
            for (int i = 0; i < mapeditor->objectlist.size; ++i) {
                if (mapeditor->objectlist.content[i] != 0) {
                    game->Graphic()->setPictureTexture(game->Interface.objectlist_mini[i],
                        game->DefWall(static_cast<enumWalls>(mapeditor->objectlist.content[i]))->MyTexture());
                } else {
                    game->Graphic()->setPictureTexture(game->Interface.objectlist_mini[i], EMPTY_TEXTURE);
                }
            }
            break;
        case OBJLIST_TILES:
            for (int i = 0; i < mapeditor->objectlist.size; ++i) {
                if (mapeditor->objectlist.content[i] != 0) {
                    game->Graphic()->setPictureTexture(game->Interface.objectlist_mini[i],
                        game->DefTile(static_cast<enumTiles>(mapeditor->objectlist.content[i]))->MyTexture());
                } else {
                    game->Graphic()->setPictureTexture(game->Interface.objectlist_mini[i], EMPTY_TEXTURE);
                }
            }
            break;
        default:
            for (int i = 0; i < mapeditor->objectlist.size; ++i) {
                game->Graphic()->setPictureTexture(game->Interface.objectlist_mini[i], EMPTY_TEXTURE);
            }
        };
    }
}
void Renderer::DisplayButtonPressed(Buttons button) {
    PictureHandler button_pic = -1;
    switch (button) {
    case BUTTON_ENDTURN:
        button_pic = game->Interface.EndTurnButton.pic;
        break;
    }
    if (button_pic != -1) {
        game->Graphic()->setPictureState(button_pic, Graphics::PICTURESTATE_ACTIVE);
        game->Delay(300);
        game->Graphic()->setPictureState(button_pic, Graphics::PICTURESTATE_BASE);
    }
        
}
void Renderer::DisplaySpellsetScrolled(ScrollState state) {
    game->Graphic()->setPictureTexture(game->Interface.spellbar_left, game->ActionbarLeftScroll(state));
    game->Delay(150);
    game->Graphic()->setPictureTexture(game->Interface.spellbar_left, game->ActionbarLeftScroll(SCROLLSTATE_BASE));
}

//======================================MAP EDITOR=====

void MapEditor::CellClick(const CoordI& coor, const long flags) {
    if (!grid.IsInside(coor)) { return; }
    Unit* unit = dynamic_cast<Unit*>(CurSel.ent);
    switch (CurMode) {
    case CURSORMODE_SELECT:
        if (CLICK_LEFT & flags) {
            SetCurSel(coor);
        } else {
            
        }
        break;
    case CURSORMODE_ME_SET_WALL:
        if (objectlist.selected != -1) {
            if (flags & CLICK_LEFT) {
                grid.SetWall(coor, game->DefWall(static_cast<enumWalls>(objectlist.content[objectlist.selected])));
            } else if(flags & CLICK_RIGHT) {
                grid.RemoveWall(coor);
            }
            renderer.GridIsChanged();
        }
        break;
    case CURSORMODE_ME_SET_TILE:
        if (objectlist.selected != -1) {
            grid.SetTile(coor, game->DefTile(static_cast<enumTiles>(objectlist.content[objectlist.selected])));
        }
        renderer.GridIsChanged();
        break;
    case CURSORMODE_ME_CREATE_ENTITY:
        if (objectlist.selected != -1) {
            this->Create(game->DefEntity(static_cast<enumEntities>(objectlist.content[objectlist.selected])), coor, NO_TEAM, UP);
        }
        renderer.GridIsChanged();
    };
}
void MapEditor::KeyClick(const Keys Key) {
    switch (Key) {
    case KEY_NEXT_SPELLSET:
        ++Spellbar.CurSet;
        if (Spellbar.CurSet >= actionsets_count) {
            Spellbar.CurSet = 0;
        }
        SetActiveModeIcon();
        break;
    case KEY_PREV_SPELLSET:
        --Spellbar.CurSet;
        if (Spellbar.CurSet < 0) {
            Spellbar.CurSet = actionsets_count-1;
        }
        SetActiveModeIcon();
        break;
    case KEY_SPELL1:
        ChooseMode(1);
        break;
    case KEY_SPELL2:
        ChooseMode(2);
        break;
    case KEY_SPELL3:
        ChooseMode(3);
        break;
    case KEY_SPELL4:
        ChooseMode(4);
        break;
    case KEY_SPELL5:
        ChooseMode(5);
        break;
    case KEY_ARROW_LEFT:
        renderer.MoveScreen(LEFT);
        break;
    case KEY_ARROW_RIGHT:
        renderer.MoveScreen(RIGHT);
        break;
    case KEY_ARROW_UP:
        renderer.MoveScreen(UP);
        break;
    case KEY_ARROW_DOWN:
        renderer.MoveScreen(DOWN);
        break;
    case KEY_ESC:
        Spellbar.CurSet = 0;
        ChooseMode(1);  //HACK: this relies on the fact that 0:1 mode is Selection
        break;
    case KEY_ACCEPT:
        
        break;
    };
}
void MapEditor::ChooseMode(const int id) {
    int index = id + actions_per_set*Spellbar.CurSet - 1;
    if (index >= Counters::map_editor_modes) { return; }
    SetCurSel();
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
    CurMode = modes_panel[index];
    SetActiveModeIcon();
    CurSel.SetNull();
    objectlist.selected = -1;
    //activating objectlist
    for (int i = 0; i < objectlist_mini_size; ++i) {
        objectlist.content[i] = 0;
    }
    if (CurMode == CURSORMODE_ME_SET_TILE || CURSORMODE_ME_SET_WALL || CURSORMODE_ME_CREATE_ENTITY) {
        game->Input()->activateButton(BUTTON_OBJECTLIST);
        game->Input()->deactivateButton(BUTTON_CHARMICON);
    } else {
        objectlist.mode = OBJLIST_NOTHING;
        game->Input()->deactivateButton(BUTTON_OBJECTLIST);
        game->Input()->activateButton(BUTTON_CHARMICON);
    }

    switch (CurMode) {
    case CURSORMODE_SELECT:
        break;
    case CURSORMODE_ME_SET_TILE:
        for (int i = 0; i < min(objectlist_mini_size, Counters::tiles-1); ++i) {
            objectlist.content[i] = i+objectlist.page*objectlist.size+1;
        }
        objectlist.mode = OBJLIST_TILES;
        break;
    case CURSORMODE_ME_SET_WALL:
        for (int i = 0; i < min(objectlist_mini_size, Counters::walls-1); ++i) {
            objectlist.content[i] = i+objectlist.page*objectlist.size+1;
        }
        objectlist.mode = OBJLIST_WALLS;
        break;
    case CURSORMODE_ME_CREATE_ENTITY:
        for (int i = 0; i < min(objectlist_mini_size, Counters::entities-1); ++i) {
            objectlist.content[i] = i+objectlist.page*objectlist.size+1;
        }
        objectlist.mode = OBJLIST_ENTITIES;
        break;
    case CURSORMODE_ME_SET_ENCHANT:
        //load enchants
        break;
    default:
        //unload all
        break;
    };
}
void MapEditor::ButtonClick(const Buttons Button, int index) {
    switch (Button) {
    case BUTTON_SPELLICON:
        ChooseMode(index);
        break;
    case BUTTON_CHARMICON:
        CharmSelected = index;
        break;
    case BUTTON_OBJECTLIST:
        if (objectlist.content[index-1] != 0) {
            game->Graphic()->setPictureVisibility(game->Interface.objectlist_selector, true);
            objectlist.selected = index-1;
            renderer.InterfaceIsChanged();
        }
    };
}

void MapEditor::SetActiveModeIcon(const CursorMode curmode) {
    renderer.InterfaceIsChanged();
    for(int i = 0; i < actions_per_set; ++i) {
        if (modes_panel[i + actions_per_set*Spellbar.CurSet] == curmode) {
            game->Graphic()->setPictureState(game->Interface.spell_icons[i], Graphics::PICTURESTATE_ACTIVE);
        } else {
            game->Graphic()->setPictureState(game->Interface.spell_icons[i], Graphics::PICTURESTATE_BASE);
        }
    }
    
}

void MapEditor::SetActiveModeIcon() {
    SetActiveModeIcon(CurMode);
}

void MapEditor::ClearSpellbar() {
    SetActiveModeIcon();
    Spellbar.CurSet = 0;
}
Entity* MapEditor::Create(const DefaultEntity* prototype, const CoordI& coor, Team team, Direction dir) {
    Entity* tmp = prototype->Create(coor, team, dir);
    grid.Add(tmp);
    ents.push_back(tmp);
    return tmp;
}

void MapEditor::SetCurSel() {
    //ClearSpellbar();
    CharmSelected = 0;
    renderer.GridIsChanged();
}
void MapEditor::SetCurSel(Entity *entity) {
    if (!entity) {
        CurSel.SetNull();
    } else {
        CurSel.Set(entity, entity->Coor());
    }
    SetCurSel();
}
void MapEditor::SetCurSel(const CoordI& coord) {
    if (coord == Coord0) {
        CurSel.SetNull();
    } else {
        auto ents = grid.GetCell(coord, GridComparer_Visible());
        if (ents.empty()) {
            CurSel.Set(nullptr, coord);
        } else {
            if (CurSel.coor != coord) {
                bool isset = false;
                for (auto it = ents.begin(); it != ents.end(); ++it) {
                    if (!(*it)->Flag(ENT_IS_UNTARGETABLE)) {
                        CurSel.Set(*it, coord);
                        isset = true;
                        break;
                    }
                }
                if (!isset) {
                    CurSel.Set(nullptr, coord);
                }
            } else {
                auto next = ents.begin();
                for (auto it = ents.begin(); it != ents.end(); ++it) {
                    if (*it == CurSel.ent) {
                        next = ++it;
                        break;
                    }
                }
                if (next == ents.end()) {
                    next = ents.begin();
                }
                CurSel.Set(*next, coord);
            }
        }

    }
    SetCurSel();
}
MapEditor::MapEditor(const CoordI& size, Game* game): game(game), grid(size), CharmSelected(0),
renderer(game, game->Graphic(), nullptr, this), modes_icons(Counters::map_editor_modes),
objectlist(objectlist_mini_size), CurMode(CURSORMODE_SELECT) {
    modes_panel[0] = CURSORMODE_SELECT;
    modes_icons[0] = ModeIconTexture(game->Graphic()->addTexture(Paths::mapeditor_icons +  "select" + common_image_extension), 
        game->Graphic()->addTexture(Paths::mapeditor_icons_active +  "select" + common_image_extension));
    modes_panel[1] = CURSORMODE_ME_SET_WALL;
    modes_icons[1] = ModeIconTexture(game->Graphic()->addTexture(Paths::mapeditor_icons +  "walls" + common_image_extension),
        game->Graphic()->addTexture(Paths::mapeditor_icons_active +  "walls" + common_image_extension));
    modes_panel[2] = CURSORMODE_ME_SET_TILE;
    modes_icons[2] = ModeIconTexture(game->Graphic()->addTexture(Paths::mapeditor_icons +  "tiles" + common_image_extension),
        game->Graphic()->addTexture(Paths::mapeditor_icons_active +  "tiles" + common_image_extension));
    modes_panel[3] = CURSORMODE_ME_CREATE_ENTITY;
    modes_icons[3] = ModeIconTexture(game->Graphic()->addTexture(Paths::mapeditor_icons +  "entities" + common_image_extension),
        game->Graphic()->addTexture(Paths::mapeditor_icons_active +  "entities" + common_image_extension));
    modes_panel[4] = CURSORMODE_ME_DELETE_ENTITY;
    modes_icons[4] = ModeIconTexture(game->Graphic()->addTexture(Paths::mapeditor_icons +  "delete entity" + common_image_extension),
        game->Graphic()->addTexture(Paths::mapeditor_icons_active +  "delete entity" + common_image_extension));
    modes_panel[5] = CURSORMODE_ME_DELETE_ALL_ENTITIES;
    modes_icons[5] = ModeIconTexture(game->Graphic()->addTexture(Paths::mapeditor_icons +  "delete entities" + common_image_extension),
        game->Graphic()->addTexture(Paths::mapeditor_icons_active + "delete entities" + common_image_extension));
    modes_panel[6] = CURSORMODE_ME_SET_OWNER;
    modes_icons[6] = ModeIconTexture(game->Graphic()->addTexture(Paths::mapeditor_icons +  "set owner" + common_image_extension),
        game->Graphic()->addTexture(Paths::mapeditor_icons_active + "set owner" + common_image_extension));
    modes_panel[7] = CURSORMODE_ME_SET_ENCHANT;
    modes_icons[7] = ModeIconTexture(game->Graphic()->addTexture(Paths::mapeditor_icons +  "set enchant" + common_image_extension),
        game->Graphic()->addTexture(Paths::mapeditor_icons_active +  "set enchant" + common_image_extension));

    SetActiveModeIcon();
    for (RectangleIterator it(Coord1, size); it.isInside(); ++it) {
        grid.SetTile(it.getCoor(), game->DefTile(TILE_GRASS));
    }
    for (int i = 1; i <= size.x; ++i) {
        grid.SetWall(CoordI(i, 1), game->DefWall(WALL_BRICK));
        grid.SetWall(CoordI(i, size.y), game->DefWall(WALL_BRICK));
    }
    for (int i = 1; i <= size.y; ++i) {
        grid.SetWall(CoordI(1, i), game->DefWall(WALL_BRICK));
        grid.SetWall(CoordI(size.x, i), game->DefWall(WALL_BRICK));
    }
}
//======================================BATTLE
Battle::Battle(const Map* map, Game* game): game(game), map(map), ents(), curplayer(TEAM_BLUE), 
    grid(map->size), players_count(map->players_count), CurMode(CURSORMODE_SELECT), CurSel(), 
    Spellbar(), MoveTrace(), LongActions(), CharmSelected(0), logger(game), replayer(),
    renderer(game, game->Graphic(), this, nullptr) {
        for (auto it = map->entities.begin(); it != map->entities.end(); ++it) {
            ents.push_back((*it)->Clone());
        }
        for (auto it = ents.begin(); it != ents.end(); ++it) {
            grid.Add(*it);
        }
        for (RectangleIterator it(Coord1, map->size); it.isInside(); ++it) {
            CoordI tmp = it.getCoor();
            Wall* wall = map->walls[tmp.x-1][tmp.y-1];
            if (wall) {
                grid.SetWall(tmp, wall);
            }
        }
        for (RectangleIterator it(Coord1, map->size); it.isInside(); ++it) {
            CoordI tmp = it.getCoor();
            grid.SetTile(tmp, map->tiles[tmp.x-1][tmp.y-1]);
        }
}
//-----user input
void Battle::CellClick(const CoordI& coor, const long flags) {
    if (!grid.IsInside(coor)) { return; }
    replayer.addInput(Replayer::UserInput(coor, flags));
    Unit* unit = dynamic_cast<Unit*>(CurSel.ent);
    switch(CurMode) {
    case CURSORMODE_SELECT:
        if (CLICK_LEFT & flags) {
            SetCurSel(coor);
        } else {
            if (CurSel.ent && CurSel.ent->Team() == curplayer) {
                if (CLICK_SHIFT & flags) {
                    ClearMoveTrace();
                    if (unit->MovePoints() > 0) {
                        Direction dir = CurSel.coor.getDirection(coor);
                        if (dir != NO_DIRECTION) {
                            Turn(CurSel.ent, dir);
                            unit->movepoints -= 1;
                            renderer.InterfaceIsChanged();
                        }
                    }
                    renderer.GridIsChanged();
                } else {
                    AddStep(coor);
                    renderer.GridIsChanged();
                }
            }
        }
        break;
    case CURSORMODE_TARGETING:
        if (CurSel.ent && CurSel.ent->Team() == curplayer) {
            //renderer.GridChanged();
            //renderer.InterfaceChanged();
            const Action* action;
            int spellid = Spellbar.ID();
            if (spellid <= 0 || spellid > actionsets_count*actions_per_set) { return; }
            action = unit->Spellbar(spellid);
            if (action == game->DefAction(NO_ACTION)) { break; }
            if (action->IsLengthy()) {
                unit->long_action.action = action;
                unit->long_action.coor = coor;
                unit->long_action.time = action->TimeToPerform();
                unit->action_points = 0;
                renderer.GridIsChanged();
                renderer.InterfaceIsChanged();
            } else {
                if (action->CheckForValidity(this, CurSel.ent, coor)) {
                    game->Sound()->PlaySound(action->MySound());
                    action->ApplyPenalties(this, CurSel.ent, coor);
                    renderer.GridIsChanged();
                    renderer.InterfaceIsChanged();
                    if (action->Flag(AFLAG_IS_AGGRESSIVE)) {
                        for (int i = 1; i < Counters::enchants; ++i) {
                            enumEnchants id = static_cast<enumEnchants>(i);
                            if (game->DefEnchant(id)->breaks_upon_aggressiveness) {
                                Disenchant(unit, id);
                            };
                        }
                    }
                    renderer.GridIsChanged();
                    renderer.InterfaceIsChanged();
                    action->Perform(this, CurSel.ent, coor);
                }
            }
            ClearSpellbar();
        }
        if (!CurSel.ent->is_dead) {
            SetCurSel(CurSel.ent);
        } else {
            SetCurSel(CurSel.coor);
        }
        CurMode = CURSORMODE_SELECT;
        break;
    case CURSORMODE_LTARGETING:
        if (unit->long_action.action->CheckForValidity(this, CurSel.ent, coor)) {
            game->Sound()->PlaySound(unit->long_action.action->MySound());
            unit->long_action.action->ApplyPenalties(this, CurSel.ent, coor);
            if (unit->long_action.action->Flag(AFLAG_IS_AGGRESSIVE)) {
                for (int i = 1; i < Counters::enchants; ++i) {
                    enumEnchants id = static_cast<enumEnchants>(i);
                    if (game->DefEnchant(id)->breaks_upon_aggressiveness) {
                        Disenchant(unit, id);
                    };
                }
            }
            unit->long_action.action->Perform(this, CurSel.ent, coor);
            unit->long_action.Nullify();
            NextLAction();
        }
        break;
    };
}
void Battle::KeyClick(const Keys Key) {
    replayer.addInput(Key);
    switch(Key) {
    case KEY_NEXT_SPELLSET:
        ChangeSpellset(true);
        break;
    case KEY_PREV_SPELLSET:
        ChangeSpellset(false);
        break;
    case KEY_SPELL1:
        ChooseSpell(1);
        break;
    case KEY_SPELL2:
        ChooseSpell(2);
        break;
    case KEY_SPELL3:
        ChooseSpell(3);
        break;
    case KEY_SPELL4:
        ChooseSpell(4);
        break;
    case KEY_SPELL5:
        ChooseSpell(5);
        break;
    case KEY_ARROW_LEFT:
        renderer.MoveScreen(LEFT);
        break;
    case KEY_ARROW_RIGHT:
        renderer.MoveScreen(RIGHT);
        break;
    case KEY_ARROW_UP:
        renderer.MoveScreen(UP);
        break;
    case KEY_ARROW_DOWN:
        renderer.MoveScreen(DOWN);
        break;
    case KEY_ESC:
        switch (CurMode){
        case CURSORMODE_TARGETING:
            CurMode = CURSORMODE_SELECT;
            SetActiveSpellIcon(0);
            break;
        case CURSORMODE_SELECT:
            ClearMoveTrace();
            break;
        case CURSORMODE_LTARGETING:
            dynamic_cast<Unit*>(CurSel.ent)->long_action.Nullify();
            NextLAction();
            break;
        };
        break;
    case KEY_ACCEPT:
        if (CurSel.ent) {
            if (CurSel.ent->Flag(ENT_IS_UNIT)) {
                ApplyUnitMove();
            }
        }
        break;
    };
}
void Battle::ChooseSpell(const int id) {
    if (CurSel.ent && CurSel.ent->Team() == curplayer) {
        if (CurMode == CURSORMODE_TARGETING || CurMode == CURSORMODE_SELECT) {
            SetActiveSpellIcon(id);
            if (id==0) {
                CurMode = CURSORMODE_SELECT;
            } else {
                CurMode = CURSORMODE_TARGETING;
            }
        }
    }
}
void Battle::ButtonClick(const Buttons Button, int index)
{
    replayer.addInput(Replayer::UserInput(Button, index));
    switch (Button) {
    case BUTTON_ENDTURN:
        if (CurMode != CURSORMODE_LTARGETING) {
            logger.addEntry(STR_ENDTURN, curplayer);
            renderer.DisplayButtonPressed(BUTTON_ENDTURN);
            EndTurn();
        }
        break;
    case BUTTON_SPELLICON:
        ChooseSpell(index);
        break;
    case BUTTON_SPELLSCROLL_UP:
        ChangeSpellset(false);
        break;
    case BUTTON_SPELLSCROLL_DOWN:
        ChangeSpellset(true);
        break;
    case BUTTON_CHARMICON:
        CharmSelected = index;
        renderer.InterfaceIsChanged();
        break;
    };
}
void Battle::ChangeSpellset(bool next_set) {
    if (next_set) {
        Spellbar.NextSet();
    } else {
        Spellbar.PrevSet();
    }
    ChooseSpell(0);
    renderer.InterfaceIsChanged();
    renderer.RefreshScreen();
    if (next_set) {
        renderer.DisplaySpellsetScrolled(SCROLLSTATE_DOWN);
    } else {
        renderer.DisplaySpellsetScrolled(SCROLLSTATE_UP);
    }
}

void Battle::SetActiveSpellIcon(const int id) {
    if (id == Spellbar.CurSpell) { return; }
    if (Spellbar.CurSpell != 0) {
        game->Graphic()->setPictureState(game->Interface.spell_icons[Spellbar.CurSpell-1], Graphics::PICTURESTATE_BASE);
    }
    if (id != 0) {
        game->Graphic()->setPictureState(game->Interface.spell_icons[id-1], Graphics::PICTURESTATE_ACTIVE);
    }
    Spellbar.CurSpell = id;
    renderer.InterfaceIsChanged();
}
void Battle::ClearSpellbar() {
    renderer.InterfaceIsChanged();
    SetActiveSpellIcon(0);
    Spellbar.CurSet = 0;
}

void Battle::SetCurSel() {
    ClearSpellbar();
    ClearMoveTrace();
    CharmSelected = 0;
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
}
void Battle::SetCurSel(Entity *entity) {
    if (!entity) {
        CurSel.SetNull();
    } else {
        CurSel.Set(entity, entity->Coor());
    }
    SetCurSel();
}
void Battle::SetCurSel(const CoordI& coord) {
    if (coord == Coord0) {
        CurSel.SetNull();
    } else {
        auto ents = grid.GetCell(coord, GridComparer_Visible());
        if (ents.empty()) {
            CurSel.Set(nullptr, coord);
        } else {
            if (CurSel.coor != coord) {
                bool isset = false;
                for (auto it = ents.begin(); it != ents.end(); ++it) {
                    if (!(*it)->Flag(ENT_IS_UNTARGETABLE)) {
                        CurSel.Set(*it, coord);
                        isset = true;
                        break;
                    }
                }
                if (!isset) {
                    CurSel.Set(nullptr, coord);
                }
            } else {
                auto next = ents.begin();
                for (auto it = ents.begin(); it != ents.end(); ++it) {
                    if (*it == CurSel.ent) {
                        next = ++it;
                        break;
                    }
                }
                if (next == ents.end()) {
                    next = ents.begin();
                }
                CurSel.Set(*next, coord);
            }
        }
        
    }
    SetCurSel();
}
//------entity interactions
Entity* Battle::Create(const DefaultEntity* prototype, const CoordI& coor, Team team, Direction dir)
{
    renderer.GridIsChanged();
    Entity* tmp = prototype->Create(coor, team, dir);
    grid.Add(tmp);
    ents.push_back(tmp);
    return tmp;
}
void Battle::SetOwner(Entity* owner, Entity* sub) {
    if (sub->object_owner == owner) { return; }
    if (sub->object_owner) {
        RemoveOwner(sub->object_owner, sub);
    }
    sub->object_owner = owner;
    owner->subs.push_back(sub);
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
}
void Battle::RemoveOwner(Entity* owner, Entity* sub) {
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
    for (auto it = owner->subs.begin(); it != owner->subs.end(); ++it) {
        if (*it == sub) {
            owner->subs.erase(it);
            return;
        }
    }
    throw("sub not found");
}
//return true, if further automatic move is impossible
bool Battle::Move(Entity* ent, const CoordI& dest, const Direction dir, bool Env, bool EnvSource)
{
    logger.addEntry(STR_MOVED_TO, ent->Name(), ent->Coor(), dest);
    grid.Move(ent, dest, dir);
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
    bool ret = true;
    if (Env) {
        ret = ApplyEnvironmentEffects(ent, TRIGGERACTIVATE_MOVED_IN);
    }
    if (EnvSource) {
        ApplyEnvironmentEffect_Source(ent);
    }
    renderer.RefreshScreen();
    return ret;
}
//returns true if destination direction is free to move
bool Battle::PushTo(Entity* ent, const Direction dir) {
    CoordI dest = ent->Coor() + CoordI(dir);
    if (grid.BlocksMoving(ent, dest)) {
        return false;
    } else {
        Move(ent, dest);
        return true;
    }
}
void Battle::PushTo(Entity* ent, const CoordI& coor) {
    auto dirs = ent->Coor().getDirection2(coor);
    if (!PushTo(ent, dirs.first) && dirs.first != dirs.second) {
        PushTo(ent, dirs.second);
    }
}
void Battle::PushTo(Entity *ent, const Angle angle) {
    auto dirs = angle.getDirection2();
    if (!PushTo(ent, dirs.first) && dirs.first != dirs.second) {
        PushTo(ent, dirs.second);
    }
}
void Battle::PushFrom(Entity* ent, const CoordI& coor) {
    auto dirs = coor.getDirection2(ent->Coor());
    if (!PushTo(ent, dirs.first) && dirs.first != dirs.second) {
        PushTo(ent, dirs.second);
    }
}
void Battle::Swap(Entity* ent1, Entity* ent2) {
    grid.Swap(ent1, ent2);
    ApplyEnvironmentEffects(ent1, TRIGGERACTIVATE_MOVED_IN);
    ApplyEnvironmentEffect_Source(ent1);
    ApplyEnvironmentEffects(ent2, TRIGGERACTIVATE_MOVED_IN);
    ApplyEnvironmentEffect_Source(ent2);
}
void Battle::Turn(Entity* ent, const Direction dir) {
    ent->Turn(dir);
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
}

void Battle::EnchantWith(Entity* ent, const Enchant& enchant, int time) {
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
    if (enchant.does_wearoff_auto) {
        logger.addEntry(STR_WAS_ENCHANTED_FOR, ent->Name(), enchant.name, time);
    } else {
        logger.addEntry(STR_WAS_ENCHANTED, ent->Name(), enchant.name);
    }
    ent->EnchantWith(enchant, time);
    game->Sound()->PlaySound(enchant.sound_on);
}
void Battle::EnchantWith(Entity* ent, enumEnchants enchantid, int time) {
    EnchantWith(ent, *game->DefEnchant(enchantid), time);
}
void Battle::Disenchant(Entity* ent, enumEnchants enchantid) {
    logger.addEntry(STR_DISENCHANTED, ent->Name(), game->DefEnchant(enchantid)->name);
    ent->Disenchant(enchantid);
    game->Sound()->PlaySound(game->DefEnchant(enchantid)->sound_off);
}
bool Battle::IsEnchanted(const Entity* ent, enumEnchants enchantid) const
{
    return ent->IsEnchanted(enchantid);
}

void Battle::RestoreHP(Entity* ent, int value) {
    ent->RestoreHP(value);
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
}
void Battle::RestoreMP(Entity* ent, int value) {
    ent->RestoreMP(value);
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
}
void Battle::BurnMP(Entity* ent, int value) {
    ent->BurnMP(value);
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
}
int Battle::Damage(Entity* source, Entity* target, const Dmg damage) {
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
    int tmp = target->HP();
    target->GetDamage(damage);
    StopCActions(target);
    tmp -= target->HP();
    if (target != source && source != nullptr) {
        logger.addEntry(STR_WAS_DAMAGED_BY, target->Name(), source->Name(), tmp);
    } else {
        logger.addEntry(STR_WAS_DAMAGED, target->Name(), tmp);
    }
    if (target->HP() <= 0) {
        Kill(target, source);
    } else if (!(damage.flags & DFLAG_IGNORE_SOUND)) {
        game->Sound()->PlaySound(target->SoundDamaged());
    }
    if (IsEnchanted(source, ENCHANT_VAMPIRISM) && target->Flag(ENT_IS_UNIT) && !target->Flag(ENT_IS_UNDEAD)) {
        RestoreHP(source, tmp); 
        game->Sound()->PlaySound(game->DefEnchant(ENCHANT_VAMPIRISM)->metasound);
    }
    return tmp;
}
void Battle::Kill(Entity* ent, Entity* killer) {
    logger.addEntry(ent->LogDie(), ent->Name());
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
    game->Delay(50);
    game->Sound()->PlaySound(ent->SoundDied());
    ent->Kill(this, killer);
    for (auto it = ent->subs.begin(); it != ent->subs.end(); ++it) {
        if ((*it)->Flag(ENT_IS_CONJURED)) {
            Kill(*it, *it);                                                                                                              
        }
    }
}
void Battle::StopCActions(Entity* ent) {
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
    if (ent->Flag(ENT_IS_UNIT)) {
        Unit* unit = dynamic_cast<Unit*>(ent);
        for (int i = 0; i < Counters::Channeling_actions; ++i) {
            if (game->DefAction(ContToCommon(static_cast<enumChannelingActions>(i)))->Flag(AFLAG_IS_BROKEN_BY_DAMAGE)) { //HACK: casting number into enum
                unit->c_action[i].UnSet();
            }
        }
    }
}
//-----projectiles
void Battle::LaunchProjectile(enumEntities projectile_id, Entity* source, const CoordI& dest) {
    LaunchProjectile(projectile_id, source, Angle(source->Coor(), dest));
}
void Battle::LaunchProjectile(enumEntities projectile_id, Entity* source, const Angle& angle) {
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
    const DefaultProjectile* defproj = static_cast<const DefaultProjectile*>(game->DefEntity(projectile_id));
    Entity* proj = defproj->Create(source->Coor(), source->Team(), source, angle);
    SetOwner(source, proj);
    ents.push_back(proj);
    grid.Add(proj);
    MoveProjectile(proj, true);
}
void Battle::LaunchHomingProjectile(enumEntities projectile_id, Entity* source, Entity* target, enumEnchants id) {
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
    const DefaultProjectile* defproj = static_cast<const DefaultProjectile*>(game->DefEntity(projectile_id));
    Entity* proj;
    if (id != NO_ENCHANT) {
        proj = defproj->Create(source->Coor(), source->Team(), target, source, id);
    } else {
        proj = defproj->Create(source->Coor(), source->Team(), target, source);
    }
    SetOwner(source, proj);
    ents.push_back(proj);
    grid.Add(proj);
    MoveProjectile(proj, true);
}
void Battle::MoveProjectile(Entity* projectile, bool IsFirstMove) {
    if (projectile->Flag(ENT_IS_PROJECTILE) && !projectile->Flag(ENT_IS_HOMING_PROJECTILE)) {
        CommonProjectile* pr = dynamic_cast<CommonProjectile*>(projectile);
        bool firstcell_noaoe = IsFirstMove && pr->BehaviourFlag(BEHAVIOUR_NO_AOE_AT_FIRST_CELL);
        game->Sound()->PlaySound(projectile->SoundMove());
        for (int i = 0; i < pr->Speed(); ++i) {
            CoordD oldexcoor;
            CoordI oldcoor;
            CoordD newexcoor = pr->ex_coor;
            CoordI newcoor = pr->Coor();
            for (int step = 0; step < projectile_steps_per_speed; step++) {
                oldexcoor = newexcoor;
                oldcoor = newcoor;

                newexcoor = oldexcoor + ProjectileStep(pr->angle) / (const double)projectile_steps_per_speed;
                newcoor = GetRoundedCoor(newexcoor);

                pr->ex_coor = newexcoor;
                //TODO: set partial re-rendering of that particular projectile on each new excoor; delay by 10 ms, remove delay(100) after move()
                if (newcoor != oldcoor) {
                    if (ProjectileReflection(projectile, oldexcoor, newexcoor, pr->angle)) {
                        --step;
                        newexcoor = oldexcoor;
                        newcoor = oldcoor;
                        continue;
                    }
                    if (grid.BlocksMoving(projectile, newcoor)) {
                        if (!pr->Collision(this, newcoor, projectile->Dir())) {
                            return;
                        }
                    }
                }
            }
            renderer.GridIsChanged();
            renderer.InterfaceIsChanged();
            Move(projectile, newcoor, NO_DIRECTION, true, !firstcell_noaoe);
            firstcell_noaoe = false;
            game->Delay(100);
        }
    } else if (projectile->Flag(ENT_IS_HOMING_PROJECTILE)) {
        HomingProjectile* pr = dynamic_cast<HomingProjectile*>(projectile);
        Entity* target = pr->target;
        if (!target || target->IsDead()) { return; }
        Angle ang = Angle(pr->ex_coor, target->ExCoor());
        for (int i = 0; i < pr->Speed(); ++i) {
            CoordD oldexcoor;
            CoordI oldcoor;
            CoordD newexcoor = pr->ex_coor;
            CoordI newcoor = pr->Coor();
            for (int step = 0; step < projectile_steps_per_speed; step++) {
                oldexcoor = newexcoor;
                oldcoor = newcoor;

                newexcoor = oldexcoor + ProjectileStep(ang) / (const double)projectile_steps_per_speed;
                newcoor = GetRoundedCoor(newexcoor);

                if (newcoor != oldcoor) {
                    Entity* reflector = nullptr;
                    if (ProjectileReflection(projectile, oldexcoor, newexcoor, ang)) {
                        --step;
                        continue;
                    }
                    if (grid.BlocksMoving(projectile, newcoor)) {
                        if (!pr->Collision(this, newcoor, projectile->Dir())) {
                            return;
                        }
                    }
                }
            }
            renderer.GridIsChanged();
            renderer.InterfaceIsChanged();
            pr->ex_coor = newexcoor;
            Move(projectile, newcoor);
            game->Delay(100);
        }
    }
}
//return true if unit holds some kind of shield that reflects this projectile
bool Battle::ProjectileReflectionShieldTest(const Projectile* projectile, const Unit* unit, const CoordD& ProjectilePoint) const
{
    return (unit->IsEnchanted(ENCHANT_REFLECTIVE_SHIELD) && projectile->ReflectionFlag(REFLECTEDBY_MAGICSHIELD) ||
        unit->is_in_blocking_state && (projectile->ReflectionFlag(REFLECTEDBY_LONGSWORD) && unit->BlockType() == BLOCK_LONGSWORD ||
        projectile->ReflectionFlag(REFLECTEDBY_SHIELD) && (unit->BlockType() == BLOCK_QUICKSHIELD || unit->BlockType() == BLOCK_SHIELD))) &&
        Angle(unit->ExCoor(), ProjectilePoint).getDegreesBetween(Angle(unit->Dir())) < ShieldAngleHalf;
}
bool Battle::ProjectileReflection(Entity* projectile, const CoordD& oldexcoor, const CoordD& excoor, Angle& angle) {
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
    Projectile* proj = dynamic_cast<Projectile*>(projectile);
    CoordI coor = GetRoundedCoor(excoor);

    bool is_reflected = false;
    if (!projectile->Flag(ENT_IS_HOMING_PROJECTILE)) {
        if (proj->ReflectionFlag(REFLECTEDBY_WALL) && grid.IsWindowlessWall(coor)) {
            is_reflected = true;
        }
    }
    auto ents = grid.GetCell(coor, GridComparer_Units());
    if (!ents.empty()) {
        Unit* unit = dynamic_cast<Unit*>(*ents.begin());
        if (ProjectileReflectionShieldTest(proj, unit, excoor)) {
            //HACK: compares angle with the newer ex coor, assuming it didnt overstepped cell center; delivers more precision
            if (projectile->Flag(ENT_IS_HOMING_PROJECTILE)) {
                dynamic_cast<HomingProjectile*>(proj)->target = projectile->object_owner;
            }
            SetOwner(unit, projectile);
            is_reflected = true;
            game->Sound()->PlaySound(game->DefEnchant(ENCHANT_REFLECTIVE_SHIELD)->metasound);
        }
    }

    if (is_reflected) {
        logger.addEntry(STR_REFLECTED, proj->Name(), coor);
        if (!projectile->Flag(ENT_IS_HOMING_PROJECTILE)) {
            /*if (angle.getDegrees() % 45 == 0 && angle.getDegrees() % 90 != 0 && //TODO: and check if adjancent cells dont reflect
                ) {
                angle.reflect(REFLECT_POINT);
            } else {*/
                CoordI oldcoor = GetRoundedCoor(oldexcoor);
                if (oldcoor.x == coor.x) {
                    angle.reflect(REFLECT_VERTICAL);
                } else if (oldcoor.y == coor.y) {
                    angle.reflect(REFLECT_HORIZONTAL);
                } else {
                    CoordD nearestverge = grid.NearestCellVerge(oldexcoor, coor);
                    Line proj_course(oldexcoor, excoor);
                    Line vertical_edge(nearestverge, Angle(UP));
                    CoordD intersect = proj_course.getIntersectionPoint(vertical_edge);
                    if (abs(intersect.x - nearestverge.x) < EPS) {
                        angle.reflect(REFLECT_HORIZONTAL);
                    } else {
                        angle.reflect(REFLECT_VERTICAL);
                    }
                }
            //}
        } else {
            angle = Angle(oldexcoor, dynamic_cast<HomingProjectile*>(proj)->target->ExCoor());
        }
    }
    return is_reflected;
}
bool Battle::UnitBlockTest(Entity* entity_to_test, const Angle& BlockingAngle, int BlockTypes, bool TestMagicShield) const
{
    if (entity_to_test->Flag(ENT_IS_UNIT)) {
        Unit* unit = dynamic_cast<Unit*>(entity_to_test);
        if (Angle(unit->Dir()).getDegreesBetween(BlockingAngle) <= ShieldAngleHalf) {
            return TestMagicShield && unit->IsEnchanted(ENCHANT_REFLECTIVE_SHIELD) ||
                unit->is_in_blocking_state && (BlockTypes & static_cast<int>(unit->BlockType()));
        }
    }
    return false;
}

//-----unit move
void Battle::AddStep(const CoordI& coor){
    if (!CurSel.ent) { return; }
    for (auto it = MoveTrace.trace.begin(); it != MoveTrace.trace.end(); ++it) {
        if (it->coor == coor) {
            MoveTrace.trace.erase(it, MoveTrace.trace.end());
            break;
        }
    }

    CoordI LastPos = (MoveTrace.trace.empty()) ? CurSel.ent->Coor() : (MoveTrace.trace.end()-1)->coor;
    Direction LastDir = (MoveTrace.trace.empty()) ? CurSel.ent->Dir() : (MoveTrace.trace.end()-1)->dir;
    CoordI dif = (coor - LastPos);
    int dist = grid.Dist(coor, LastPos, true);
    if (dist == 1) {
        if (!grid.BlocksMoving(CurSel.ent, coor)) {
            TraceStep curstep(coor, false, false, LastPos.getDirection(coor), LastDir);
            MoveTrace.trace.push_back(curstep);
            //RenderMoveArrow(curstep);
        }
    } else if (dist == 2 && (dif.y == 0 || dif.x == 0 || abs(dif.x) == abs(dif.y))) { //only straight lines accepted for jump
        CoordI middle_cell = LastPos + dif/2;
        if (!grid.BlocksMoving(CurSel.ent, coor) && !grid.BlocksMoving(CurSel.ent, middle_cell, true) ) {
            TraceStep curstep(coor, false, true, LastPos.getDirection(coor), LastDir);
            TraceStep middle_step(middle_cell, true, false, curstep.dir, LastDir);
            MoveTrace.trace.push_back(middle_step);
            MoveTrace.trace.push_back(curstep);
            /*RenderMoveArrow(middle_step);
            RenderMoveArrow(curstep);*/
        }
    } else {
        Path path = grid.FindPath(LastPos, coor, CurSel.ent);
        if (path.empty())
            return;
        for (auto it = path.rbegin()+1; it != path.rend(); ++it) {
            AddStep(*it);
        }
    }
}
void Battle::ClearMoveTrace() {
    MoveTrace.trace.clear(); 
}
int Battle::EvaluateStepCost(const TraceStep& step) const {
    if (step.IsJumpEnd) { return 0; }
    if (step.IsJump) {
        if (step.dir != step.prevdir) {
            return 3;
        } else {
            return 2;
        }
    } else {
        return 1;
    }
}
int Battle::EvaluateMoveCost() const {
    if (!CurSel.ent) { return 0; }
    int cost = 0;
    for (auto it = MoveTrace.trace.begin(); it != MoveTrace.trace.end(); ++it) {
        cost += EvaluateStepCost(*it);
    }
    return cost;
}
void Battle::ApplyUnitMove() {
    if (MoveTrace.trace.empty()) { return; }
    if (!CurSel.ent ||                          //nobody selected
        !CurSel.ent->Flag(ENT_IS_UNIT) ||       //not a unit
        CurSel.ent->Team() != curplayer) {      //not your unit
            return; 
    }
    int cost = EvaluateMoveCost();
    Unit* unit = dynamic_cast<Unit*>(CurSel.ent);
    if (unit->MovePoints() < cost) { return; } //not enough movepoints
    
    unit->is_in_blocking_state = false;
    unit->StopCSpells();
    for (int i = 1; i < Counters::enchants; ++i) {
        enumEnchants id = static_cast<enumEnchants>(i);
        if (game->DefEnchant(id)->breaks_upon_moving){
            Disenchant(unit, id);
        };
    }

    bool force_break = false;
    for (auto it = MoveTrace.trace.begin(); it != MoveTrace.trace.end(); ++it) {
        game->Delay(50);
        if (unit->is_dead) {
            break;      //unit can only die due to apply environment effects; this can be reached if it died in jump.
        }
        Move(CurSel.ent, it->coor, it->dir, false);
        unit->is_jumping = it->IsJump;
        unit->movepoints -= EvaluateStepCost(*it);
        if (force_break) {
            ApplyEnvironmentEffects(CurSel.ent, TRIGGERACTIVATE_MOVED_IN);
            break;
        }
        if (ApplyEnvironmentEffects(CurSel.ent, TRIGGERACTIVATE_MOVED_IN)) {
            if (it->IsJump) {
                force_break = true; //if affecting jumping unit, it doesn't break moving immediately, but set a forces break at next cell
            } else {
                break;
            }
        }
    }
    if (unit->is_dead) {
        SetCurSel(CurSel.ent->coor);
    } else {
        SetCurSel(CurSel.ent);
    }
}
//------end turn
bool Battle::ApplyEnvironmentEffects(Entity* ent, enumTriggerActivationType trigger_activation_type) {
    bool isaffected = false;
    EntContainer ents = grid.GetCell(ent->Coor(), GridComparer_EnvironmentAffecting());
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        if (!(*it)->is_dead && *it != ent) {
            if (ent->Flag(ENT_IS_UNIT) && (*it)->Flag(ENT_IS_OBELISK)) {
                ent->DrainMana(*it);
            }
            if ((*it)->Flag(ENT_IS_PROJECTILE)) {
                Projectile* pr = dynamic_cast<Projectile*>(*it);
                if (pr->BlocksMoving(ent)) {
                    pr->Collision(this, ent->Coor(), ent->LastMoveDirection());
                    isaffected = true;
                }
            }
            if ((*it)->Flag(ENT_IS_TRIGGER) && (*it)->TriggerFlagsMaskTest(ent)) {
                (this->*(*it)->MyTrigger())(ent, *it, trigger_activation_type, ent->LastMoveDirection());
            }
            if ((*it)->Flag(ENT_IS_AOE_DAMAGING) && (*it)->AuraDamage().value > 0) {
                isaffected = true;
                ApplyEnvironmentEffect_AOE(ent, *it);
            }
        }
    }
    ents = grid.GetSquare(ent->Coor(), 1, GridComparer_EnvironmentAffecting());
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        if (!(*it)->is_dead) {
            if (ent->Flag(ENT_IS_UNIT) && (*it)->Flag(ENT_IS_OBELISK)) {
                ent->DrainMana(*it);
            }
            if ((*it)->Flag(ENT_IS_AOE_DAMAGING) && (*it)->AuraRadiusDamage().value > 0) {
                isaffected = true;
                ApplyEnvironmentEffect_AOE(ent, *it, true);
            }
            if ((*it)->Flag(ENT_IS_UNIT) && (*it)->IsEnchanted(ENCHANT_SHOCK)) {
                isaffected = true;
                ApplyEnvironmentEffect_Shock(ent, *it);
            }
        }
    }
    return isaffected;
}
void Battle::ApplyEnvironmentEffect_Source(Entity* effects_holder)
{
    if (effects_holder->Flag(ENT_IS_AOE_DAMAGING) && effects_holder->AuraDamage().value != 0) {
        EntContainer ents = grid.GetCell(effects_holder->Coor(), GridComparer_Mortal());
        for (auto it = ents.begin(); it != ents.end(); ++it) {
            if (!(*it)->IsDead() && *it != effects_holder) {
                ApplyEnvironmentEffect_AOE(*it, effects_holder);
            }
        }
    }
    if (effects_holder->Flag(ENT_IS_AOE_DAMAGING) && effects_holder->AuraRadiusDamage().value != 0) {
        EntContainer ents = grid.GetSquare(effects_holder->Coor(), 1, GridComparer_Mortal());
        for (auto it = ents.begin(); it != ents.end(); ++it) {
            ApplyEnvironmentEffect_AOE(*it, effects_holder, true);
        }
    }

    if (effects_holder->Flag(ENT_IS_UNIT) && effects_holder->IsEnchanted(ENCHANT_SHOCK)) {
        EntContainer ents = grid.GetSquare(effects_holder->Coor(), 1, GridComparer_Mortal());
        for (auto it = ents.begin(); it != ents.end(); ++it) {
            if (ApplyEnvironmentEffect_Shock(*it, effects_holder)) {
                break;
            }
        }
    }
    if (effects_holder->Flag(ENT_IS_TRIGGER)) {
        EntContainer ents = grid.GetCell(effects_holder->Coor());
        for (auto it = ents.begin(); it != ents.end(); ++it) {
            if (effects_holder->TriggerFlagsMaskTest(*it)) {
                (this->*effects_holder->MyTrigger())(*it, effects_holder, TRIGGERACTIVATE_TRIGGERMOVE, (*it)->LastMoveDirection());
            }
        }
    }
}
void Battle::ApplyEnvironmentEffect_AOE(Entity* ent, Entity* aoeer, bool radius) {
    if (!ent->Flag(ENT_IS_IMMORTAL) && (aoeer->AuraDamageAffectsFliers() || !(ent->Flag(ENT_IS_FLYING) || 
        (ent->Flag(ENT_IS_UNIT) && dynamic_cast<Unit*>(ent)->is_jumping)))) {
        Dmg damage = (radius) ? aoeer->AuraRadiusDamage() : aoeer->AuraDamage();
        switch (aoeer->AuraDamageType()) {
        case DefaultEntity_AOEDamage::DAMAGES_HEALTH:
            Damage(aoeer, ent, damage);
            break;
        case DefaultEntity_AOEDamage::DAMAGES_MANA:
            ent->BurnMP(damage.value);
            break;
        case DefaultEntity_AOEDamage::DAMAGES_BOTH:
            Damage(aoeer, ent, damage);
            ent->BurnMP(damage.value);
            break;
        };
    }
}
bool Battle::ApplyEnvironmentEffect_Shock(Entity* ent, Entity* shocker)
{    
    if (!ent->Flag(ENT_IS_IMMORTAL) && ent->Flag(ENT_IS_UNIT) && ent->Team() != shocker->Team()) {
        Damage(shocker, ent, ShockEnchantDamage);
        Disenchant(shocker, ENCHANT_SHOCK);
        //TODO: play proper sound
        return true;
    }
    return false;
}

void Battle::EndTurn() {
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
    EntContainer tmpent;
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        if (!(*it)->IsDead()) {
            tmpent.push_back(*it);
        }
    }
    //Current player's units finish turn
    ClearMoveTrace();
    CharmSelected = 0;
    ClearLActions();
    for (auto it = tmpent.begin(); it != tmpent.end(); ++it) {
        if ((*it)->Team() == curplayer) {
            EndTurn(*it);
        }
    }
    if (IsLActionsListFree()) {
        StartTurn();
    } else {
        SetLActionsMode_Endturn(true);
        CurMode = CURSORMODE_LTARGETING;
        NextLAction();
    }
}
void Battle::StartTurn() {
    renderer.GridIsChanged();
    renderer.InterfaceIsChanged();
    EntContainer tmpent;
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        if (!(*it)->IsDead()) {
            tmpent.push_back(*it);
        }
    }
    if (curplayer == players_count) {
        //Neutral objects complete their turn
        for (auto it = tmpent.begin(); it != tmpent.end(); ++it) {
            if ((*it)->Team() == NO_TEAM) {
                StartTurn(*it);
                //here goes the code for neutral objects activity
                EndTurn(*it);
            }
        }
        curplayer = static_cast<Team>(1);
    } else {
        curplayer = static_cast<Team>(static_cast<int>(curplayer)+1);
    }
    game->Graphic()->setPictureTexture(game->Interface.Flag, game->Flag(curplayer, false));

    ClearLActions();
    //New player's units start their turn
    for (auto it = tmpent.begin(); it != tmpent.end(); ++it) {
        if ((*it)->Team() == curplayer) {
            StartTurn(*it);
        }
    }
    for (auto it = tmpent.begin(); it != tmpent.end(); ++it) {
        if ((*it)->Team() == curplayer && !(*it)->IsDead() && (*it)->Flag(ENT_IS_PROJECTILE)) {
            MoveProjectile(*it);
        }
    }
    if (!IsLActionsListFree()) {
        SetLActionsMode_Endturn(false);
        CurMode = CURSORMODE_LTARGETING;
        NextLAction();
    }
}
void Battle::EndTurn(Entity* ent) {
    if (ent->IsDead()) { return; }
    ApplyEnvironmentEffects(ent, TRIGGERACTIVATE_END_TURN);
    
    if (ent->Flag(ENT_IS_UNIT)) {
        Unit* unit = dynamic_cast<Unit*>(ent);
        if (unit->IsBusy()) {                   //is doing something
            unit->long_action.time--; //incrementing time counter
            if (unit->long_action.time == 0) { //is finished
                AddLAction(unit);                   //adding to list for further processing
            }
        }
        for (int i = 0; i < Counters::Channeling_actions; ++i) {
            enumChannelingActions id = static_cast<enumChannelingActions>(i);
            if (unit->c_action[id].Is_Cast) {
                const Action_ChannelingEffect* cont_act = dynamic_cast<const Action_ChannelingEffect*>(game->DefAction(ContToCommon(id)));
                cont_act->ApplyEffect(this, ent, unit->c_action[id].target);
            }
        }
        if (IsEnchanted(unit, ENCHANT_PIXIES)) {
            auto ents = grid.GetSolidSquare(unit->Coor(), game->DefAction(ACTION_PIXIES)->Range(), GridComparer_Units());
            ents.sort(EntityCoorSorter(unit->Coor(), grid));
            for (auto it = ents.begin(); it != ents.end(); ++it) {
                if (unit->Team() != (*it)->Team() && grid.IsSeenFrom(unit->Coor(), (*it)->Coor())) {
                    LaunchHomingProjectile(ENT_PIXIE_SWARM, unit, *it);
                    Disenchant(unit, ENCHANT_PIXIES);
                    break;
                }
            }
        }
    }
    ent->EndTurn();
    //game->Delay(50);
}
void Battle::StartTurn(Entity* ent) {
    if (ent->IsDead()) { return; }
    ApplyEnvironmentEffects(ent, TRIGGERACTIVATE_START_TURN);
    Unit* unit = nullptr;
    if (ent->Flag(ENT_IS_UNIT)) {
        unit = dynamic_cast<Unit*>(ent);
    }
    if (unit != nullptr) {
        if (unit->IsBusy()) {
            unit->long_action.time--;
            if (unit->long_action.time == 0) {
                AddLAction(unit);
            }
        }
    }
    if (unit) {
        for (int i = 0; i < Counters::enchants; ++i) {
            enumEnchants id = static_cast<enumEnchants>(i);
            if (unit->IsEnchanted(id) && game->DefEnchant(static_cast<enumEnchants>(i))->does_wearoff_auto) {
                unit->EnchantReduce(id);
                if (!unit->IsEnchanted(id)) {
                    game->Sound()->PlaySound(game->DefEnchant(id)->sound_off);
                }
            }
        }
    }
    ent->StartTurn();
    if (unit) {
        if (unit->IsEnchanted(ENCHANT_POISON)) {
            Damage(unit, unit, PoisonDamage);
            game->Sound()->PlaySound(game->DefEnchant(ENCHANT_POISON)->metasound);
        }
    }

    if (ent->hp <= 0) {
        Kill(ent, ent);
    }
}
void Battle::NextLAction() {
    LongActions.i++;
    if ((unsigned)LongActions.i < LongActions.units.size()) {
        if (!LongActions.units[LongActions.i]->IsDead()) {
            SetCurSel(LongActions.units[LongActions.i]);
            CurMode = CURSORMODE_LTARGETING;
            renderer.MoveScreenTo(maxCoord((LongActions.units[LongActions.i]->Coor()) - renderer.FieldWindowSize/2, Coord1));
        } else {
            NextLAction();
        }
    } else { //finished processing LActions from this list.
        CurMode = CURSORMODE_SELECT;
        if (LongActions.is_endturn_list) {
            StartTurn();
        }
    }
}
void Battle::ClearLActions() {
    LongActions.units.clear();
    LongActions.i = -1;
}

//=======================ACTIONS=========================
void Battle::ApplyWeaponEnchant(const Weapon::WeaponEnchant& enchant, Entity *source, Entity *target, Angle attack_angle) {
    switch (enchant.type) {
    case CHANT_IMPACT:
        for (int i = 0; i < enchant.power; i++) {
            PushTo(target, attack_angle);
        }
        break;
    default:
        ApplyWeaponEnchant(enchant, source, target);
    };
}
void Battle::ApplyWeaponEnchant(const Weapon::WeaponEnchant& enchant, Entity *source, Entity *target, int damage_dealt) {
    switch (enchant.type) {
    case CHANT_ANCHORING:
        EnchantWith(target, ENCHANT_ANCHOR, enchant.power);
        break;
    case CHANT_FIRE_SPLASH:
        Damage(source, target, Dmg(DMG_FIRE, enchant.power, 0));
        //TODO: sound
        break;
    case CHANT_LEECH:
        source->RestoreHP(damage_dealt*enchant.power/100);
        break;
    case CHANT_MANA_BURN:
        target->BurnMP(damage_dealt*enchant.power/100);
        break;
    case CHANT_MANA_LEECH:
        target->BurnMP(damage_dealt*enchant.power/100);
        source->RestoreMP(damage_dealt*enchant.power/100);
        break;
    case CHANT_POISON:
        target->EnchantWith(*game->DefEnchant(ENCHANT_POISON), enchant.power);
        break;
    case CHANT_SHOCK_SPARK:
        Damage(source, target, Dmg(DMG_SHOCK, enchant.power, 0));
        //TODO: sound
        break;
    case CHANT_SLOW:
        target->EnchantWith(*game->DefEnchant(ENCHANT_SLOW), enchant.power);
        break;
    case CHANT_STUN:
        target->EnchantWith(*game->DefEnchant(ENCHANT_STUN), enchant.power);
    };
}
void Battle::MeleeStrike(Entity* source, const CoordI& coor, const Action* me) {
    const Weapon* weap = source->MyWeapon();
    Entity* target = grid(coor);
    if (!target->Flag(ENT_IS_IMMORTAL)) {
        int damaged = Damage(source, target, Dmg(weap->type, weap->BaseDamage, 0));
        for (int i = 0; i < max_weapon_enchants; ++i) {
            if (weap->Enchants[i].type == CHANT_IMPACT) {
                ApplyWeaponEnchant(weap->Enchants[i], source, target, Angle(source->Coor(), target->Coor()));
            } else {
                ApplyWeaponEnchant(weap->Enchants[i], source, target, damaged);
            }
        }
    }
}

void Battle::HellfireStaff(Entity* source, const CoordI& coor, const Action* me) {
    Angle main_angle = Angle(source->Coor(), coor);
    LaunchProjectile(ENT_FIREBALL_SMALL, source, main_angle);
    LaunchProjectile(ENT_FIREBALL_SMALL, source, Angle(main_angle.getDegrees() + HellStaff_Spread));
    LaunchProjectile(ENT_FIREBALL_SMALL, source, Angle(main_angle.getDegrees() - HellStaff_Spread));
}
void Battle::Deathray(Entity* source, const CoordI& coor, const Action* me) {
    Entity* target = grid(coor);
    if (!target->Flag(ENT_IS_IMMORTAL) && !UnitBlockTest(target, Angle(target->Coor(), source->Coor()))) {
        //if unit can reflect with shield of any type, and is directed right, then following won't be executed
        Damage(source, target, DeathrayDamage);
    }
}
void Battle::Inversion(Entity* source, const CoordI& coor, const Action* me) {
    auto ents = grid.GetSolidSquare(coor, me->Range(), GridComparer_Homings());
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        HomingProjectile* pr = dynamic_cast<HomingProjectile*>(*it);
        pr->target = pr->object_owner;
        SetOwner(source, pr);
    }
}
void Battle::Counterspell(Entity* source, const CoordI& coor, const Action* me) {
    auto ents = grid.GetSolidSquare(coor, me->Range(), GridComparer_VisibleFrom(coor, grid));
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        if ((*it)->Flag(ENT_IS_DISPELLABLE)) {
            Kill(*it, source);
        }
        if ((*it)->Flag(ENT_IS_UNIT)) {
            Unit* unit = dynamic_cast<Unit*>(*it);
            for (int id = 1; id < Counters::enchants; ++id) {
                if (game->DefEnchant(static_cast<enumEnchants>(id))->is_dispellable) {
                    Disenchant(*it, static_cast<enumEnchants>(id));
                }
            }
            for (int id = 1; id < Counters::Channeling_actions; ++id) {
                enumChannelingActions tmpid = static_cast<enumChannelingActions>(id);
                if (unit->GetC_Action(tmpid).Is_Cast) {
                    if (dynamic_cast<const Action_ChannelingEffect*>(game->DefAction(ContToCommon(tmpid)))->IsDispellable()) {
                        unit->GetC_Action(tmpid).UnSet();
                    }
                }
            }
        }
    }
}
void Battle::RingOfFire(Entity* source, const CoordI& coor, const Action* me) {
    int angle_dif = 360 / RingOfFire_Number;
    for (int num = 1, cur_angle = 0; num <= RingOfFire_Number; ++num, cur_angle += angle_dif) {
        LaunchProjectile(ENT_FIRERING_FLAME, source, Angle(cur_angle));
    }
}
void Battle::DispellUndead(Entity* source, const CoordI& coor, const Action* me) {
    Entity* dispeller = Create(game->DefEntity(ENT_DISPELLER_CENTER), coor, source->Team());
    SetOwner(source, dispeller);
    int angle_dif = 360 / DispellUndead_Number;
    for (int num = 1, cur_angle = 0; num <= DispellUndead_Number; ++num, cur_angle += angle_dif) {
        LaunchProjectile(ENT_DISPELLER_LIGHT, dispeller, Angle(cur_angle));
    }
}
void Battle::Meteor(Entity* source, const CoordI& coor, const Action* me) {
    Entity* meteor = Create(game->DefEntity(ENT_METEOR_SHADOW), coor, source->Team());
    SetOwner(source, meteor);
}
void Battle::FistOfVengeance(Entity* source, const CoordI& coor, const Action* me) {
    Entity* fist = Create(game->DefEntity(ENT_FIST_SHADOW), coor, source->Team());
    SetOwner(source, fist);
}
void Battle::Burn(Entity* source, const CoordI& coor, const Action* me) {
    Entity* flame = Create(game->DefEntity(ENT_FLAME), coor, source->Team());
    SetOwner(source, flame);
}
void Battle::MagicWall(Entity* source, const CoordI& coor, const Action* me) {
    if (grid.BlocksMoving(game->DefEntity(ENT_MAGICWALL), coor)) {
        return; //no wall for you
    }
    CoordI lefteven, leftodd, righteven, rightodd; //increments vectors
    switch (source->Coor().getDirection(coor)) {
    case UP:
        leftodd = lefteven = CoordI(LEFT);
        rightodd = righteven = CoordI(RIGHT);
        break;
    case DOWN:
        leftodd = lefteven = CoordI(RIGHT);
        rightodd = righteven = CoordI(LEFT);
        break;
    case LEFT:
        leftodd = lefteven = CoordI(DOWN);
        rightodd = righteven = CoordI(UP);
        break;
    case RIGHT:
        leftodd = lefteven = CoordI(UP);
        rightodd = righteven = CoordI(DOWN);
        break;
    case UP_LEFT:
        leftodd = CoordI(DOWN); lefteven = CoordI(LEFT);
        rightodd = CoordI(RIGHT); righteven = CoordI(UP);
        break;
    case UP_RIGHT:
        leftodd = CoordI(LEFT); lefteven = CoordI(UP);
        rightodd = CoordI(DOWN); righteven = CoordI(RIGHT);
        break;
    case DOWN_LEFT:
        leftodd = CoordI(RIGHT); lefteven = CoordI(DOWN);
        rightodd = CoordI(UP); righteven = CoordI(LEFT);
        break;
    case DOWN_RIGHT:
        leftodd = CoordI(LEFT); lefteven = CoordI(DOWN);
        rightodd = CoordI(UP); righteven = CoordI(RIGHT);
        break;
    };

    bool deleted = false;
    for (auto it = source->subs.begin(); it != source->subs.end(); (!deleted)? ++it : it ) {
        deleted = false;
        if (game->DefEntity(ENT_MAGICWALL) == &(*it)->GetPrototype()) {
            Kill(*it, *it);
            (*it)->object_owner = nullptr;
            it = source->subs.erase(it);
            deleted = true;
        }
    }
    CoordI wallcoor = coor;
    Entity* wall = Create(game->DefEntity(ENT_MAGICWALL), wallcoor, source->Team());
    SetOwner(source, wall);
    for (int num = 1; num <= WallHalfLength; ++num) {
        wallcoor += (num % 2 == 0) ? lefteven : leftodd;
        if (grid.BlocksMoving(game->DefEntity(ENT_MAGICWALL), wallcoor)) {
            break; //no more wall for this side
        }
        wall = Create(game->DefEntity(ENT_MAGICWALL), wallcoor, source->Team());
        SetOwner(source, wall);
    }
    wallcoor = coor;
    for (int num = 1; num <= WallHalfLength; ++num) {
        wallcoor += (num % 2 == 0) ? righteven : rightodd;
        if (grid.BlocksMoving(game->DefEntity(ENT_MAGICWALL), wallcoor)) {
            break; //no more wall for this side
        }
        wall = Create(game->DefEntity(ENT_MAGICWALL), wallcoor, source->Team());
        SetOwner(source, wall);
    }
}
void Battle::Push(Entity* source, const CoordI& coor, const Action* me) {
    auto ents = grid.GetSolidSquare(coor, me->Range(), GridComparer_Movable());
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        PushFrom(*it, coor);
        game->Delay(40);
    }
}
void Battle::Pull(Entity* source, const CoordI& coor, const Action* me) {
    auto ents = grid.GetSolidSquare(coor, me->Range(), GridComparer_Movable());
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        PushTo(*it, coor);
        game->Delay(40);
    }
}
void Battle::ReflectiveShield(Entity* source, const CoordI& coor, const Action* me) {
    EnchantWith(source, ENCHANT_REFLECTIVE_SHIELD);
}
void Battle::TeleportToTarget(Entity* source, const CoordI& coor, const Action* me) {
    Move(source, coor);
}
void Battle::ToxicCloud(Entity* source, const CoordI& coor, const Action* me) {
    Entity* cloud = nullptr;
    for (RectangleIterator it(coor - Coord1, coor + Coord1); it.isInside(); ++it) {
        cloud = Create(game->DefEntity(ENT_TOXIC_CLOUD), it.getCoor(), source->Team());
        SetOwner(source, cloud);
    }
}
void Battle::Earthquake(Entity* source, const CoordI& coor, const Action* me) {
    for (int radius = 1; radius <= me->Range(); ++radius) {
        auto ents = grid.GetSquare(coor, radius, GridComparer_Mortal());
        for (auto it = ents.begin(); it != ents.end(); ++it) {
            if ((*it)->Team() != source->Team()) {
                Damage(source, *it, Dmg(DMG_MAGIC, EarthquakeCentralDamage - radius*EarthquakeDamageDecreasePerCell));
                game->Delay(100);
            }
        }
    }
}
void Battle::LesserHeal(Entity* source, const CoordI& coor, const Action* me)
{
    Entity* target = grid(coor);
    if (target == source) {
        RestoreHP(target, HealValue);
    } else {
        LaunchHomingProjectile(ENT_HEALBALL, source, target);
    }
}
void Battle::Berserker(Entity* source, const CoordI& coor, const Action* me) {
    auto path = grid.TraceRay(source->Coor(), coor);
    int num = 1;
    for (auto it = path.begin()+1; it != path.end() && num <= me->Range(); ++it, ++num) {
        game->Delay(50);
        if (grid.BlocksMoving(source, *it)) {
            auto ents = grid.GetCell(*it, GridComparer_Mortal());
            bool is_unit = false;
            for (auto it = ents.begin(); it != ents.end(); ++it) {
                Damage(source, *it, BerserkerDamage);
                if ((*it)->Flag(ENT_IS_UNIT)) {
                    is_unit = true;
                }
            }
            if (!is_unit) {
                Damage(source, source, Dmg(DMG_MAGIC, source->hp*BersekerSelfdamageMultiplier, DFLAG_IGNORES_ALL));
            }
            break;
        } else {
            Move(source, *it);
            if (source->IsDead()) {
                return;
            }
        }
    }
}
void Battle::Harpoon(Entity* source, const CoordI& coor, const Action* me) {
    auto path = grid.TraceRay(source->Coor(), coor);
    int num = 1;
    Entity* hooked_to = nullptr;
    bool pull_itself = false;
    CoordI hook_coor;
    for (auto it = path.begin()+1; it != path.end() && num <= me->Range(); ++it, ++num) {
        game->Delay(50);
        if (grid.IsWindowlessWall(*it)) {
            pull_itself = true;
            hook_coor = *it;
            break;
        }
        auto ents = grid.GetCell(*it);
        for (auto it = ents.begin(); it != ents.end(); ++it) {
            if ((*it)->Flag(ENT_IS_SOLID) && (*it)->Flag(ENT_IS_TALL)) {
                hooked_to = *it;
                hook_coor = hooked_to->Coor();
                break;
            }
        }
    }
    if (hooked_to) {
        if (!hooked_to->Flag(ENT_IS_IMMOBILE)) { //pull the victim
            auto path = grid.TraceLine(hooked_to->Coor(), source->Coor());
            for (auto it = path.begin()+1; it != path.end(); ++it) {
                if (grid.BlocksMoving(hooked_to, *it)) {
                    return;
                } else {
                    Move(hooked_to, *it);
                    if (hooked_to->IsDead()) {
                        return;
                    }
                }
            }
        } else {
            pull_itself = true;
        }
    }
    if (pull_itself) {
        for (auto it = path.begin()+1; it != path.end(); ++it) {
            game->Delay(100);
            if (grid.BlocksMoving(source, *it)) {
                return;
            } else {
                Move(source, *it);
                if (source->IsDead()) {
                    return;
                }
            }
        }
    }
}
void Battle::Warcry(Entity* source, const CoordI& coor, const Action* me) {
    Counterspell(source, coor, me);
    auto ents = grid.GetSolidSquare(coor, me->Range(), GridComparer_MortalVisibleFrom(coor, grid));
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        if ((*it)->Team() != source->Team()) { //something more complex will be needed (stunning, frightening)
            EnchantWith(*it, ENCHANT_NULLIFICATION);
        }
    }
}

bool Battle::FireballCollide(Entity* projectile, const CoordI& dest, const Direction dir) {
    Kill(projectile, projectile);
    return false;
}
bool Battle::ArrowCollide(Entity* projectile, const CoordI& dest, const Direction dir) {
    Entity* target = grid(dest);
    if (target && !target->Flag(ENT_IS_IMMORTAL) && !UnitBlockTest(target, Angle(target->ExCoor(), projectile->ExCoor()))) {
        Damage(projectile->object_owner, grid(dest), ArrowDamage);
    }
    Kill(projectile, projectile);
    return false;
}
bool Battle::ShurikenCollide(Entity* projectile, const CoordI& dest, const Direction dir) {
    Entity* target = grid(dest);
    if (target && !target->Flag(ENT_IS_IMMORTAL) && !UnitBlockTest(target, Angle(target->ExCoor(), projectile->ExCoor()))) {
        Damage(projectile->object_owner, grid(dest), ShurikenDamage);
    }
    Kill(projectile, projectile);
    return false;
}
bool Battle::EmptyCollide(Entity* projectile, const CoordI& dest, const Direction dir) {
    return true;
}
bool Battle::EnchantballCollide(Entity* projectile, const CoordI& dest, const Direction dir) {
    Entity* target = grid(dest);
    if (!target || target != dynamic_cast<HomingProjectile*>(projectile)->target) {
        return false;
    }
    EnchantWith(target, dynamic_cast<EnchantProjectile*>(projectile)->enchant_id);
    Kill(projectile, projectile);
    return false;
}
bool Battle::HealballCollide(Entity* projectile, const CoordI& dest, const Direction dir) {
    Entity* target = grid(dest);
    if (!target || target != dynamic_cast<HomingProjectile*>(projectile)->target) {
        return false;
    }
    RestoreHP(target, HealValue);
    Kill(projectile, projectile);
    return false;
}
bool Battle::SwapballCollide(Entity* projectile, const CoordI& dest, const Direction dir) {
    Entity* target = grid(dest);
    if (!target || target != dynamic_cast<HomingProjectile*>(projectile)->target) {
        return false;
    }
    Kill(projectile, projectile);
    if (!target->Flag(ENT_IS_IMMOBILE) && !projectile->object_owner->Flag(ENT_IS_IMMOBILE)) {
        Swap(target, projectile->object_owner);
    }
    return false;
}
bool Battle::FireringCollide(Entity* projectile, const CoordI& dest, const Direction dir) {
    Entity* target = grid(dest);
    if (target && !target->Flag(ENT_IS_FLYING) && !target->Flag(ENT_IS_IMMORTAL)) {
        Damage(projectile->object_owner, target, RingOfFireDamage);
    }
    if (grid.IsWall(dest)) {
        Kill(projectile, projectile);
        return false;
    }
    return true;
}
bool Battle::DispellerCollide(Entity* projectile, const CoordI& dest, const Direction dir) {
    Entity* target = grid(dest);
    if (target && !target->Flag(ENT_IS_IMMORTAL) && target->Flag(ENT_IS_UNDEAD)) {
        int damaged = Damage(projectile->object_owner->object_owner, target,
            Dmg(DMG_MAGIC, min(target->HP(), projectile->object_owner->MP()), DFLAG_IGNORES_ALL));
        projectile->object_owner->mp -= damaged;
    }
    if (grid.IsWall(dest)) {
        Kill(projectile, projectile);
        return false;
    }
    return true;
}
bool Battle::MissileCollide(Entity* projectile, const CoordI& dest, const Direction dir) {
    Entity* target = grid(dest);
    if (!target || target != dynamic_cast<HomingProjectile*>(projectile)->target) {
        return false;
    }
    if (!target->Flag(ENT_IS_IMMORTAL) && !UnitBlockTest(target, Angle(target->ExCoor(), projectile->ExCoor()))) {
        Damage(projectile->object_owner, grid(dest), MissilesDamage);
    }
    if (!target->Flag(ENT_IS_IMMOBILE)) {
        PushFrom(target, projectile->Coor());
    }
    Kill(projectile, projectile);
    return false;
}
bool Battle::PixieCollide(Entity* projectile, const CoordI& dest, const Direction dir) {
    Entity* target = grid(dest);
    if (!target || target != dynamic_cast<HomingProjectile*>(projectile)->target) {
        return false;
    }
    if (!target->Flag(ENT_IS_IMMORTAL) && !UnitBlockTest(target, Angle(target->ExCoor(), projectile->ExCoor()))) {
        Damage(projectile->object_owner, grid(dest), PixiesDamage);
    }
    Kill(projectile, projectile);
    return false;
}

void Battle::FireballDie(Entity* object, Entity* killer) {
    auto ents = grid.GetSolidSquare(object->Coor(), 1, GridComparer_Mortal());
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        if ((*it)->Coor() == object->Coor() || !UnitBlockTest(*it, Angle((*it)->Coor(), object->Coor()))) {
            Damage(object->object_owner, *it, FireballBlastDamage);
            if (!(*it)->Flag(ENT_IS_IMMOBILE)) {
                PushFrom(*it, object->Coor());
            }
        }
    }
}
void Battle::FireballSmallDie(Entity* object, Entity* killer) {
    auto ents = grid.GetSolidSquare(object->Coor(), 1, GridComparer_Mortal());
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        if ((*it)->Coor() == object->Coor() || !UnitBlockTest(*it, Angle((*it)->Coor(), object->Coor()))) {
            Damage(object->object_owner, *it, FireballSmallBlastDamage);
            PushFrom(*it, object->Coor());
        }
    }
}
void Battle::MeteorShadowDie(Entity* object, Entity* killer) {
    auto ents = grid.GetSolidSquare(object->Coor(), 1, GridComparer_Mortal());
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        Damage(object->object_owner, *it, MeteorDamage);
    }
}
void Battle::FistShadowDie(Entity* object, Entity* killer) {
    auto ents = grid.GetSolidSquare(object->Coor(), 1, GridComparer_Mortal());
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        Damage(object->object_owner, *it, FistOfVengeanceDamage);
    }
}
void Battle::FONDie(Entity* object, Entity* killer) {
    if (killer != object && killer != nullptr) {
        Angle angle(dynamic_cast<CommonProjectile*>(object)->angle);
        LaunchProjectile(ENT_FON_SMALL, object, angle);
        LaunchProjectile(ENT_FON_SMALL, object, Angle(angle.getDegrees()+ForceOfNatureSpread));
        LaunchProjectile(ENT_FON_SMALL, object, Angle(angle.getDegrees()-ForceOfNatureSpread));
    }
}
void Battle::ToxicCloudTrigger(Entity* object_activator, Entity* object_trigger, enumTriggerActivationType activation_type, const Direction dir) {
    if (object_activator->Flag(ENT_IS_UNIT) && !object_activator->Flag(ENT_IS_UNDEAD)) {
        EnchantWith(object_activator, ENCHANT_POISON);
        Damage(object_trigger->object_owner, object_activator, ToxicCloudDamage);
    }
}
void Battle::DispellerTrigger(Entity* object_activator, Entity* object_trigger, enumTriggerActivationType activation_type, const Direction dir) {
    if (object_activator->Flag(ENT_IS_DISPELLABLE)) {
        Kill(object_activator, object_trigger);
    }
}

void Battle::LightningApply_common(Unit* source, Entity* target, const Action* me, int& magic_reserve) {
    Entity *target2 = nullptr, *target3 = nullptr;
    bool target_changed = true;
    for (int num = 0; num < LightningImpulseNumber; ++num) {
        game->Delay(30);
        if (magic_reserve < LightningImpulseCost) {
            source->GetC_Action(me->ChannelingID()).UnSet();
            return;
        }
        //if target is invalid, find new one
        if (grid.Dist(source->Coor(), target->Coor(), true) > me->Range() ||
            !grid.IsSeenFrom(source->Coor(), target->Coor()) || target->IsDead()) {
            target = nullptr;
            target_changed = true;
            auto ents = grid.GetSolidSquare(source->Coor(), me->Range(), GridComparer_Units());
            ents.sort(EntityCoorSorter(source->Coor(), grid));
            for (auto it = ents.begin(); it != ents.end(); ++it) {
                if (source->Team() != (*it)->Team() && grid.IsSeenFrom(source->Coor(), (*it)->Coor())) {
                    target = *it;
                    break;
                }
            }
            if (!target) {
                source->GetC_Action(me->ChannelingID()).UnSet();
                return;
            }
        }
        //target is valid, defining two secondaries
        if (target_changed) {
            auto ents = grid.GetSolidSquare(target->Coor(), SecondaryLightningsRange, GridComparer_Units());
            ents.sort(EntityCoorSorter(target->Coor(), grid));
            for (auto it = ents.begin(); it != ents.end(); ++it) {
                if (source->Team() != (*it)->Team() && grid.IsSeenFrom(target->Coor(), (*it)->Coor()) && *it != target) {
                    if (!target2) {
                        target2 = *it;
                    } else {
                        target3 = *it;
                        break;
                    }
                }
            }
        }
        //applying damage and reducing mana
        magic_reserve -= LightningImpulseCost;
        if (target->Coor() == source->Coor() || !UnitBlockTest(target, Angle(target->Coor(), source->Coor()), 0, true)) {
            Damage(source, target, LightningImpulseDamage);
        }
        if (target2 && (target2->Coor() == target->Coor() || !UnitBlockTest(target2, Angle(target2->Coor(), target->Coor()), 0, true))) {
            Damage(source, target2, LightningImpulseDamage);
        }
        if (target3 && (target3->Coor() == target->Coor() || !UnitBlockTest(target3, Angle(target3->Coor(), target->Coor()), 0, true))) {
            Damage(source, target3, LightningImpulseDamage);
        }
        target_changed = false;
    }
}
void Battle::LightningApply(Entity* source, Entity* target, const Action* me) {
    LightningApply_common(dynamic_cast<Unit*>(source), target, me, source->mp);
}
void Battle::LightningStaffApply(Entity* source, Entity* target, const Action* me) {
    Unit* unit = dynamic_cast<Unit*>(source);
    LightningApply_common(unit, target, me, unit->ammo);
}
void Battle::EnergyBoltApply(Entity* source, Entity* target, const Action* me) {
    for (int num = 0; num < EnergyboltImpulseNumber; ++num) {
        game->Delay(30);
        //if target is invalid, stop casting
        if (source->mp < LightningImpulseCost || grid.Dist(source->Coor(), target->Coor(), true) > me->Range() ||
            !grid.IsSeenFrom(source->Coor(), target->Coor()) || target->IsDead()) {
            dynamic_cast<Unit*>(source)->GetC_Action(me->ChannelingID()).UnSet();
            return;
        }
        //applying damage and reducing mana
        BurnMP(source, EnergyboltImpulseCost);
        if (!UnitBlockTest(target, Angle(target->Coor(), source->Coor()), 0, true)) {
            Damage(source, target, EnergyboltImpulseDamage);
        }
    }
}
void Battle::GreaterHealApply(Entity* source, Entity* target, const Action* me) {
    for (int num = 0; num < GreaterHealImpulseNumber; ++num) {
        //if target is invalid, stop casting
        if (source->mp < GreaterHealImpulseCost || grid.Dist(source->Coor(), target->Coor(), true) > me->Range() ||
            !grid.IsSeenFrom(source->Coor(), target->Coor()) || target->IsDead() || target->HP() == target->MaxHP()) {
            dynamic_cast<Unit*>(source)->GetC_Action(me->ChannelingID()).UnSet();
            return;
        }
        //applying damage and reducing mana
        BurnMP(source, GreaterHealImpulseCost);
        RestoreHP(target, GreaterHealImpulseHeal);
    }
}
void Battle::DrainManaApply(Entity* source, Entity* target, const Action* me) {
    int mana_to_drain = ManaDrainPerApply;
    auto ents = grid.GetSolidSquare(source->Coor(), me->Range(), GridComparer_Obelisks());
    ents.splice(ents.end(), grid.GetSolidSquare(source->Coor(), me->Range(), GridComparer_Units()));
    ents.sort(EntityCoorSorter(source->Coor(), grid));
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        game->Delay(100);
        if (source->MP() == source->MaxMP()) {
            dynamic_cast<Unit*>(source)->GetC_Action(me->ChannelingID()).UnSet();
        }
        if (!mana_to_drain) {
            return;
        }
        if (source->Team() != (*it)->Team()) {
            int tmp = min(mana_to_drain, (*it)->MP());
            BurnMP(*it, tmp);
            RestoreMP(source, tmp);
            mana_to_drain -= tmp;
        }
    }
}
void Battle::ChannelApply(Entity* source, Entity* target, const Action* me) {
    int tmp = min(min(source->MaxMP() - source->MP(), ChannelPerApply), source->HP() - 1);
    if (tmp == 0) {
        dynamic_cast<Unit*>(source)->GetC_Action(me->ChannelingID()).UnSet();
    }
    source->hp -= tmp;
    RestoreMP(source, tmp);
}
void Battle::CharmApply(Entity* source, Entity* target, const Action* me) {
    auto c_action = dynamic_cast<Unit*>(source)->GetC_Action(me->ChannelingID());
    if (grid.Dist(source->Coor(), target->Coor(), true) > me->Range() ||
        !grid.IsSeenFrom(source->Coor(), target->Coor()) || target->IsDead()) {
        c_action.UnSet();
    }
    ++c_action.metadata;
    if (c_action.metadata >= target->CharmSize()) {
        c_action.UnSet();
        target->team = source->Team();
        SetOwner(source, target);
    }
}
void Battle::ObliterationApply(Entity* source, Entity* target, const Action* me) {
    int tmp = min(ObliterationManaPerTurn, source->MP());
    BurnMP(source, tmp);
    auto c_action = dynamic_cast<Unit*>(source)->GetC_Action(me->ChannelingID());
    c_action.metadata += tmp;
    if (source->MP() == 0) {
        auto ents = grid.GetSolidSquare(source->Coor(), me->Range(), GridComparer_Mortal());
        for (auto it = ents.begin(); it != ents.end(); ++it) {
            Damage(source, *it, Dmg(DMG_MAGIC, c_action.metadata * ObliterationDamagePerMana, DFLAG_IGNORES_ALL));
        }
    }
}
ErrorBase* Battle::CheckConsistency() const {
    ErrorBase* tmp = nullptr;
    tmp = grid.CheckConsistency();
    if (tmp) { return tmp; }
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        if (!(*it)->IsDead()) {
            tmp = (*it)->CheckConsistency();
            if (tmp) { return tmp; }
            if (!(*it)->Coor().isInside(Coord1, grid.Size())) {
                return new Errors::EntityOutsideGrid(*it);
            }

            auto ents = grid.GetCell((*it)->Coor());
            bool hasnt_proper_entity = true;
            for (auto it2 = ents.begin(); it2 != ents.end(); ++it2) {
                if (*it2 == *it) {
                    hasnt_proper_entity = false;
                }
            }
            if (hasnt_proper_entity) {
                return new Errors::EntityNotInGrid(*it);
            }
        }
    }
    if (CurSel.ent && CurSel.ent->Coor() != CurSel.coor) {
        return new Errors::CurSelCoorMismatchingEntity();
    }
    if (Spellbar.CurSet < 0 || Spellbar.CurSet >= actionsets_count) {
        return new Errors::SpellsetNotInRange();
    }
    if (Spellbar.CurSpell < 0 || Spellbar.CurSpell > actions_per_set) {
        return new Errors::SpellNotInRange();
    }
    return nullptr;
}
void Logger::addEntry(enumStrings entry_base, LogArgument arg1 /*= LogArgument()*/, LogArgument arg2 /*= LogArgument()*/, LogArgument arg3 /*= LogArgument()*/) {
    if (entry_base == NO_STRING) { return; }
    string entrybase = game->String(entry_base); //TODO: implement logger without using Game*
    std::stringstream entry;
    unsigned int i = 0;
    bool prc_flag = true;
    while (i < entrybase.size()) {
        if (prc_flag) {
            if (entrybase[i] == '1') {
                entry << toString(arg1);
            } else if (entrybase[i] == '2') {
                entry << toString(arg2);
            } else if (entrybase[i] == '3') {
                entry << toString(arg3);
            }
        } else {
            if (entrybase[i] != '$') {
                entry << entrybase[i];
            } else {
                prc_flag = true;
            }
        }
        ++i;
    }
    string tmp;
    entry.str(tmp);
    log.push_back(tmp);
}
string Logger::toString(const LogArgument& arg) const {
    switch (arg.type) {
    case LOG_NOTHING:
        return "";
    case LOG_NUMBER:
        return int_to_string(arg.num);
    case LOG_COOR:
        return arg.coor.toString();
    case LOG_NAME:
        return game->String(arg.name);
    };
    return "";
}
const list<string>& Logger::getLog() const {
    return log;
}

void Replayer::addInput(const UserInput inp) {
    input_log.push_back(inp);
}
const list<Replayer::UserInput>& Replayer::getInputLog() const {
    return input_log;
}
