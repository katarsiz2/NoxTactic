#include "Grid.h"
#include "Entity.h"
#include <assert.h>
#include <algorithm>
Grid::Grid(const CoordI& size): size(size) {
    cells.resize(size.x);
    for(auto it = cells.begin(); it != cells.end(); ++it){
        (*it).resize(size.y);
    }
}

Entity* Grid::operator()(const int x, const int y) const {
    for (auto it = cells[x-1][y-1].ents.begin(); it != cells[x-1][y-1].ents.end(); ++it) {
        if (!(*it)->Flag(ENT_IS_UNTARGETABLE) && !(*it)->Flag(ENT_IS_IMMORTAL) && 
            !(*it)->IsDead() && !(*it)->Flag(ENT_IS_INVISIBLE)) {
            return *it;
        }
    }
    //nothing was found
    return nullptr;
}
Grid::Cell& Grid::_GetCell(const CoordI& coor) { return cells[coor.x-1][coor.y-1]; }
const Grid::Cell& Grid::_GetCell(const CoordI& coor) const { return cells[coor.x-1][coor.y-1]; }

list<Entity*>::iterator Grid::FindMe(Entity* value) {
    return find(_GetCell(value->Coor()).ents.begin(),
                _GetCell(value->Coor()).ents.end(), value);
}
void Grid::Move(Entity* ent, const CoordI& dest, const Direction dir){
    if (ent->Coor() == dest) { return; }
    auto it = FindMe(ent);
    _GetCell(ent->Coor()).ents.erase(it);
    _GetCell(dest).ents.push_back(ent);
    ent->Move(dest, dir);
}
void Grid::Swap(Entity* ent1, Entity* ent2){
    auto it1 = FindMe(ent1);
    auto it2 = FindMe(ent2);
    *it1 = ent2; *it2 = ent1;
    CoordI coor1 = ent1->Coor(), coor2 = ent2->Coor();
    ent1->Move(coor2);
    ent2->Move(coor1);
}
void Grid::Add(Entity* ent){
    _GetCell(ent->Coor()).ents.push_back(ent);
}
 
void Grid::SetTile(const CoordI& coor, const Tile* tile) {
    _GetCell(coor).tile = tile;
}
void Grid::SetWall(const CoordI& coor, const Wall* wall) {
    _GetCell(coor).is_wall = true;
    _GetCell(coor).wall = wall;
}
void Grid::RemoveWall(const CoordI& coor) {
    _GetCell(coor).is_wall = false;
}

bool Grid::IsWall(const CoordI& coor) const {
    return _GetCell(coor).is_wall;
}
bool Grid::IsWindow(const CoordI& coor) const {
    if (IsWall(coor)) {
        return _GetCell(coor).wall->HasWindow();
    }
    return false;
}
bool Grid::IsWindowlessWall(const CoordI& coor) const {
    if (IsWall(coor)) {
        return !_GetCell(coor).wall->HasWindow();
    } else {
        return false;
    }
}

TextureHandler Grid::GetWallTexture(const CoordI& coor) const {
    if (IsWall(coor)) {
        return _GetCell(coor).wall->MyTexture();
    } else {
        return EMPTY_TEXTURE;
    }
}
TextureHandler Grid::GetTileTexture(const CoordI& coor) const {
    return _GetCell(coor).tile->MyTexture();
}

bool Grid::IsSeenFrom(const CoordI& start, const CoordI& dest) const {
    Path trace = TraceLine(CoordI(start), CoordI(dest));
    for (auto it = trace.begin(); it != trace.end(); ++it) {
        if (BlocksSight(*it)) {
            return false;
        }
    }
    return true;
}
bool Grid::BlocksMoving(const Entity* ent, const CoordI& dest, bool UnitJumps) const {
    if (ent->IsDead()) { return false; }
    return BlocksMoving(&ent->GetPrototype(), dest, UnitJumps);
}
bool Grid::BlocksMoving(const DefaultEntity* ent, const CoordI& dest, bool UnitJumps) const {
    const DefaultProjectile* proj = nullptr;
    if (ent->Flag(ENT_IS_PROJECTILE)) {
        proj = dynamic_cast<const DefaultProjectile*>(ent);
    }
    if (!proj || proj->CollisionFlag(COLLIDESWITH_WALLS)) {
        if (ent->Flag(ENT_IS_FLYING)) { //it flies
            if (IsWindowlessWall(dest)) {
                return true;
            }
        } else { //it doesnt fly
            if (IsWall(dest)) { //there is wall
                return true;
            }
        }
    }
    auto ents = GetCell(dest, GridComparer_All());
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        if (ent->BlocksMoving(&(*it)->GetPrototype())) {
            return true;
        }
    }
    return false;
}

bool Grid::BlocksSight(const CoordI& dest) const {
    if (IsWindowlessWall(dest)) {
        return true;
    }
    const EntContainer& ents = _GetCell(dest).ents;
    for (auto it = ents.begin(); it != ents.end(); ++it) {
        if ((*it)->Flag(ENT_IS_OPAQUE) && !(*it)->IsDead()) {
            return true;
        }
    };
    return false;
}

bool Grid::IsInside(const CoordI& v) const {
    return v.isInside(Coord1, size);
}
int Grid::Dist(const CoordI& start, const CoordI& dest, bool diagonal) const {
    return (start-dest).intlength(diagonal);
}

void Bresenham(Path& res, CoordI start, CoordI dest) {
    const int deltaX = abs(dest.x - start.x);
    const int deltaY = abs(dest.y - start.y);
    const int signX = start.x < dest.x ? 1 : -1;
    const int signY = start.y < dest.y ? 1 : -1;
    //
    int error = deltaX - deltaY;
    while(start.x != dest.x || start.y != dest.y) {
        res.push_back(start);
        const int error2 = error * 2;
        //
        if(error2 > -deltaY) {
            error -= deltaY;
            start.x += signX;
        }
        if(error2 < deltaX) {
            error += deltaX;
            start.y += signY;
        }
    }
    res.push_back(dest);
}
Path Grid::TraceRay(CoordI start, CoordI dest) const {
    Path res;
    if (start == dest) {
        res.push_back(start);
        return res;
    }
    
    CoordI diff = dest - start;
    while (IsInside(dest)) {
        dest += diff;
    }
    Bresenham(res, start, dest);
    for (auto it = res.begin(); it != res.end(); ++it) {
        if (!IsInside(*it)) {
            res.erase(it, res.end());
            break;
        }
    }
    return res;
}
Path Grid::TraceLine(CoordI start, CoordI dest) const {
    Path res;
    if (start == dest) {
        res.push_back(start);
        return res;
    }
    
    Bresenham(res, start, dest);
    return res;
}
Path Grid::FindPath(CoordI start, CoordI dest, const Entity* ent) {

    struct Item
    {
        int x;
        int y;
        int wave;
        Item(int x_, int y_, int wave_): x(x_), y(y_), wave(wave_) {}
    };

    static const int dx[8] = {1, -1, 0, 0, 1, -1, 1, -1};
    static const int dy[8] = {0, 0, 1, -1, 1, 1, -1, -1};

    // Первый этап - поиск собственно пути
    std::list<Item> queue;
    bool found = false;
    queue.push_front(Item(start.x, start.y, nextWave));
    cells[start.x-1][start.y-1].wave = nextWave;
    int wave1 = nextWave;
    int wave2 = nextWave + 1;
    int wave3 = nextWave + 2;
    int wave4 = nextWave + 3;
    nextWave += 4;

    while (!queue.empty()) {
        Item pt(queue.front());
        queue.pop_front();

        int waveNow = pt.wave;
        int waveNext = (waveNow==wave4) ? wave1 : (waveNow + 1);
        if (pt.x == dest.x && pt.y == dest.y) { // Пришли
            found = true;
            break;
        }

        for (int i = 0; i < 8; ++i) {
            int x = pt.x + dx[i];
            int y = pt.y + dy[i];
            CoordI pt2 = CoordI(x, y);

            if (!IsInside(pt2))
                continue;
            if (!BlocksMoving(ent, pt2, false)) {
                if ((cells[x-1][y-1].wave != wave1) &&
                    (cells[x-1][y-1].wave != wave2) &&
                    (cells[x-1][y-1].wave != wave3) &&
                    (cells[x-1][y-1].wave != wave4)
                    ) {
                    //TODO: если мы хотим A-Star, в Item надо добавить оценку, здесь вычислить новую оценку
                    // и тут поддерживать список отсортированым по оценки стоимости пути в сторону убывания
                    queue.push_back(Item(x, y, waveNext));
                    cells[x-1][y-1].wave = waveNext;
                }
            }
        }
    }
    // Второй этап - построение пути
    if (!found)
        return Path();
  std::vector<std::string> dbgInfo;

    for (int i = 0; i< size.y; ++i) {
        std::string dbgStr;
        for (int j = 0; j<size.x; ++j) {
            dbgStr += '0' + cells[j][i].wave;
        }
        dbgInfo.push_back(dbgStr);
    }
    Path ret;
    int px = dest.x;
    int py = dest.y;
    int wave0 = wave3 - 2;
    int waveNow = cells[px-1][py-1].wave;

    while (true) {
        ret.push_back(CoordI(px, py));
        if (px == start.x && py == start.y)
            break;
        int wavePrev = (waveNow == wave1) ? wave4 : (waveNow - 1);
        bool moving = false; //по идее можно убрать, только для поиска ошибок
        for (int i = 0; i < 8; ++i) {
            int x = px + dx[i];
            int y = py + dy[i];
            if (!IsInside(CoordI(x, y)))
                continue;
            if (cells[x-1][y-1].wave == wavePrev) {
                px = x; py = y;
                moving = true;
                break;
            }
        }
        assert(moving);
        waveNow = wavePrev;
    }
    
    return ret;
}

CoordD Grid::NearestCellVerge(const CoordD& minimizer, const CoordI& coor) const {
    vector<CoordD> verges;
    verges.push_back(coor);
    verges.push_back(coor + CoordI(1, 0));
    verges.push_back(coor+CoordI(0, 1));
    verges.push_back(coor + Coord1);
    double mindist = 0; int minindex = 0;
    for (int i = 0; i < 4; ++i) {
        double dist = (minimizer-verges[i]).length();
        if (dist < mindist) {
            mindist = dist;
            minindex = i;
        }
    }
    return verges[minindex];
}

ErrorBase* Grid::CheckConsistency() const
{
    if (size.x <= 0 || size.y <= 0) {
        return new Errors::GridWrongSize();
    }
    for (RectangleIterator it(Coord1, size); it.isInside(); ++it) {
        const Cell cell = _GetCell(it.getCoor());
        if (cell.is_wall && cell.wall == nullptr) {
            return new Errors::GridNullWall(it.getCoor());
        }
        if (cell.tile == nullptr) {
            return new Errors::GridNullTile(it.getCoor());
        }
        for (auto it2 = cell.ents.begin(); it2 != cell.ents.end(); ++it2) {
            if ((*it2)->Coor() != it.getCoor()) {
                return new Errors::GridEntityMismatchingCoor(it.getCoor(), *it2);
            }
        }
    }
    return nullptr;
}

void Loader::loadTiles(ContainerDefs::TileContainer& container)
{
    container.resize(Counters::tiles);
    container[NO_TILE] = new Tile();
    container[TILE_GRASS] = new Tile(STR_TILE_GRASS);
    container[TILE_LAVA] = new Tile(STR_TILE_LAVA);
    container[TILE_RUG] = new Tile(STR_TILE_RUG);
}
void Loader::loadWalls(ContainerDefs::WallContainer& container)
{
    container.resize(Counters::walls);
    container[NO_WALL] = new Wall();
    container[WALL_BRICK] = new Wall(STR_WALL_BRICK, false, false);
    container[WALL_COBBLESTONE] = new Wall(STR_WALL_COBBLESTONE, false, false);
    container[WALL_UNDERWORLD] = new Wall(STR_WALL_UNDERWORLD, false, false);
    container[WALL_MAGIC] = new Wall(STR_WALL_MAGIC, false, false);

}
