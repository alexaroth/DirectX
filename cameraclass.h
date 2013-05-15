#pragma once

#pragma comment (lib, "dinput8.lib")
#include <dinput.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <xnamath.h>

class CameraClass
{
    public:

	    CameraClass();	  
	    ~CameraClass();
        
	    void SetPosition(float, float, float);
	    void SetRotation(float, float, float);
        	
	    float roll;
	    XMMATRIX rotationMatrix;
	    void Render();
        void UpdateCamera();
	    XMMATRIX GetViewMatrix();
        XMVECTOR GetCamTarget();
        XMVECTOR getPos();
        void moveLeftRight(float x);
        void moveBackForward(float x);
        
        float camYaw;
        float camPitch;



    private:

        XMVECTOR DefaultForward; 
        XMVECTOR DefaultRight; 
        XMVECTOR camForward;
        XMVECTOR camRight;
        XMMATRIX camRotationMatrix;
        float LeftRight;
        float BackForward;
        XMVECTOR camPosition;
        XMVECTOR camTarget;
        XMVECTOR camUp;
        XMMATRIX camView;


};

