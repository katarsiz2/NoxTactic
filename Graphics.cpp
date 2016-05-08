/*#include "Graphics.h"
#include "Geometry.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <windowsx.h>

//void render_frame();
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_TEX1)
// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPDIRECT3DVERTEXBUFFER9 v_buffer; //vertex buffer
LPDIRECT3DINDEXBUFFER9 i_buffer; //index buffer
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

GraphicsInfoHolder *GIH;
void GraphicsInfoHolder::SetSize(const vInt& size) {
    data.resize(size.x);
    for(auto it = data.begin(); it != data.end(); ++it){
        (*it).resize(size.y);
    }
    mapsize = size;
}
int  GraphicsInfoHolder::AddTexture(const std::string& path, const D3DCOLOR transparent_color) {
    int i = vtext.size();

    IDirect3DTexture9 *texture;
    D3DXCreateTextureFromFileEx(d3ddev, path.data(), 0, 0, 0, 0, D3DFMT_UNKNOWN, 
        D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, transparent_color, 0, 0, &texture);
    vtext.push_back(texture);
    return 0;
}
void GraphicsInfoHolder::ClearCell(const vInt& coor) {
    data[coor.x][coor.y].clear();
}
void GraphicsInfoHolder::SetTexture(const vInt& coor, const int texture) {
    data[coor.x][coor.y].push_back(texture);
}
void GraphicsInfoHolder::SetBackgroundTexture(const int texture) {
    background_index = texture;
}
void GraphicsInfoHolder::SetScreenCoor(const vInt& coor) {
    ScreenCoor = coor;
}
void GraphicsInfoHolder::SetPortrait(const int texture) {
    portrait.texture = texture;
}

struct CUSTOMVERTEX
{
    FLOAT x, y, z;    // from the D3DFVF_XYZRHW flag
    float tu, tv;;    // from the D3DFVF_DIFFUSE flag
};
void InitializeRenderer(HINSTANCE& hInstance) {
    GIH = new GraphicsInfoHolder;

    WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_VREDRAW|CS_HREDRAW|CS_OWNDC, 
                    WndProc, 0, 0, hInstance, NULL, NULL, (HBRUSH)(COLOR_WINDOW+1), 
                    NULL, "MainWindow", NULL};
    RegisterClassEx(&wc);
    HWND hWnd = CreateWindowEx(0, "MainWindow", "Another useless YOBA game",
                               WS_EX_TOPMOST , 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT,
                               NULL, NULL, hInstance, NULL);

    d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

    D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information
    ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
    d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
    d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;    // set the back buffer format to 32-bit
    d3dpp.BackBufferWidth = WINDOW_WIDTH;    // set the width of the buffer
    d3dpp.BackBufferHeight = WINDOW_HEIGHT;    // set the height of the buffer
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // create a device class using this information and information from the d3dpp stuct
    d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
    //MessageBox(NULL, "Hello World!", "Just another useless program",
    //           MB_ICONEXCLAMATION | MB_OK);


    d3ddev->CreateVertexBuffer(4*sizeof(CUSTOMVERTEX), 0, CUSTOMFVF, D3DPOOL_MANAGED,
                            &v_buffer, NULL);
    d3ddev->CreateIndexBuffer(6*sizeof(short), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED,
                           &i_buffer, NULL);
    d3ddev->SetFVF(CUSTOMFVF);
    CUSTOMVERTEX vertices[] =
    {
        { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f, 0.0f }
    };
    short indices[] =
    {  
        0, 1, 2,  
        2, 0, 3
    };
    VOID* pVoid;    // the temporary void pointer; LOCK fills it, then we use it
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);    
    memcpy(pVoid, vertices, sizeof(vertices));    
    v_buffer->Unlock();
    i_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, indices, sizeof(indices));
    i_buffer->Unlock();

    d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE); 
    d3ddev->SetRenderState(D3DRS_ZENABLE, FALSE);
    d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);    // both sides of the triangles
    d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    d3ddev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    d3ddev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    d3ddev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    d3ddev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    d3ddev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
}

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return(0);
        case WM_PAINT: 
            d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
            d3ddev->Present(NULL, NULL, NULL, NULL);

            ValidateRect(hwnd, NULL);
            return(0);
    }

    return(DefWindowProc(hwnd, msg, wParam, lParam));
}
void ShutDownRenderer(){
    i_buffer->Release();
    v_buffer->Release();
    d3ddev->Release();
    d3d->Release();
    delete GIH;
}

void render_frame()
{

    IDirect3DTexture9 *texture;
    HRESULT tmp = D3DXCreateTextureFromFileEx(d3ddev, "C:\\Tracer.png", 0, 0, 0, 0, D3DFMT_UNKNOWN, 
        D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_ARGB(255, 255,255,255), 0, 0, &texture);

    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    //d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    d3ddev->BeginScene();    // begins the 3D scene
    d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
    d3ddev->SetIndices(i_buffer);

    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView,
                       &D3DXVECTOR3 (20.0f, 20.0f, 100.0f),    // the camera position
                       &D3DXVECTOR3 (20.0f, 20.0f, 0.0f),    // the look-at position
                       &D3DXVECTOR3 (0.0f, 1.0f, 0.0f));    // the up direction
    d3ddev->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

    D3DXMATRIX matProjection;     // the projection transform matrix
    D3DXMatrixPerspectiveFovLH(&matProjection,
                               D3DXToRadian(45),    // the horizontal field of view
                               (FLOAT)WINDOW_WIDTH / (FLOAT)WINDOW_HEIGHT, // aspect ratio
                               1.0f,    // the near view-plane
                               100.0f);    // the far view-plane
    d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection);    // set the projection

    D3DXMATRIX matScale;
    if (GIH->background_index != 0) {
        D3DXMatrixScaling(&matScale, (float)picsizex*GIH->mapsize.x, (float)picsizey*GIH->mapsize.y, 0.0f);
        d3ddev->SetTexture(0, GIH->vtext[GIH->background_index]);
        d3ddev->SetTransform(D3DTS_WORLD, &(matScale));
        d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
    }
    
    D3DXMatrixScaling(&matScale, (float)picsizex/10, (float)picsizey/10, 0.0f);
    D3DXMATRIX matTranslate;
    for (int x = 0; x != GIH->data.size(); ++x){
        for (int y = 0; y != GIH->data[x].size(); ++y) {
            for (int z = 0; z != GIH->data[x][y].size(); ++z) {
                //if (GIH->data[x][y][z] != 0) {
    
                    
                    D3DXMatrixTranslation(&matTranslate, (float)picsizex*x/10, (float)picsizex*y/10, 0.0f);
                    //d3ddev->SetTexture(0, GIH->vtext[GIH->data[x][y][z]]);
                    d3ddev->SetTexture(0, texture);
                    d3ddev->SetTransform(D3DTS_WORLD, &(matScale*matTranslate));
                    d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
                //}
            }
        }
    }

    d3ddev->EndScene();    // ends the 3D scene
    d3ddev->Present(NULL, NULL, NULL, NULL);    // displays the created frame
}*/