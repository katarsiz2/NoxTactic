#include "NoxTacticEngine.h"
#include <string>
std::string inttostring(int a) {
    char buf[100];
    _itoa_s(a, buf, 10);
    std::string tmp(buf);
    return tmp;
}
const CoordD Input::GlitchyInputRepresentationScale = CoordD(1.0204081632653061224489795918367, 1.0714285714285714285714285714286);
#pragma warning (disable:4244)
template <class T> D3DXVECTOR2 tod3d(const Coord<T>& a) {
    return D3DXVECTOR2(a.x, a.y);
}

CoordD Graphics::getTextureSize(TextureHandler texture) {
    return CoordD(getTextureWidth(texture), getTextureHeight(texture));
}

void Graphics::setMapSize(const CoordI& size) {
    setMapSize(size.x, size.y);
}
void Graphics::setMapSize(int x, int y) {
    spritemap.resize(x);
    for (auto it = spritemap.begin(); it != spritemap.end(); ++it) {
        it->resize(y);
    }
    size = CoordI(x, y);
}
void Graphics::pushTexture(const CoordI& coor, TextureHandler texture, const CoordD& offset) {
    spritemap[coor.x-1][coor.y-1].push_back(ShiftedTextureHandler(texture, offset));
}
void Graphics::clearCell(const CoordI& coor) {
    spritemap[coor.x-1][coor.y-1].clear();
}

PictureHandler Graphics::addPicture(const CoordI& position, const CoordI& size, ELEMENT_TYPE type) {
    PictureBase* element = nullptr;
    switch (type) {
    case ELEMENT_PICTURE:
        element = new Picture();
        break;
    case ELEMENT_SPELLICON:
        element = new SpellIcon();
        break;
    case ELEMENT_BUTTON:
        element = new Button();
        break;
    };
    element->coor = position;
    element->size = size;
    pictures.push_back(element);
    element->setTexture(EMPTY_TEXTURE);
    element->setActiveTexture(EMPTY_TEXTURE);
    element->setDisabledTexture(EMPTY_TEXTURE);
    element->setState(PICTURESTATE_BASE);
    return pictures.size();
}
PictureHandler Graphics::addPicture(const Rectangle& rect, ELEMENT_TYPE type) {
    return addPicture(rect.coor, rect.size, type);
}
LabelHandler Graphics::addLabel(const CoordI& position, const CoordI& size, const Color& color) {
    return addLabel(Rectangle(position, size), color);
}
LabelHandler Graphics::addLabel(const Rectangle& rect, const Color& color) {
    Label *label = new Label(rect, color);
    labels.push_back(label);
    return labels.size();
}
void Graphics::setPictureState(PictureHandler pic, PICTURE_STATE state) {
    pictures[pic-1]->state = state;
}
void Graphics::setPictureTexture(PictureHandler pic, TextureHandler text) {
    pictures[pic-1]->setTexture(text);
}
void Graphics::setPictureActiveTexture(PictureHandler pic, TextureHandler text) {
    pictures[pic-1]->setActiveTexture(text);
}
void Graphics::setDisabledTexture(PictureHandler pic, TextureHandler text) {
    pictures[pic-1]->setDisabledTexture(text);
}
void Graphics::setPicturePosition(PictureHandler pic, const CoordI& position) {
    pictures[pic-1]->coor = position;
}
void Graphics::setPictureSize(PictureHandler pic, const CoordI& size) {
    pictures[pic-1]->size = size;
}
void Graphics::setLabelText(LabelHandler label, const string& text) {
    labels[label-1]->text = text;
}
void Graphics::setLabelText(LabelHandler label) {
    setLabelText(label, "");
}
void Graphics::setLabelColor(LabelHandler label, const Color& color) {
    labels[label-1]->color = color;
}
void Graphics::setPictureVisibility(PictureHandler pic, bool visibility) {
    pictures[pic-1]->is_invisible = !visibility;
}
void Graphics::setLabelVisibility(LabelHandler label, bool visibility) {
    labels[label-1]->is_invisible = !visibility;
}
void Graphics::movePicture(PictureHandler pic, CoordI newposition) {
    pictures[pic-1]->coor = newposition;
}


void Graphics::printLabel(const Label* label) {
    if (label->is_invisible) { return; }
    CoordI coor2 = label->coor + label->size;
    this->printString(label->text, label->color, label->coor.x, coor2.x, label->coor.y, coor2.y);
}
void Graphics::paintPicture(const PictureBase* pic) {
    if (pic->is_invisible) { return; }
    TextureHandler texture = pic->getTexture();
    if (texture == 0) { return; }
    setSpriteScale(tod3d((CoordD)pic->size / getTextureSize(texture)));
    paintSprite(texture, tod3d(pic->coor));
}
void Graphics::paint(const CoordI& coor1, const CoordI& coor2) {
    clearDisplay();
    beginSprite();
    setSpriteScale(tod3d(picscale));
    //TODO: print grid

    //HACK: painting walls and tiles via index 0 and 1; TODO: fix such relativity with something like 'texture->paintfirst'
    for (RectangleIterator it(coor1, minCoord(size, coor2)); it.isInside(); ++it) {
        for (unsigned int i = 0; i < 2; ++i) {
            auto tmp = spritemap[it.getCoor().x-1][it.getCoor().y-1][i];
            CoordI tmpcoor = (tmp.offset + it.getCoor() - coor1) * picsize;
            paintSprite(tmp.texture, tod3d(tmpcoor));
        }
    }
    //finishing painting other shit
    for (RectangleIterator it(coor1, minCoord(size, coor2)); it.isInside(); ++it) {
        for (unsigned int i = 2; i < spritemap[it.getCoor().x-1][it.getCoor().y-1].size(); ++i) {
            auto tmp = spritemap[it.getCoor().x-1][it.getCoor().y-1][i];
            CoordI tmpcoor = (tmp.offset + it.getCoor() - coor1) * picsize;
            paintSprite(tmp.texture, tod3d(tmpcoor));
        }
    }
    for (auto it = pictures.begin(); it != pictures.end(); ++it) {
        paintPicture(*it);
    }
    setSpriteScale();
    for (auto it = labels.begin(); it != labels.end(); ++it) {
        printLabel(*it);
    }
    endSprite();
}

void Graphics::shutdown() {
    for (auto it = pictures.begin(); it != pictures.end(); ++it) {
        delete *it;
    }
    for (auto it = labels.begin(); it != labels.end(); ++it) {
        delete *it;
    }
    GraphicCore::shutdown();
}

TextureHandler Graphics::addAnimatedTexture(const string& dir, const string& extension, const int frames, long long milliseconds_per_frame, const Color& transparent) {
    vector<TextureHandler> tmptextures;
    for (int i = 0; i < frames; ++i) {
        tmptextures.push_back(addTexture(dir + inttostring(i+1) + extension, transparent));
    }
    return GraphicCore::addAnimatedTexture(tmptextures, milliseconds_per_frame);
}

//==============================================
void Input::setDefaultKeyBindings() {
    for (int i = 0; i < KEYS; ++i) {
        this->keybinds[i] = NO_KEY;
    }
    keybinds['A'] = KEY_SPELL1;
    keybinds['S'] = KEY_SPELL2;
    keybinds['D'] = KEY_SPELL3;
    keybinds['F'] = KEY_SPELL4;
    keybinds['G'] = KEY_SPELL5;
    keybinds['Q'] = KEY_PREV_SPELLSET;
    keybinds['E'] = KEY_NEXT_SPELLSET;
    keybinds[VK_LEFT] = KEY_ARROW_LEFT;
    keybinds[VK_RIGHT] = KEY_ARROW_RIGHT;
    keybinds[VK_UP] = KEY_ARROW_UP;
    keybinds[VK_DOWN] = KEY_ARROW_DOWN;
    keybinds[VK_RETURN] = KEY_ACCEPT;
    keybinds[VK_ESCAPE] = KEY_ESC;
}
void Input::setKeyBindings(Keys keybindings[]) {
    for (int i = 0; i < KEYS; ++i) {
        this->keybinds[i] = keybindings[i];
    }
}
void Input::setField(CoordL cellsize, CoordL fieldstart, CoordI windowsize) {
    this->cellsize = cellsize;
    this->fieldstart = fieldstart;
    this->windowsize = windowsize;
}
void Input::addButton(CoordL coor, CoordL size, Buttons id, int index)
{
    buttons.push_back(ButtonField(coor, size, id, index));
    activateButton(id);
    id_to_handler[id].push_back(buttons.size()-1);  //TODO: Check if map works like that
}
void Input::activateButton(Buttons id) {

    for (auto it = active_buttons.begin(); it != active_buttons.end(); ++it) {
        if (*it == id) {
            return; //it is already activated;
        }
    }
    active_buttons.push_back(id);
}
void Input::deactivateButton(Buttons id) {
    for (auto it = active_buttons.begin(); it != active_buttons.end(); ++it) {
        if (*it == id) {
            active_buttons.erase(it);
            return;
        }
    }
}
pair<Buttons, int> Input::findButton( const CoordL& coor )
{
    for (auto it = active_buttons.begin(); it != active_buttons.end(); ++it) {
        for (auto it2 = id_to_handler[*it].begin(); it2 != id_to_handler[*it].end(); ++it2) {
            if (coor.isInside(buttons[*it2].coor, buttons[*it2].coor + buttons[*it2].size)) {
                return make_pair(buttons[*it2].id, buttons[*it2].index);
            }
        }
        
    }
    return make_pair(NO_BUTTON, 0);
}
Keys Input::findKey(unsigned virtual_code) {
    if (virtual_code < KEYS) {
        return keybinds[virtual_code];
    } else {
        return NO_KEY;
    }
}
CoordI Input::findCell(CoordL coor) {
    coor -= fieldstart;
    if (coor.isInside(CoordL(0, 0), (CoordL)(cellsize)*windowsize - Coord1)) {
        coor /= cellsize;
        return coor + Coord1;
    } else {
        return Coord0;
    }
}

CoordL Input::deglitchCoor(const CoordL& coor) {
    return CoordD(coor)*GlitchyInputRepresentationScale;
}
