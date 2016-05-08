#include "Engine.h"
#include <atlbase.h>
#include <Windows.h>

#pragma comment (lib, "d3d9")
#pragma comment (lib, "d3dx9")
#pragma comment (lib, "dxguid")
#pragma comment (lib, "dinput8")
EngineCore* core;
LRESULT WINAPI MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_CLOSE) {
        DestroyWindow(hWnd);
        PostQuitMessage(0);
    }
    if (!core) {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    } else {
        return core->MsgProc(hWnd, uMsg, wParam, lParam);
    }
}

HWND EngineCore::getWnd() const { return wnd; }
HINSTANCE EngineCore::getInstance() const { return inst; }

void EngineCore::moveWindow(long newx, long newy) {}
void EngineCore::resizeWindow(long newwidth, long newheight) {}
void EngineCore::setMouseVisibility(bool val) {}

EngineCore::EngineCore(HINSTANCE& hInstance, const std::string& classname, const std::string& caption, const string& cursor, const string& icon,
    long window_width, long window_height, long startposx, long startposy):
    inst(hInstance), classname(classname), caption(caption), width(window_width), height(window_height),
    pos_x(startposx), pos_y(startposy), wnd(0) {    
    {
    //http://stackoverflow.com/questions/2647429/c-windows-path-to-the-folder-where-the-executable-is-located
        HMODULE hModule = GetModuleHandleW(NULL);
        CHAR path[MAX_PATH];
        GetModuleFileName(hModule, path, MAX_PATH);

        applicationPath = std::string(path);
        int dotpos = applicationPath.find_last_of('\\');
        applicationExeName = std::string(applicationPath.begin() + dotpos, applicationPath.end());
        applicationPath.erase(applicationPath.begin() + dotpos, applicationPath.end());
    }
    WNDCLASSEX wc;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = classname.c_str();
    wc.lpszMenuName = NULL;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ::MsgProc;
    wc.hInstance = hInstance;
    wc.hIcon = reinterpret_cast<HICON>(LoadImage(NULL, (applicationPath+icon).data(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
    wc.hIconSm = NULL;
    wc.hCursor = reinterpret_cast<HCURSOR>(LoadImage(NULL, (applicationPath+cursor).data(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE));
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;

    SetLastError(0);
    RegisterClassEx(&wc);
    DWORD t = GetLastError();
    wnd = CreateWindowEx(0, classname.c_str(), caption.c_str(),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, width, height,
        NULL, NULL, hInstance, NULL);


    
    ShowWindow(wnd, SW_SHOW);
    UpdateWindow(wnd);
}
EngineCore::~EngineCore() {
    UnregisterClass(classname.c_str(), inst);
}

GraphicCore::GraphicCore(EngineCore* engine): wnd(engine->getWnd()), 
    width(engine->getWindowWidth()), height(engine->getWindowHeight()), iszbuffer(false),
    d3d(nullptr), device(nullptr), sprite(nullptr), currentSpriteScale(D3DXVECTOR2(1.0, 1.0)), cursor(nullptr) {}
GraphicCore::~GraphicCore() {}

void GraphicCore::init(){
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS d3dpp;        // create a struct to hold various device information
    ZeroMemory(&d3dpp, sizeof(d3dpp));  // clear out the struct for use
    d3dpp.Windowed = TRUE;              // program windowed, not fullscreen
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
    d3dpp.hDeviceWindow = wnd;          // set the window to be used by Direct3D
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;    // set the back buffer format to 32-bit
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferWidth = width;      // set the width of the buffer
    d3dpp.BackBufferHeight = height;    // set the height of the buffer
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.MultiSampleQuality			= 0;
    d3dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;
    d3dpp.PresentationInterval			= D3DPRESENT_INTERVAL_IMMEDIATE;

    d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &device);



    device->SetRenderState(D3DRS_LIGHTING, FALSE); 
    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    
    device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    D3DXCreateSprite(device, &sprite);

    D3DXCreateFont(device, 15/*fontHeight*/, 0, 0, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Calibri"), &font);
}     
void GraphicCore::shutdown(){
    for (auto it = textures.begin(); it != textures.end(); ++it) {
        delete *it;
    }
    sprite->Release();
    device->Release();
    d3d->Release();
}

/*
void GraphicCore::SetMode(HWND hWnd, bool Windowed, bool UseZBuffer,
                long Width, long Height, char BPP){}*/

void GraphicCore::beginScene(){
    device->BeginScene();
}  
void GraphicCore::endScene(){
    device->EndScene();
}  

void GraphicCore::beginSprite(){
    device->BeginScene();
    sprite->Begin(D3DXSPRITE_ALPHABLEND);
}
void GraphicCore::paintSprite(TextureHandler TextureID, const D3DXVECTOR2& coor) {
    if (TextureID == 0) { return; }
    if ((unsigned)TextureID > textures.size()) { throw ("Wrong texture id"); }
    sprite->Draw(textures[TextureID-1]->MyD3DTexture(), NULL, NULL,
        &D3DXVECTOR3(coor.x / currentSpriteScale.x, coor.y / currentSpriteScale.y, 0), 0xffffffff);
}
void GraphicCore::endSprite(){
    sprite->End();
    device->EndScene();
    device->Present(NULL,NULL,NULL,NULL);
} 
void GraphicCore::setSpriteScale(const D3DXVECTOR2& scale) {
    if (scale.x != currentSpriteScale.x || scale.y != currentSpriteScale.y) {
        currentSpriteScale = scale;
        D3DXMATRIX matrix;
        D3DXMatrixScaling(&matrix, scale.x, scale.y, 1.0);
        sprite->SetTransform(&matrix);
    }
}

D3DXVECTOR2 GraphicCore::GetUnscaledCoor(const D3DXVECTOR2& coor) {
    return D3DXVECTOR2(coor.x / currentSpriteScale.x, coor.y / currentSpriteScale.y);
}

void GraphicCore::printString(const string& text, const Color& color, const long left, const long right, const long top, const long bottom) {
    RECT lineRect;

    lineRect.left = left;
    lineRect.top = top;
    lineRect.right = right;
    lineRect.bottom = bottom;

    font->DrawTextA(sprite, text.data(), -1, &lineRect, DT_LEFT, color);
}


void GraphicCore::clearDisplay(long Color){
    device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, Color, 1.0f, 0 );
}
void GraphicCore::clearZBuffer(float ZBuffer){}
void GraphicCore::clear(long Color, float ZBuffer){}
/*

long GraphicCore::GetWidth(){ return width; }
long GraphicCore::GetHeight(){ return height; }
char GraphicCore::GetBPP(){ return bpp; }

void GraphicCore::SetPerspective(float fov, float aspect, float Near, float Far){
    D3DXMatrixPerspectiveFovLH(&Perspective, fov, aspect, Near, Far);
}
void GraphicCore::SetCamera(const D3DXVECTOR3& Position, const D3DXVECTOR3& Point, const D3DXVECTOR3& UpDirection) {
    D3DXMatrixLookAtLH(&Camera, &Position, &Point, &UpDirection);
}
void GraphicCore::ClearTranslation(){
    D3DXMatrixScaling(&World, 1, 1, 1);
}
void GraphicCore::WorldMove(const D3DXVECTOR3& coor){
    D3DXMATRIX matrix;
    D3DXMatrixTranslation(&matrix, coor.x, coor.y, coor.z);
    World = World * matrix;
}
void GraphicCore::WorldRotateX(float angle){
    D3DXMATRIX matrix;
    D3DXMatrixRotationX(&matrix, angle);
    World = World * matrix;}
void GraphicCore::WorldRotateY(float angle){
    D3DXMATRIX matrix;
    D3DXMatrixRotationY(&matrix, angle);
    World = World * matrix;}
void GraphicCore::WorldRotateZ(float angle){
    D3DXMATRIX matrix;
    D3DXMatrixRotationZ(&matrix, angle);
    World = World * matrix;
}
void GraphicCore::WorldScale(const D3DXVECTOR3& scale){
    D3DXMATRIX matrix;
    D3DXMatrixScaling(&matrix, scale.x, scale.y, scale.z);
    World = World * matrix;
}*/

//use absolute path
GraphicCore::TextureHandler GraphicCore::addTexture_Abs(const std::string& AbsolutePath, const Color& transparent, double activepartx, double activeparty) {
    try {
        Texture *tmptext = new Texture(device, AbsolutePath, transparent, activepartx, activeparty);
        textures.push_back(tmptext);
    }
    catch (const string&) {
        return 1; //the first texture should be representing NOICON
    }
    return textures.size();
}
//terminating with '/' is required
void GraphicCore::setTexturesPath(const std::string& Path){
    TexturesPath = Path;
}
//use local path
GraphicCore::TextureHandler GraphicCore::addTexture(const std::string& SubPath, const Color& transparent, double activepartx, double activeparty) {
    return addTexture_Abs(TexturesPath + SubPath, transparent, activepartx, activeparty);
}
GraphicCore::TextureHandler GraphicCore::addAnimatedTexture(const vector<TextureHandler>& frames, long long milliseconds_per_frame)
{
    vector<Texture*> tmp;
    for (auto it = frames.begin(); it != frames.end(); ++it) {
        Texture* tmptext = dynamic_cast<Texture*>(textures[*it-1]);
        if (tmptext == nullptr) {
            throw("WRONG TEXTURE TYPE");
        }
        tmp.push_back(tmptext);
    }
    AnimatedTexture* tmpanitext = new AnimatedTexture(tmp, milliseconds_per_frame);
    textures.push_back(tmpanitext);
    return textures.size();
}

GraphicCore::Texture::Texture(const Texture& another): texture(another.texture), TextureBase(another.width, another.height) {}
GraphicCore::Texture::Texture(IDirect3DDevice9 *device, const std::string& Path, Color transparent, double activepartx, double activeparty) {
    HRESULT hr;
    if (transparent.IsNull()) {
        hr = D3DXCreateTextureFromFileEx(device, Path.data(), 0, 0, 0, 0, D3DFMT_UNKNOWN,
            D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &texture);
    } else {
        hr = D3DXCreateTextureFromFileEx(device, Path.data(), 0, 0, 0, 0, D3DFMT_UNKNOWN,
            D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, transparent, 0, 0, &texture);
    }
    if FAILED(hr) {
        throw("wrong texture: " + Path);
    }
    D3DXIMAGE_INFO info;
    D3DXGetImageInfoFromFile(Path.data(), &info);
    width = (long)(info.Width * activepartx);
    height = (long)(info.Height * activeparty);
}
GraphicCore::Texture::~Texture() {
    texture->Release();
}
IDirect3DTexture9* GraphicCore::AnimatedTexture::MyD3DTexture() const {
    steady_clock::time_point cur = steady_clock::now();
    long long frames_passed = (duration_cast<milliseconds>(cur - start)).count() / time_per_frame;
    int frame = frames_passed % frames_number;
    return frames[frame]->MyD3DTexture();
}

void GraphicCore::setCursor(const string& path) {
    cursor = LoadCursorFromFile(path.data());
    SetCursor(cursor);
}

long GraphicCore::getTextureWidth(TextureHandler texture) const {
    return textures[texture-1]->getWidth();
}
long GraphicCore::getTextureHeight(TextureHandler texture) const {
    return textures[texture-1]->getHeight();
}



/*
IDirectInput8* InputCore::GetDevice() {
    return device;
}
HWND InputCore::GetWnd() const {
    return wnd;
}
void InputCore::Init(HWND hWnd, HINSTANCE hInst) {
    DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&device, NULL);
    device->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
    device->CreateDevice(GUID_SysMouse, &mouse, NULL);
    keyboard->SetDataFormat(&c_dfDIKeyboard);
    mouse->SetDataFormat(&c_dfDIMouse);
    keyboard->SetCooperativeLevel(wnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    mouse->SetCooperativeLevel(wnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

    keyboard->Acquire();
    mouse->Acquire();
}
void InputCore::Shutdown() {
    mouse->Unacquire();
    keyboard->Unacquire();
    mouse->Release();
    keyboard->Release();
    device->Release();
}

void InputCore::ReadKeyboard(char *buffer, int size) {
    keyboard->GetDeviceState(size, buffer);
}
void InputCore::ReadMouse(void *buffer, int size) {
    mouse->GetDeviceState(size, buffer);
}*/

SoundCore::SoundHandler SoundCore::AddSound(int variations, const string& path, const string& extension) {
    sounds.push_back(nullptr);
    int tmp = sounds.size();
    //sounds[tmp-1] = new Sound(variations, path, extension, soundengine);
    return tmp;
}
void SoundCore::PlaySound(SoundHandler sound, int volume) {
    //TODO: sound engine
}
/*
void SoundCore::Init() {
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    //create the engine
    if (FAILED(XAudio2Create(&soundengine))) {
        CoUninitialize();
        return;
    }
    //create the mastering voice
    if (FAILED(soundengine->CreateMasteringVoice(&mastervoice))) {
        soundengine->Release();
        CoUninitialize();
        return;
    }
}
*/void SoundCore::Shutdown() {
    //release the engine, NOT the voices!
//     mastervoice->DestroyVoice();
//     soundengine->Release();
//     CoUninitialize();
}

//SoundCore::Sound::Sound(int variations, const string& path, const string& extension, IXAudio2* soundengine): 
//variations(variations), path(path), extension(extension) {}

const string SoundCore::Sound::soundpath() const {
    //TODO: set up a randomizer
    if (variations == 1) {
        return path+extension;
    }/* else {
        char buf[10];
        _itoa_s(randpm, buf, 10);;
        return path+string(buf)+extension;
    }*/
    return path+extension;
}
