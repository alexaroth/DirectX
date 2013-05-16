#pragma once
#include "cameraclass.h"
#include "modelLoader.h"

using namespace std;


class GameObject
{
public:
    GameObject();

    GameObject(ID3D11Device* d3d11Device, ID3D11SamplerState* CubesTexSamplerState, ID3D11RasterizerState* CWcullMode,
                      ID3D11RasterizerState* CCWcullMode, char* objFile, wstring texture, CameraClass &camera, float scale, 
                      bool hasLight, bool hasAlpha, rawModel* imported_raw);

    ~GameObject();
    void draw(ID3D11DeviceContext* d3d11DevCon, ID3D11Buffer* cbPerObjectBuffer);
    void update();    
    void setWorldPos(float x, float y, float z);
    XMMATRIX getCubeWorld();
    void changeObjectWorld(XMMATRIX &change);
    void setRotAxis(float x, float y, float z);
    void setRotating(bool rotating, float multiplier);
    rawModel* getRawModel();
    XMFLOAT3 getPos();
    void setCulled(bool cull);
    vector<XMFLOAT4> getFrustumPlanes(XMMATRIX& viewProj);
    bool cull(vector<XMFLOAT4> &frustumPlanes, vector<XMFLOAT3> AABB, XMFLOAT3 ObjPos);

private:


    rawModel *base_model;
    modelLoader *objectModel;
    bool isRotating, culled;

    ID3D11Buffer* squareIndexBuffer;
    ID3D11Buffer* squareVertBuffer;

    ID3D11SamplerState* CubesTexSamplerState;
    ID3D11RasterizerState* CWcullMode;
    ID3D11RasterizerState* CCWcullMode;

    ID3D11ShaderResourceView* objectTexture;
    cbPerFrame constbuffPerFrame;
    cbPerObject cbPerObj;
    bool hasLight;
    
    float rot, rotMult;
    XMVECTOR rotaxis;
    float posX, posY, posZ;

    XMMATRIX WVP;
    XMMATRIX cubeWorld;
    XMMATRIX Rotation;
    XMMATRIX Scale;
    XMMATRIX WorldPos;  
    XMVECTOR localCameraPos;
    CameraClass *camera;
    XMMATRIX camProjection;
};

