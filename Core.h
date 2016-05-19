#pragma once
#include "NoxTacticEngine.h"
#include "Geometry.h"
#include "Constants.h"
#include "Grid.h"
#include "Entity.h"
#include "Enchants.h"
#include "IO.h"
#include "Actions.h"
#pragma warning(disable:4244)
extern EngineCore* core;
using namespace GameBaseConstants;
class Game;
class Battle;
class MapEditor;
class Graphics;
class Input;
#define use_mapeditor true //HACK: no game menu yet

class Map
{
    CoordI size;
    vector<Entity *> entities;
    vector<vector<Wall*>> walls;
    vector<vector<Tile*>> tiles;
    int players_count;
public:
    Map(const CoordI& size, const vector<Entity *>& entities, vector<vector<Wall*>> walls, vector<vector<Tile*>> tiles, const int players_count):
        size(size), entities(entities), walls(walls), tiles(tiles), players_count(players_count) {}
    ~Map();
    void Destroy();
    friend class Battle;
};

class Logger
{
    Game* game;
    list<string> log;
    enum LogArgumentType
    {
        LOG_NOTHING,
        LOG_COOR,
        LOG_NAME,
        LOG_NUMBER
    };
    struct LogArgument
    {
        CoordI coor;
        enumStrings name;
        int num;
        LogArgumentType type;
        LogArgument(CoordI coor): coor(coor), name(NO_STRING), num(0), type(LOG_COOR) {}
        LogArgument(enumStrings name): coor(), name(name), num(0), type(LOG_NAME) {}
        LogArgument(int num): coor(), name(NO_STRING), num(num), type(LOG_NUMBER) {}
        LogArgument(): coor(), name(NO_STRING), num(0), type(LOG_NOTHING) {}
    };
    string toString(const LogArgument& arg) const;
public:
    void addEntry(enumStrings entry_base, LogArgument arg1 = LogArgument(), LogArgument arg2 = LogArgument(), LogArgument arg3 = LogArgument());
    const list<string>& getLog() const;
    Logger(Game* game): game(game) {}
};
class Replayer
{
public:
    enum UserInputType
    {
        UINPUT_NOTHING,
        UINPUT_CELLCLICK,
        UINPUT_BUTTONCLICK,
        UINPUT_KEYCLICK
    };
    class UserInput
    {
        CoordI cell_coor;
        Buttons button_id;
        Keys key_id;
        union
        {
            long mouseflags;
            int index;
        };
        UserInputType type;
    public:
        UserInput(CoordI cell_coor, long mouseflags): cell_coor(cell_coor), button_id(NO_BUTTON), key_id(NO_KEY), mouseflags(mouseflags), type(UINPUT_CELLCLICK) {}
        UserInput(Buttons button_id, int index): cell_coor(), button_id(button_id), key_id(NO_KEY), index(index), type(UINPUT_BUTTONCLICK) {}
        UserInput(Keys key_id): cell_coor(), button_id(NO_BUTTON), key_id(key_id), mouseflags(0), type(UINPUT_KEYCLICK) {}
        UserInput(): cell_coor(), button_id(NO_BUTTON), key_id(NO_KEY), mouseflags(0), type(UINPUT_NOTHING) {}
    };
private:
    list<UserInput> input_log;
public:
    void addInput(const UserInput inp);
    const list<UserInput>& getInputLog() const;
};

struct TraceStep
{
    CoordI coor;
    bool IsJump, IsJumpEnd;
    Direction dir, prevdir;
    TraceStep(const CoordI& coor, bool IsJump, bool IsJumpEnd, Direction dir, Direction prevdir):
        coor(coor), IsJump(IsJump), IsJumpEnd(IsJumpEnd), dir(dir), prevdir(prevdir) {}
};
struct TraceHolder
{
    vector<TraceStep> trace;
};
struct ObjectList
{
    vector<int> content;
    ObjectlistType mode;
    int selected;
    int page;
    int size;
    ObjectList(int size): size(size), content(size), mode(OBJLIST_NOTHING), selected(-1), page(0) {}
};
typedef pair<TextureHandler, TextureHandler> ModeIconTexture;

enum ButtonState
{
    BUTTONSTATE_BASE,
    BUTTONSTATE_ACTIVE,
    BUTTONSTATE_DISABLED
};
enum ScrollState
{
    SCROLLSTATE_BASE,
    SCROLLSTATE_UP,
    SCROLLSTATE_DOWN
};
class Renderer
{
    Game* game;         //info arrays
    Graphics* graphics;

    MapEditor* mapeditor;   //HACK: these should be heritors from common class (in case there would be third similar class)
    Battle* battle;

    bool grid_is_changed, interface_is_changed;
    Grid* grid;

    void RenderCell(const CoordI& coor);
    void RenderEntity(const Entity* ent, bool ClearCell = false);
    void RenderSelection();
    void RenderSelection(const CoordI& coor);
    void RenderEntityInfo(const Entity* ent);
    void RenderEntityInfo(); //renders info of currently selected unit; does RenderSpellbar(); in it
    void RenderSpellbar(const Entity* ent);   //renders spellbar of currently selected unit
    void RenderModeBar();
    void RenderMoveTrace();
    void RenderMoveArrow(const TraceStep& step);
    void RenderObjectList();
public:
    Renderer(Game* game, Graphics* graphics, Battle* battle, MapEditor* mapeditor);
    CoordI MapScreenPosition; //Upper-left corner of the game screen (on field)
    CoordI FieldWindowSize;   //Number of cells displayed on screen
    void MoveScreen(const Direction dir);
    void MoveScreenTo(const CoordI& coor);
    void RefreshScreen();
    void DisplayButtonPressed(Buttons button);
    void DisplaySpellsetScrolled(ScrollState state);

    void GridIsChanged() { grid_is_changed = true; }
    void InterfaceIsChanged() { interface_is_changed = true; }
};

class Game: public EngineCore
{
    ContainerDefs::StringContainer  Strings;
    ContainerDefs::GestureContainer Gestures;
    ContainerDefs::EnchantContainer Enchants;
    ContainerDefs::ActionContainer  Actions;
    ContainerDefs::EntityContainer  Entities;
    ContainerDefs::WeaponContainer  Weapons;
    ContainerDefs::MapContainer     Maps;
    ContainerDefs::WallContainer    Walls;
    ContainerDefs::TileContainer    Tiles;
    Battle *battle;
    MapEditor *mapeditor;
    struct TextureHolderStructure
    {
        TextureHandler Selection_Texture;
        TextureHandler MoveArrows_Texture[Counters::directions];
        TextureHandler Flags[max_flags], Miniflags[max_flags];
        TextureHandler Actionbar_base, Actionbar_leftblank, Actionbar_rightblank, Actionbar_title, Actionbar_nuggets[actions_per_set];
        TextureHandler Actionbar_trap, Actionbar_trapactive, Actionbar_bomber, Actionbar_bomberactive, Actionbar_spellbook, Actionbar_spellbookactive;
        TextureHandler Actionbar_leftscroll, Actionbar_leftscrolldown, Actionbar_leftscrollup;
        struct ButtonTextureHolder
        {
            TextureHandler base, active, disabled;
        };
        ButtonTextureHolder buttons_common[Counters::button_types], buttons_radio[Counters::radiobutton_types], button_cross;
    } TextureHolder;
    
    ::Input* input;
public:
    struct InterfaceStructure
    {
        PictureHandler spellbar_left, spellbar_right, spellbar_base;
        PictureHandler spellbar_titlebar;
        PictureHandler spell_icons[actions_per_set];
        PictureHandler nuggets[actions_per_set];
        PictureHandler Flag;
        PictureHandler objectlist_selector;
        LabelHandler spellbar_title;
        vector<PictureHandler> objectlist_mini;
        struct Button
        {
            TextureHolderStructure::ButtonTextureHolder *texture;
            string caption;
            PictureHandler pic;
            LabelHandler label;
            CoordI size, position, label_position;
            Buttons button_id;
            Button(TextureHolderStructure::ButtonTextureHolder *texture, const CoordI& position, const CoordI& size, const CoordI& label_shift, const string& caption, Buttons button_id):
                texture(texture), caption(caption), size(size), position(position), label_position(position+label_shift), button_id(button_id) {}
            Button(): texture(nullptr), caption(), size(), position(), button_id(NO_BUTTON) {}
        };
        Button EndTurnButton, objectlist_scrollup, objectlist_scrolldown, objectlist_expand, SaveButton, LoadButton, ExitButton;

        struct
        {
            PictureHandler portrait, status_tip_icon;
            LabelHandler name, hp, mp, speed, can_act, ammo, status_tip; //direction, coor, team, is blocking, owner
            vector<PictureHandler> enchantbar;
        } EntInfo;
    } Interface;
    Graphics* Graphic();
    ::Input* Input();
    SoundCore* Sound();
    void SetInput(::Input* input);
    Game(EngineCore core);
    void MainLoop();

    const Enchant*       DefEnchant(enumEnchants id) const { return Enchants[id]; }
    const std::string    String(enumStrings id) const { return Strings[id]; }
    const DefaultEntity* DefEntity(enumEntities id) const { return Entities[id]; }
    const Wall*          DefWall(enumWalls id) const { return Walls[id]; }
    const Tile*          DefTile(enumTiles id) const { return Tiles[id]; }
    const Map*           DefMap(int id) const { return Maps[id]; }
    const Weapon*        DefWeapon(enumWeapons id) const { return Weapons[id]; }
    const Action*        DefAction(enumActions id) const { return Actions[id]; }
    const Gesture*       DefGesture(enumGestures id) const { return Gestures[id]; }

    virtual void init();
    virtual void shutdown();
    virtual void frame();
    virtual LRESULT WINAPI MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual ~Game(){}

    void StartBattle(Battle* battle);
    void EndBattle();
    void StartMapEditor(MapEditor* mapeditor);
    void EndMapEditor();

    void CellClick(const CoordI& coor, const long flags);
    void KeyClick(const Keys key);
    void ButtonClick(const Buttons button, int index = 0);
    void DecodeInput(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void RefreshScreen();
    void Delay(long milliseconds = 100);

    void InitAction(Action* action);
    void InitEnt(DefaultEntity* defent);
    void InitEnchant(Enchant* enchant);
    void InitWall(Wall* wall);
    void InitTile(Tile* tile);
    void InitButton(InterfaceStructure::Button& button);


    TextureHandler MoveArrow_Texture(Direction dir) const;
    TextureHandler SelectionFrame_Texture() const;

    TextureHandler ButtonCommon_texture(int type, ButtonState state) const;
    TextureHandler ButtonRadio_texture(int type, ButtonState state) const;
    TextureHandler ButtonCross_texture(ButtonState state) const;
    TextureHandler ActionbarSpellbook(ButtonState state) const;
    TextureHandler ActionbarRightTrap(ButtonState state) const;
    TextureHandler ActionbarRightBomber(ButtonState state) const;
    TextureHandler ActionbarRightBlank() const;
    TextureHandler ActionbarLeftBlank() const;
    TextureHandler ActionbarLeftScroll(ScrollState state) const;
    TextureHandler ActionbarBase() const;
    TextureHandler ActionbarTitle() const;
    TextureHandler Flag(Team team, bool mini = true) const;
};

class MapEditor
{
    Game* game;
    Renderer renderer;
    vector<Entity*> ents;  //list of all entities in field
    Grid grid;              //pointers from "ents" arranged on the map
    int players_count;      //Number of players in game
    CursorMode CurMode;     //Current mode of cursor

    struct CurrentlySelected
    {
        Entity *ent;
        CoordI coor;
        void Set(Entity *entity, const CoordI& coord) {
            ent = entity;
            coor = coord;
        }
        void SetNull() {
            ent = nullptr;
            coor = CoordI();
        }
        CurrentlySelected(): ent(nullptr), coor() {}
    } CurSel;   //Currently selected entity/cell
    struct Spell_bar
    {
        int CurSet;
        int CurSpell;
        int ID() { return CurSet * actions_per_set + CurSpell; }
        Spell_bar(): CurSet(0), CurSpell(0) {}
    } Spellbar;
    CursorMode modes_panel[Counters::map_editor_modes];
    vector<ModeIconTexture> modes_icons;
    int CharmSelected;
    ObjectList objectlist;
public:
    MapEditor(const CoordI& size, Game* game);
    const Grid& Grid() const { return grid; }

    void CellClick(const CoordI& coor, const long flags);
    void KeyClick(const Keys Key);
    void ButtonClick(const Buttons Button, int index = 0);
    void ChooseMode(const int id);

    void SetCurSel(Entity *entity);
    void SetCurSel(const CoordI& coord);
    void SetCurSel();

    void SetActiveModeIcon(const CursorMode curmode);
    void SetActiveModeIcon();
    void ClearSpellbar();
    //returns pointer to newly created entity
    Entity* Create(const DefaultEntity* prototype, const CoordI& coor, Team team, Direction dir = NO_DIRECTION);
    void SetOwner(Entity* owner, Entity* sub);
    void RemoveOwner(Entity* owner, Entity* sub);
    ~MapEditor() {
        for (auto it = ents.begin(); it != ents.end(); ++it) {
            delete (*it);
        }
    }
    friend Game;
    friend Renderer;
};
class Battle {
    Game* game;     
    Logger logger;
    Replayer replayer;
    Renderer renderer;
    const Map* map;         //the battle's map
    vector<Entity*> ents;  //list of all entities in field
    Grid grid;              //pointers from "ents" arranged on the map
    Team curplayer;         //Current Player
    int players_count;      //Number of players in game
    CursorMode CurMode;     //Current mode of cursor
    struct CurrentlySelected
    {
        Entity *ent;
        CoordI coor;
        void Set(Entity *entity, const CoordI& coord) { 
            ent = entity;
            coor = coord;
        }
        void SetNull() {
            ent = nullptr;
            coor = CoordI();
        }
        CurrentlySelected(): ent(nullptr), coor(){}
    } CurSel;   //Currently selected entity/cell
    struct Spell_bar
    {
        int CurSet;
        int CurSpell;
        int ID() { return CurSet * actions_per_set + CurSpell; }
        void PrevSet() {
            --CurSet;
            if (CurSet < 0) {
                CurSet = actionsets_count-1;
            }
        }
        void NextSet() {
            ++CurSet;
            if (CurSet >= actionsets_count) {
                CurSet = 0;
            }
        }
        Spell_bar(): CurSet(0), CurSpell(0) {}   
    } Spellbar;         //Currently selected spell/spellset
    int CharmSelected;                 
    TraceHolder MoveTrace;                                         //Container of currently placed move trace
    struct LActionsInCompletionInfo
    {
        vector<Unit *> units;
        int i;
        //shows whether this list is for units ending their turn. 
        //if it is, after finishing processing their LActions, starting next turn is initiated
        bool is_endturn_list; 
    } LongActions;          //Currently processed long actions with expired time
public:
    ErrorBase* CheckConsistency() const;
    Battle(const Map* map, Game* game);
    const Grid& Grid() const { return grid; }
    void EndTurn();
    void StartTurn();
    void EndTurn(Entity* ent);
    void StartTurn(Entity* ent);

    void ClearLActions();
    void AddLAction(Unit* unit) { LongActions.units.push_back(unit); }
    void NextLAction();
    void SetLActionsMode_Endturn(bool value) { LongActions.is_endturn_list = value; }
    bool IsLActionsListFree() const { return LongActions.units.empty(); }
    //Scans small square around entity and applies effects of the environment; return true, if something affected entity;
    bool ApplyEnvironmentEffects(Entity* ent, enumTriggerActivationType type);
    //Scans small square around entity and applies its effects to everything around
    void ApplyEnvironmentEffect_Source(Entity* effects_holder);
    //Applies AOE damaging effect
    void ApplyEnvironmentEffect_AOE(Entity* ent, Entity* aoeer, bool radius = false);
    //Applies SHOCK enchant effect
    bool ApplyEnvironmentEffect_Shock(Entity* ent, Entity* shocker);

    void CellClick(const CoordI& coor, const long flags);
    void KeyClick(const Keys Key);
    void ButtonClick(const Buttons Button, int index = 0);
    void ChooseSpell(const int id);
    void ChangeSpellset(bool next_set);

    void SetActiveSpellIcon(const int id);
    void ClearSpellbar();

    void SetCurSel(Entity *entity);
    void SetCurSel(const CoordI& coord);
    void SetCurSel();

    void EnchantWith(Entity* ent, const Enchant& enchant, int time = 0);
    void EnchantWith(Entity* ent, enumEnchants enchantid, int time = 0);
    void Disenchant(Entity* ent, enumEnchants enchantid);
    bool IsEnchanted(const Entity* ent, enumEnchants enchantid) const;

    void RestoreHP(Entity* ent, int value);
    void RestoreMP(Entity* ent, int value);
    void BurnMP(Entity* ent, int value);
    int Damage(Entity* source, Entity* target, const Dmg damage);
    void Kill(Entity* ent, Entity* killer);
    void StopChannelActions(Entity* ent, bool dispell = false);
    //return true, if further automatic move is impossible
    bool Move(Entity* ent, const CoordI& dest, const Direction dir = NO_DIRECTION, bool Env = true, bool EnvSource = true);
    bool PushTo(Entity* ent, const Direction dir);
    void PushTo(Entity* ent, const CoordI& coor);
    void PushTo(Entity *ent, const Angle angle);
    void PushFrom(Entity* ent, const CoordI& coor);
    void Swap(Entity* ent1, Entity* ent2);
    void Turn(Entity* ent, const Direction dir);
    //returns pointer to newly created entity
    Entity* Create(const DefaultEntity* prototype, const CoordI& coor, Team team, Direction dir = NO_DIRECTION);
    void SetOwner(Entity* owner, Entity* sub);
    void RemoveOwner(Entity* owner, Entity* sub);
    void ApplyWeaponEnchant(const Weapon::WeaponEnchant& enchant, Entity *source, Entity *target, Angle attack_angle);
    void ApplyWeaponEnchant(const Weapon::WeaponEnchant& enchant, Entity *source, Entity *target, int damage_dealt = 0);

    void LaunchProjectile(enumEntities projectile_id, Entity* source, const CoordI& dest);
    void LaunchProjectile(enumEntities projectile_id, Entity* source, const Angle& angle);
    void LaunchHomingProjectile(enumEntities projectile_id, Entity* source, Entity* target, enumEnchants id = NO_ENCHANT);
    void MoveProjectile(Entity* projectile, bool IsFirstMove = false);
    bool ProjectileReflectionShieldTest(const Projectile* projectile, const Unit* unit, const CoordD& ProjectilePoint) const;
    bool ProjectileReflection(Entity* projectile, const CoordD& oldexcoor, const CoordD& excoor, Angle& angle);
    bool UnitBlockTest(Entity* entity_to_test, const Angle& BlockingAngle, 
        int BlockTypes = BLOCK_QUICKSHIELD+BLOCK_SHIELD, bool TestMagicShield = true) const;

    void AddStep(const CoordI& coor);
    void ClearMoveTrace();
    int EvaluateMoveCost() const;
    int EvaluateStepCost(const TraceStep& step) const;
    void ApplyUnitMove();


    ~Battle() {
        for (auto it = ents.begin(); it != ents.end(); ++it) {
            delete (*it);
        }
    }
    friend Game;
    friend Renderer;


    bool CheckActionValidity(const Action* action, const Entity* actor, const CoordI& target);
    void PayActionCost(const Action* action, Entity* actor, const CoordI& target);
    void ProjectileActionPerform(Entity* source, const CoordI& coor, const Action* me);
    void EnchantSpellPerform(Entity* source, const CoordI& coor, const Action* me);
    void ChannelingActionPerform(Entity* source, const CoordI& coor, const Action* me);

    //===========ACTIONS=============
    void MeleeStrike(Entity* source, const CoordI& coor, const Action* me);
    void LightningApply_common(Unit* source, Entity* target, const Action* me, int& magic_reserve);
    void LightningApply(Entity* source, Entity* target, const Action* me);
    void LightningStaffApply(Entity* source, Entity* target, const Action* me);
    void EnergyBoltApply(Entity* source, Entity* target, const Action* me);
    void GreaterHealApply(Entity* source, Entity* target, const Action* me);
    void DrainManaApply(Entity* source, Entity* target, const Action* me);
    void CharmApply(Entity* source, Entity* target, const Action* me);
    void ObliterationApply(Entity* source, Entity* target, const Action* me);
    void ChannelApply(Entity* source, Entity* target, const Action* me);

    void HellfireStaff(Entity* source, const CoordI& coor, const Action* me);
    void Deathray(Entity* source, const CoordI& coor, const Action* me);
    void Inversion(Entity* source, const CoordI& coor, const Action* me);
    void Counterspell(Entity* source, const CoordI& coor, const Action* me);
    void RingOfFire(Entity* source, const CoordI& coor, const Action* me);
    void DispellUndead(Entity* source, const CoordI& coor, const Action* me);
    void Meteor(Entity* source, const CoordI& coor, const Action* me);
    void FistOfVengeance(Entity* source, const CoordI& coor, const Action* me);
    void Burn(Entity* source, const CoordI& coor, const Action* me);
    void MagicWall(Entity* source, const CoordI& coor, const Action* me);
    void Push(Entity* source, const CoordI& coor, const Action* me);
    void Pull(Entity* source, const CoordI& coor, const Action* me);
    void ReflectiveShield(Entity* source, const CoordI& coor, const Action* me);
    void TeleportToTarget(Entity* source, const CoordI& coor, const Action* me);
    void ToxicCloud(Entity* source, const CoordI& coor, const Action* me);
    void Earthquake(Entity* source, const CoordI& coor, const Action* me);
    void LesserHeal(Entity* source, const CoordI& coor, const Action* me);
    void Berserker(Entity* source, const CoordI& coor, const Action* me);
    void Harpoon(Entity* source, const CoordI& coor, const Action* me);
    void Warcry(Entity* source, const CoordI& coor, const Action* me);
    void HammerStrike(Entity* source, const CoordI& coor, const Action* me) {}

    void FireballDie(Entity* object, Entity* killer);
    void FireballSmallDie(Entity* object, Entity* killer);
    void MeteorShadowDie(Entity* object, Entity* killer);
    void FistShadowDie(Entity* object, Entity* killer);
    void FONDie(Entity* object, Entity* killer);

    bool FireballCollide(Entity* projectile, const CoordI& dest, const Direction dir);
    bool ArrowCollide(Entity* projectile, const CoordI& dest, const Direction dir);
    bool ShurikenCollide(Entity* projectile, const CoordI& dest, const Direction dir);
    bool EmptyCollide(Entity* projectile, const CoordI& dest, const Direction dir);
    bool EnchantballCollide(Entity* projectile, const CoordI& dest, const Direction dir);
    bool HealballCollide(Entity* projectile, const CoordI& dest, const Direction dir);
    bool SwapballCollide(Entity* projectile, const CoordI& dest, const Direction dir);
    bool FireringCollide(Entity* projectile, const CoordI& dest, const Direction dir);
    bool DispellerCollide(Entity* projectile, const CoordI& dest, const Direction dir);
    bool MissileCollide(Entity* projectile, const CoordI& dest, const Direction dir);
    bool PixieCollide(Entity* projectile, const CoordI& dest, const Direction dir);
    //TODO: add paralyzers for obliteration/charming
    void TeleportPentagramTrigger(Entity* object_activator, Entity* object_trigger, enumTriggerActivationType activation_type, const Direction dir) {}
    void ToxicCloudTrigger(Entity* object_activator, Entity* object_trigger, enumTriggerActivationType activation_type, const Direction dir);
    void DispellerTrigger(Entity* object_activator, Entity* object_trigger, enumTriggerActivationType activation_type, const Direction dir);
    
};


namespace Loader
{
    void loadActions(ActionContainer& container,
        FunctionContainer& functions, ChannelingFunctionContainer& Channeling_functions);

    void loadGestures(GestureContainer& container);

    void loadEnchants(EnchantContainer& container);

    void loadMaps(MapContainer& container, EntityContainer& entities,
        WallContainer& walls, TileContainer& tiles);
};