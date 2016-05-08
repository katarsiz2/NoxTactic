#pragma once
#include <vector>
#include <list>
#include <string>
#include "Strings.h"
#include "Geometry.h"
class Entity;
#include "Entity.h"
typedef std::list<Entity*> EntContainer;
typedef std::vector<CoordI> Path;
class GridComparer_All
{
public:
    bool operator()(const Entity* ent) { return !ent->IsDead(); }
};

enum enumTiles 
{
    NO_TILE,
    TILE_LAVA,
    TILE_RUG,
    TILE_GRASS
    //TODO: finish tiles list
};
enum enumWalls
{
    NO_WALL,
    WALL_COBBLESTONE,
    WALL_BRICK,
    WALL_UNDERWORLD,
    WALL_MAGIC
    //TODO: finish walls list
};
class Tile
{
    TextureHandler texture_id;
    enumStrings name;
public:
    Tile(enumStrings name): name(name) {}
    Tile(): texture_id(0), name(NO_STRING) {}
    void SetTexture(TextureHandler texture) { texture_id = texture; }
    TextureHandler MyTexture() const { return texture_id; }
    enumStrings Name() const { return name; }
};
class Wall
{
    TextureHandler texture_id;
    bool window;
    bool is_destructible;
    enumStrings name;
public:
    Wall(enumStrings name, bool window, bool is_destructible = false):
        window(window), name(name), is_destructible(is_destructible) {}
    Wall(): texture_id(0), window(false), name(NO_STRING), is_destructible(false) {}
    TextureHandler MyTexture() const { return texture_id; }
    bool HasWindow() const { return window; }
    bool IsDestructible() const { return is_destructible; }
    void SetTexture(TextureHandler texture) { texture_id = texture; }
    enumStrings Name() const { return name; }
};

namespace Loader
{
    using namespace ContainerDefs;
    void loadTiles(TileContainer& container);
    void loadWalls(WallContainer& container);
};

class Grid
{
    struct Cell
    {
        list<Entity *> ents;
        bool is_wall;
        const Wall* wall;
        const Tile* tile;
        int wave;
        Cell(): ents(), is_wall(false), wall(nullptr), tile(nullptr), wave(0) {}
    };
    std::vector<std::vector<Cell>> cells;
    CoordI size;
    int nextWave = 1;

    list<Entity*>::iterator FindMe(Entity* value);
    Cell& _GetCell(const CoordI& coor);
    const Cell& _GetCell(const CoordI& coor) const;
public:
    ErrorBase* CheckConsistency() const;
    bool IsInside(const CoordI& v) const;
    Grid(const CoordI& size);
    //note that it DOES change ent coordinates
    void Move(Entity* ent, const CoordI& dest, const Direction dir = NO_DIRECTION);
    void Swap(Entity* ent1, Entity* ent2);
    void Add(Entity* ent);
    void SetWall(const CoordI& coor, const Wall* wall);
    void SetTile(const CoordI& coor, const Tile* tile);
    
    void RemoveWall(const CoordI& coor);
    bool IsWall(const CoordI& coor) const;
    bool IsWindow(const CoordI& coor) const;
    bool IsWindowlessWall(const CoordI& coor) const;
    bool BlocksMoving(const Entity* ent, const CoordI& dest, bool UnitJumps = false) const;
    bool BlocksMoving(const DefaultEntity* ent, const CoordI& dest, bool UnitJumps = false) const;
    TextureHandler GetTileTexture(const CoordI& coor) const;
    TextureHandler GetWallTexture(const CoordI& coor) const;
 
    //bool CausesCollide(const Projectile* proj, const vInt& dest) const;
    bool BlocksSight(const CoordI& dest) const;
    bool IsSeenFrom(const CoordI& start, const CoordI& dest) const;
    Entity* operator()(const int x, const int y) const;
    Entity* operator()(const CoordI& coor) const { return (*this)(coor.x, coor.y); }
    int sizex() const { return size.x; }
    int sizey() const { return size.y; }

    CoordD NearestCellVerge(const CoordD& minimizer, const CoordI& coor) const;
    CoordI Size() const { return size; }

    Path TraceRay(CoordI start, CoordI dest) const;
    Path TraceLine(CoordI start, CoordI dest) const;
    Path FindPath(CoordI start, CoordI dest, const Entity* ent);

    int Dist(const CoordI& start, const CoordI& dest, bool diagonal) const;

    //Comparer has to have "bool operator()(const Entity*);"
    template<class Comparer> 
    void Getter_ProcessCell(EntContainer& container, const CoordI& coor, Comparer comparer) const {
        if (IsInside(coor)) {
            const Cell& cell = _GetCell(coor);
            for (auto it = cell.ents.begin(); it != cell.ents.end(); ++it) {
                if (comparer(*it)) {
                    container.push_back(*it);
                }
            }
        }
    }
    void Getter_ProcessCell(EntContainer& container, const CoordI& coor) const {
        return Getter_ProcessCell(container, coor, GridComparer_All());
    }
    
    template<class Comparer> 
    EntContainer GetCell(const CoordI& coor, Comparer comparer) const {
        EntContainer container;
        Getter_ProcessCell(container, coor, comparer);
        return container;
    }
    EntContainer GetCell(const CoordI& coor) const { 
        return GetCell(coor, GridComparer_All()); 
    }
    template<class Comparer> 
    EntContainer GetRing(const CoordI& center, const int radius, Comparer comparer) const; //TODO: implement ringed getter
    EntContainer GetRing(const CoordI& center, const int radius) const {
        return GetRing(center, radius, GridComparer_All());
    }
    template<class Comparer> 
    EntContainer GetSolidRing(const CoordI& center, const int radius, Comparer comparer) const {
        EntContainer container, tmp;
        Getter_ProcessCell(container, center, comparer);
        for (int i = 1; i <= radius; ++i) {
            tmp = GetRing(center, i, comparer);
            container.splice(container.end(), tmp);
        }
        return container;
    }
    EntContainer GetSolidRing(const CoordI& center, const int radius) const {
        return GetSolidRing(center, radius, GridComparer_All());
    }
    template<class Comparer> 
    EntContainer GetSquare(const CoordI& center, const int radius, Comparer comparer) const {
        EntContainer container;
        int x, y = center.y - radius;
        for (x = center.x - radius; x <= center.x + radius; ++x) {
            Getter_ProcessCell(container, CoordI(x, y), comparer);
        }
        for (y = center.y - radius + 1; y <= center.y + radius -1; ++y) {
            Getter_ProcessCell(container, CoordI(center.x - radius, y), comparer);
            Getter_ProcessCell(container, CoordI(center.x + radius, y), comparer);
        }
        y = center.y + radius;
        for (x = center.x - radius; x <= center.x + radius; ++x) {
            Getter_ProcessCell(container, CoordI(x, y), comparer);
        }
        return container;
    }
    EntContainer GetSquare(const CoordI& center, const int radius) const {
        return GetSquare(center, radius, GridComparer_All());
    }
    template<class Comparer> 
    EntContainer GetSolidSquare(const CoordI& center, const int radius, Comparer comparer) const {
        EntContainer container, tmp;
        Getter_ProcessCell(container, center, comparer);
        for (int i = 1; i <= radius; ++i) {
            tmp = GetSquare(center, i, comparer);
            container.splice(container.end(), tmp);
        }
        return container;
    }
    EntContainer GetSolidSquare(const CoordI& center, const int radius) const {
        return GetSolidSquare(center, radius, GridComparer_All());
    }
    
    template<class Comparer> Entity* FindNearest(const CoordI& center, Comparer comparer, int max_radius = 0) const {
        if (IsInside(center)) {
            if (max_radius == 0) {
                max_radius = max(max(abs(size.x - center.x), center.x - 1), 
                                 max(abs(size.y - center.y), center.y - 1));
            }
            EntContainer container = GetSolidSquare(center, i, comparer);
            if (!container.empty()){
                container.sort(EntityCoorSorter(center, *this));
                return *(container.begin());
            }
        }
        return nullptr;
    }
};
class EntityCoorSorter {
    CoordI center; 
    const Grid& grid;
public:
    EntityCoorSorter(const CoordI& center, const Grid& grid): center(center), grid(grid) {}
    bool operator()(const Entity* ent1, const Entity* ent2) {
        return grid.Dist(ent1->Coor(), center, true) < grid.Dist(ent2->Coor(), center, true);
    }
};
class EntityZOrderSorter {
    int GetEntZOrderWeight(const Entity* ent) const {
        int weight = 0;
        if (ent->Flag(ENT_IS_SOLID)) { weight += 100; }
        if (ent->Flag(ENT_IS_UNIT)) { weight += 1000; }
        return weight;
    }
public:
    bool operator()(const Entity* ent1, const Entity* ent2) {
        return (GetEntZOrderWeight(ent1) < GetEntZOrderWeight(ent2));
    }
};

class GridComparer_Obelisks
{
public:
    bool operator()(const Entity* ent) { return !ent->IsDead() && ent->Flag(ENT_IS_OBELISK); }
};
class GridComparer_Visible
{
public:
    bool operator()(const Entity* ent) { return !ent->IsDead() && !ent->Flag(ENT_IS_INVISIBLE); }
};
class GridComparer_Units
{
public:
    bool operator()(const Entity* ent) { return !ent->IsDead() && ent->Flag(ENT_IS_UNIT); }
};
class GridComparer_Mortal
{
public:
    bool operator()(const Entity* ent) {
        return !ent->IsDead() && !ent->Flag(ENT_IS_IMMORTAL) && !ent->Flag(ENT_IS_UNTARGETABLE);
    }
};
class GridComparer_MortalVisibleFrom
{
    CoordI center;
    const Grid& grid;
public:
    GridComparer_MortalVisibleFrom(const CoordI& center, const Grid& grid): center(center), grid(grid) {}
    bool operator()(const Entity* ent) {
        return !ent->IsDead() && !ent->Flag(ENT_IS_IMMORTAL) && !ent->Flag(ENT_IS_UNTARGETABLE) &&
            grid.IsSeenFrom(center, ent->Coor());
    }
}; 
class GridComparer_VisibleFrom
{
    CoordI center;
    const Grid& grid;
public:
    GridComparer_VisibleFrom(const CoordI& center, const Grid& grid): center(center), grid(grid) {}
    bool operator()(const Entity* ent) {
        return !ent->IsDead() && !ent->Flag(ENT_IS_UNTARGETABLE) &&
            grid.IsSeenFrom(center, ent->Coor());
    }
};
class GridComparer_Movable
{
public:
    bool operator()(const Entity* ent) {
        return !ent->IsDead() && !ent->Flag(ENT_IS_IMMOBILE) && !ent->Flag(ENT_IS_UNTARGETABLE);
    }
};
class GridComparer_Homings
{
public:
    bool operator()(const Entity* ent) {
        return !ent->IsDead() && ent->Flag(ENT_IS_HOMING_PROJECTILE) && !ent->Flag(ENT_IS_UNTARGETABLE);
    }
};
class GridComparer_EnvironmentAffecting
{
public:
    bool operator()(const Entity* ent) {
        return  !ent->IsDead() && !ent->Flag(ENT_IS_UNTARGETABLE) && 
            (ent->Flag(ENT_IS_PROJECTILE) || ent->Flag(ENT_IS_OBELISK) ||
            ent->Flag(ENT_IS_AOE_DAMAGING) || ent->Flag(ENT_IS_TRIGGER) ||
            (ent->Flag(ENT_IS_UNIT) && dynamic_cast<const Unit*>(ent)->IsEnchanted(ENCHANT_SHOCK)));
    }
};
class GridComparer_Dead
{
public:
    bool operator()(const Entity* ent) {
        return ent->IsDead();
    }
};