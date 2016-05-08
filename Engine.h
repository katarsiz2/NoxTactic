#pragma once
#define DIRECTINPUT_VERSION 0x800 
#pragma comment (lib, "d3d9")
#pragma comment (lib, "d3dx9")
#pragma comment (lib, "dxguid")
#pragma comment (lib, "dinput8")
#pragma comment (lib, "dsound")
#include <Windows.h>
#include <WindowsX.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <ctime>
#include <ratio>
#include <chrono>
using std::string;
using std::vector;
using namespace std::chrono;

#undef PlaySound
#define KEYS 256
class GraphicCore;
class InputCore;
class SoundCore;
class NetworkCore;
class EngineCore
{
    HINSTANCE inst;
    HWND wnd;
protected:
    std::string classname;
    std::string caption;
    long pos_x, pos_y, width, height;
    std::string applicationPath;
    std::string applicationExeName;

    GraphicCore *graphic;
    InputCore *input;
    SoundCore *sound;
public:
    EngineCore(HINSTANCE& hInstance, const string& classname, const string& caption, const string& cursor, const string& icon,
        long window_width, long window_height, long startposx, long startposy);
    HWND getWnd() const;
    HINSTANCE getInstance() const;

    void moveWindow(long newx, long newy);
    void resizeWindow(long newwidth, long newheight);
    void setMouseVisibility(bool val);
    long getWindowWidth() const { return width; }
    long getWindowHeight() const { return height; }
    void setGraphicCore(GraphicCore* gr) { graphic = gr; }
    void setInputCore(InputCore* in) { input = in; }
    void setSoundCore(SoundCore* snd) { sound = snd; }
    GraphicCore* Graphics_core() const { return graphic; }
    InputCore* Input_core() const { return input; }
    const std::string& getAppPath() const { return applicationPath; }

    virtual LRESULT WINAPI MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    virtual void init(){}
    virtual void shutdown(){}
    virtual void frame(){}
    virtual ~EngineCore();
};
extern EngineCore* core;

struct Color
{
    int R, G, B, A;
    Color(int R, int G, int B, int A = 255): R(R), G(G), B(B), A(A) {}
    Color(): R(0), G(0), B(0), A(255) {}
    bool IsNull() const { return R == -1; }
    operator D3DCOLOR() const{ return D3DCOLOR_ARGB(A,R,G,B); }
};
const Color ColorWhite = Color(255, 255, 255, 255);
const Color ColorBlack = Color(0, 0, 0, 255);
const Color ColorNull = Color(-1, -1, -1, -1);
class GraphicCore
{
public:
    typedef int TextureHandler;
    static const TextureHandler EMPTY_TEXTURE = 0;
private:
    class TextureBase
    {
    protected:
        long width, height;
    public:
        virtual IDirect3DTexture9* MyD3DTexture() const = 0;
        TextureBase(long width, long height): width(width), height(height) {}
        TextureBase(): width(0), height(0) {}
        long getWidth() const { return width; }
        long getHeight() const { return height; }
        virtual ~TextureBase() {}
    };
    class Texture: public TextureBase
    {
        IDirect3DTexture9* texture;
    public:
        Texture(IDirect3DDevice9 *device, const std::string& Path, Color transparent = ColorNull, double activepartx = 1.0, double activeparty = 1.0);
        Texture(const Texture& another);
        virtual IDirect3DTexture9* MyD3DTexture() const { return texture; }
        virtual ~Texture();
    };
    class AnimatedTexture: public TextureBase
    {
        vector<Texture*> frames;
        long long time_per_frame;
        steady_clock::time_point start;
        int frames_number;
    public:
        AnimatedTexture(const vector<Texture*>& textures, long long milliseconds_per_frame):
            frames(textures), time_per_frame(milliseconds_per_frame), frames_number(textures.size()),
            start(steady_clock::now()), TextureBase(textures[0]->getWidth(), textures[0]->getHeight()) {}
        virtual IDirect3DTexture9* MyD3DTexture() const;
        virtual ~AnimatedTexture() {}
    };

    typedef std::vector<TextureBase*> TextureContainer;
    HWND wnd;
    IDirect3D9 *d3d;
    IDirect3DDevice9 *device;
    ID3DXSprite *sprite;
    ID3DXFont *font;
    HCURSOR cursor;
    bool iswindowed, iszbuffer, ishal;
    long width, height;
    char bpp; //bits per pixel
    D3DXVECTOR2 currentSpriteScale;

    std::string TexturesPath;
    TextureContainer textures;
protected:
/*  void SetMode(HWND hWnd, bool Windowed = TRUE, bool UseZBuffer = FALSE, long Width = 0, long Height = 0, char BPP = 0);*/
/*  char GetFormatBPP(D3DFORMAT Format);*/

    void beginScene();  
    void endScene();  

    void beginSprite(); 
    void paintSprite(TextureHandler TextureID, const D3DXVECTOR2& coor);
    void endSprite();
    void setSpriteScale(const D3DXVECTOR2& scale = D3DXVECTOR2(1.0, 1.0));
    D3DXVECTOR2 GetUnscaledCoor(const D3DXVECTOR2& coor);

    void printString(const string& text, const Color& color, const long left, const long right, const long top, const long bottom);

    void clearDisplay(long Color = 0);
    void clearZBuffer(float ZBuffer = 1.0f);
    void clear(long Color = 0, float ZBuffer = 1.0f);

/*
    long GetWidth();
    long GetHeight();
    char GetBPP();
    
    void SetPerspective(float FOV = D3DX_PI/4, float aspect = 1.3333f,
                        float Near = 1.0f, float Far = 10000.0f);
    void SetCamera(const D3DXVECTOR3& Position, const D3DXVECTOR3& Point, const D3DXVECTOR3& UpDirection);
    void ClearTranslation();
    void WorldMove(const D3DXVECTOR3& coor);
    void WorldRotateX(float angle);
    void WorldRotateY(float angle);
    void WorldRotateZ(float angle);
    void WorldScale(const D3DXVECTOR3& scale);*/
public:
    GraphicCore(EngineCore* engine); 
    ~GraphicCore();

    void init();
    virtual void shutdown();

    //use absolute path
    TextureHandler addTexture_Abs(const string& AbsolutePath, const Color& transparent = ColorNull, double activepartx = 1.0, double activeparty = 1.0);

    //terminating with '/' is required
    void setTexturesPath(const string& Path);
    //use local path
    TextureHandler addTexture(const string& SubPath, const Color& transparent = ColorNull, double activepartx = 1.0, double activeparty = 1.0);
    TextureHandler addAnimatedTexture(const vector<TextureHandler>& frames, long long milliseconds_per_frame);

    void setCursor(const string& path);

    long getTextureWidth(TextureHandler texture) const;
    long getTextureHeight(TextureHandler texture) const;
};

/*
class InputCore
{
    HWND wnd;
    IDirectInput8* device;
    IDirectInputDevice8 *mouse, *keyboard;
protected:
    void ReadMouse(void* buffer, int size = sizeof(DIMOUSESTATE));
    void ReadKeyboard(char *buffer, int size = KEYS);
public:
    IDirectInput8* GetDevice();
    HWND GetWnd() const;
    void Init(HWND hWnd, HINSTANCE hInst);
    void Shutdown();
    bool IsKeyPressed(char *buffer, int key) { return (buffer[key] & 0x80)? true : false; }
    bool IsKeyPressed(unsigned char *buffer, int key) { return (buffer[key] & 0x80)? true : false; }
};
*/

class SoundCore
{
    class Sound 
    {
        string path, extension;
        const int variations;
    public:
        //Sound(int variations, const string& path, const string& extension, IXAudio2* soundengine);
        void Play();
        const string soundpath() const;
    };

    typedef std::vector<Sound*> SoundContainer;
    SoundContainer sounds;
    //inner mechanics
//     IXAudio2* soundengine;
//     IXAudio2MasteringVoice* mastervoice;
public:
    typedef int SoundHandler;
    static const SoundHandler EMPTY_SOUND = 0;
    //SoundCore(): soundengine(nullptr), mastervoice(nullptr) {}
    SoundHandler AddSound(int variations, const string& path, const string& extension);
    void PlaySound(SoundHandler sound, int volume = 0); //this sets difference from base volume
    void Init();
    virtual void Shutdown();
};