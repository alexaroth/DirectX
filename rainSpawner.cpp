#include "rainSpawner.h"


rainSpawner::rainSpawner(GameObject &object, CameraClass &camera)
{
    this -> camera = &camera;
    rainDropObject = &object;
    srand(time(nullptr));

    enabled = true;

    for (int a = 0; a < 3000; a++)
    {
        rainDrops[a] = rainDrop(random(-25,25), 10, random(-25,25), false, random(0, 3000));
    }

}


void rainSpawner::draw(ID3D11DeviceContext* d3d11DevCon, ID3D11Buffer* cbPerObjectBuffer)
{
    if (enabled)
    {
        XMFLOAT3 dropPos;

        for (int a = 0; a < 3000; a++)
        {
            dropPos = rainDrops[a].getPos();

            rainDrops[a].update(camera);

            if(rainDrops[a].isAlive())
            {            
                rainDropObject -> setWorldPos(dropPos.x, dropPos.y, dropPos.z);
                rainDropObject -> update();
                rainDropObject -> draw(d3d11DevCon, cbPerObjectBuffer);
            }
        }
    }
}

rainSpawner::rainSpawner()
{
}


rainSpawner::~rainSpawner()
{
}

void rainSpawner::setEnabled(bool enabled)
{
    this -> enabled = enabled;
}

float rainSpawner::random(float a, float b)
{
    return ((b-a)*((float)rand()/RAND_MAX))+a;
}
