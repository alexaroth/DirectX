#pragma once
#include  <time.h>  
#include "GameObject.h"
#include "rainDrop.h"

class rainSpawner
{

public:
    rainSpawner();
    rainSpawner(GameObject &object, CameraClass &camera);
    ~rainSpawner();
    float random(float a, float b);
    void draw(ID3D11DeviceContext* d3d11DevCon, ID3D11Buffer* cbPerObjectBuffer);
    void setEnabled(bool enabled);

private:

    rainDrop rainDrops[3000];
    GameObject *rainDropObject;
    CameraClass *camera;
    bool enabled;
};


