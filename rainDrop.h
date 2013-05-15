#pragma once
#include "GameObject.h"

class rainDrop
{

public:

    rainDrop(float x, float y, float z, bool alive, float delay);
    rainDrop();
    ~rainDrop();
    XMFLOAT3 getPos();
    void addPos(float x, float y, float z);
    void setPos(float x, float y, float z);
    bool isAlive();
    void setAlive();
    void setDelay(float delay);
    void update(CameraClass *camera);

private:
    float x, y, z;
    float initial_x, initial_y, initial_z;
    float triggerDelay;
    bool alive;
    int triggerTick;
};

