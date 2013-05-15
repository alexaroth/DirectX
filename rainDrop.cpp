#include "rainDrop.h"


rainDrop::rainDrop(float x, float y, float z, bool alive, float delay)
{
    this -> x = x;
    this -> y = y;
    this -> z = z;
    this -> alive = alive;    
    this -> triggerDelay = delay;
    this -> triggerTick = 0;

    initial_x = x;
    initial_y = y;
    initial_z = z;
}

XMFLOAT3 rainDrop::getPos()
{
    return XMFLOAT3(x, y, z);
}

bool rainDrop::isAlive()
{
    return this -> alive;
}

void rainDrop::update(CameraClass *camera)
{
    if (!alive)
    {
        triggerTick += 15;
    
        if (triggerTick > triggerDelay)
        {
            alive = true;
            triggerTick = 0;
        }
    }

    else
    {    
        y -= 0.45;

        if (y < -5)
        {
           y = 10;
           x = initial_x + XMVectorGetX(camera -> getPos());
           z = initial_z + XMVectorGetZ(camera -> getPos());
           alive = false;
        }
    }
}

void rainDrop::addPos(float x, float y, float z)
{
    this -> x += x;
    this -> y += y;
    this -> z += z;
}

void rainDrop::setPos(float x, float y, float z)
{
    this -> x = x;
    this -> y = y;
    this -> z = z;
}

void rainDrop::setDelay(float delay)
{
    this -> triggerDelay = delay;
}

rainDrop::~rainDrop(void)
{
}

rainDrop::rainDrop(void)
{
}

