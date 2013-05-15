#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment (lib, "D3D10_1.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "D2D1.lib")
#pragma comment (lib, "dwrite.lib")
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <D3D10_1.h>
#include <DXGI.h>
#include <D2D1.h>
#include <sstream>
#include <dwrite.h>
#include "cameraclass.h"
#include "GameObject.h"
#include "rainSpawner.h"
#include "SoundEngine.h"
#include "scruts.h"
#include "Node.h"

using namespace irrklang;

class graphics
{

public:
    graphics(int Width, int Height);
    ~graphics();

        
    void DrawScene();
    void UpdateScene(float tStep);
    bool InitScene();
    void CleanUp();
    bool InitializeDirect3d11App(int Width, int Height, HWND hwnd);
    void detectInput();
    bool InitDirectInput(HINSTANCE hInstance, HWND hwnd);

        
private:

    int Width;
    int Height;

    IDXGISwapChain* SwapChain;
    ID3D11Device* d3d11Device;
    ID3D11DeviceContext* d3d11DevCon;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* depthStencilView;
    ID3D11Texture2D* depthStencilBuffer;
    ID3D11VertexShader* VS;
    ID3D11PixelShader* PS;
    ID3D10Blob* VS_Buffer;
    ID3D10Blob* PS_Buffer;
    ID3D11InputLayout* vertLayout;
    ID3D11Buffer* cbPerObjectBuffer;
    ID3D11BlendState* Transparency;
    ID3D11RasterizerState* CCWcullMode;
    ID3D11RasterizerState* CWcullMode;
    ID3D11SamplerState* CubesTexSamplerState;
    ID3D11Buffer* cbPerFrameBuffer;
    ID3D11Texture2D *BackBuffer11;
    
    IDirectInputDevice8* KeyboardInput;
    IDirectInputDevice8* MouseInput;

    DIMOUSESTATE mouseLastState;
    LPDIRECTINPUT8 DirectInput;

    CameraClass camera1;
    UINT numElements;
    Light light;
    cbPerFrame constbuffPerFrame;

    GameObject MainObject, ChildObject1, ChildObject2, ChildObject1_1, ChildObject2_1, ChildObject2_2, ground, flashlight, rainDrop, MG, cube1, cube2, cube3;
    rainSpawner rain_spawner;

    bool lightDropped, lightDroppedPressed, rainActive, rainActivePressed;
    Node *MainNode;
    
    HRESULT hr;

    XMMATRIX camProjection;
    SoundEngine Sound;
};

