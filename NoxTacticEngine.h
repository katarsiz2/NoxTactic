#pragma once
#include "Engine.h"
#include "Geometry.h"
#include "Constants.h"
#include <vector>
#include <list>
#include <map>
using std::vector; 
using std::string;
using std::pair;
using std::list;
using std::map;

class Input
{
    struct ButtonField
    {
        CoordL coor, size;
        Buttons id;
        int index;
        ButtonField(CoordL coor, CoordL size, Buttons id, int index = 0): coor(coor), size(size), id(id), index(index) {}
    };
    Keys keybinds[KEYS];
    vector<ButtonField> buttons;
    list<Buttons> active_buttons;
    map<Buttons, list<int>> id_to_handler;
    //field info
    CoordL cellsize, fieldstart;
    CoordI windowsize;
    //HACK: windows input messages are 782,560 in the corner of window 800,600. multiplying by this constant fixes that
    static const CoordD GlitchyInputRepresentationScale;
public:
    Input(): buttons(), cellsize(), fieldstart(), windowsize() {}
    void setField(CoordL cellsize, CoordL fieldstart, CoordI windowsize);
    void setDefaultKeyBindings();
    void setKeyBindings(Keys keybindings[]);
    //setting index != 0 means creating array
    void addButton(CoordL coor, CoordL size, Buttons id, int index = 0);
    //windows input messages are 782,560 in the corner of window 800,600. applying this method fixes that
    void activateButton(Buttons id);
    void deactivateButton(Buttons id);

    CoordL deglitchCoor(const CoordL& coor);
    //finds a button among those added from outside, withing frame of which the coor lies
    pair<Buttons, int> findButton(const CoordL& coor);
    //decodes key code
    Keys findKey(unsigned virtual_code);
    //finds a cell, within which the coor lies
    CoordI findCell(CoordL coor);
};

const CoordI DefaultLabelSize = CoordI(1024, 50);
const Color DefaultLabelColor = ColorWhite;
typedef int PictureHandler;
typedef int LabelHandler;

class Graphics: public GraphicCore
{
public:
    struct Rectangle
    {
        CoordI coor;
        CoordI size;
        Rectangle(const CoordI& coor, const CoordI& size = CoordI()):
            coor(coor), size(size) {}
        Rectangle(): coor(0, 0), size(0, 0) {}
    };
    enum PICTURE_STATE
    {
        PICTURESTATE_BASE,
        PICTURESTATE_ACTIVE,
        PICTURESTATE_DISABLED
    };
    struct PictureBase: Rectangle
    {
        bool is_invisible;
        PICTURE_STATE state;
        PictureBase(const Rectangle& rect): Rectangle(rect), is_invisible(false) {}
        PictureBase(): Rectangle(), is_invisible(false) {}
        void setState(PICTURE_STATE newstate) { state = newstate; }
        virtual void setTexture(TextureHandler text) {}
        virtual void setActiveTexture(TextureHandler text) {}
        virtual void setDisabledTexture(TextureHandler text) {}
        virtual TextureHandler getTexture() const { return EMPTY_TEXTURE; }
    };
    enum ELEMENT_TYPE
    {
        ELEMENT_PICTURE,
        ELEMENT_SPELLICON,
        ELEMENT_BUTTON
    };
private:
    struct Label: Rectangle
    {
        std::string text;
        bool is_invisible;
        Color color;
        Label(const string& text, const CoordI& coor, const Color& color = DefaultLabelColor):
            Rectangle(coor, DefaultLabelSize), text(text), color(color), is_invisible(false) {}
        Label(const string& text, const Rectangle& rect, const Color& color = DefaultLabelColor):
            Rectangle(rect), text(text), color(color), is_invisible(false) {}
        Label(const Rectangle& rect, const Color& color = DefaultLabelColor):
            Rectangle(rect), text(), color(color), is_invisible(false) {}
        Label(): Rectangle(CoordI(), DefaultLabelSize), text(), color(DefaultLabelColor), is_invisible(false) {}
    };

    struct SpellIcon: PictureBase
    {
        TextureHandler base, active;
        virtual TextureHandler getTexture() const {
            switch (state) {
            case PICTURESTATE_BASE:
                return base;
            case PICTURESTATE_ACTIVE:
                return active;
            default:
                return EMPTY_TEXTURE;
            };
        }
        virtual void setTexture(TextureHandler text) { base = text; }
        virtual void setActiveTexture(TextureHandler text) { active = text; }
    };
    struct Button: PictureBase
    {
        TextureHandler base, disabled, active;
        virtual TextureHandler getTexture() const {
            switch (state) {
            case PICTURESTATE_BASE:
                return base;
            case PICTURESTATE_ACTIVE:
                return active;
            case PICTURESTATE_DISABLED:
                return disabled;
            default:
                return EMPTY_TEXTURE;
            };
        }
        virtual void setTexture(TextureHandler text) { base = text; }
        virtual void setActiveTexture(TextureHandler text) { active = text; }
        virtual void setDisabledTexture(TextureHandler text) { disabled = text; }
    };
    struct Picture: PictureBase
    {
        TextureHandler base;
        virtual TextureHandler getTexture() const {
            return base;
        }
        virtual void setTexture(TextureHandler text) { base = text; }
    };
    CoordD getTextureSize(TextureHandler texture);
private:
    struct ShiftedTextureHandler
    {
        TextureHandler texture;
        CoordD offset;
        ShiftedTextureHandler(TextureHandler txt, CoordD offset): texture(txt), offset(offset) {}
    };
    vector<vector<vector<ShiftedTextureHandler>>> spritemap;
    CoordI size;
    vector<PictureBase*> pictures;
    vector<Label*> labels;
public:
    Graphics(EngineCore* game):
        GraphicCore(static_cast<EngineCore*>(game)), spritemap(), size(), pictures() {}
    void setMapSize(const CoordI& size);
    void setMapSize(int x, int y);
    void pushTexture(const CoordI& coor, TextureHandler texture, const CoordD& offset = CoordD());
    void clearCell(const CoordI& coor);
    CoordI getSize() const { return size; }

    PictureHandler addPicture(const CoordI& position, const CoordI& size, ELEMENT_TYPE type);
    PictureHandler addPicture(const Rectangle& rect, ELEMENT_TYPE type);
    LabelHandler addLabel(const Rectangle& rect, const Color& color = DefaultLabelColor);
    LabelHandler addLabel(const CoordI& position, const CoordI& size, const Color& color = DefaultLabelColor);
    void setPictureState(PictureHandler pic, PICTURE_STATE state = PICTURESTATE_BASE);
    void setPictureTexture(PictureHandler pic, TextureHandler text = EMPTY_TEXTURE);
    void setPictureActiveTexture(PictureHandler pic, TextureHandler text = EMPTY_TEXTURE);
    void setDisabledTexture(PictureHandler pic, TextureHandler text = EMPTY_TEXTURE);
    void setPicturePosition(PictureHandler pic, const CoordI& position);
    void setPictureSize(PictureHandler pic, const CoordI& size);
    void setLabelText(LabelHandler label, const string& text);
    void setLabelText(LabelHandler label);
    void setLabelColor(LabelHandler label, const Color& color = DefaultLabelColor);
    void setPictureVisibility(PictureHandler pic, bool visibility);
    void setLabelVisibility(LabelHandler pic, bool visibility);
    void movePicture(PictureHandler pic, CoordI newposition);

    void paintGrid();
    void printLabel(const Label* label);
    void paintPicture(const PictureBase* pic);
    void paint(const CoordI& coor1, const CoordI& coor2);

    TextureHandler addAnimatedTexture(const string& dir, const string& extension, const int frames, long long milliseconds_per_frame, const Color& transparent = ColorNull);

    virtual void shutdown();
};