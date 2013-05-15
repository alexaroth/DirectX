#include "cameraclass.h"


CameraClass::CameraClass()
{
    DefaultForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
    DefaultRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
    camForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
    camRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
    
    LeftRight = 0.0f;
    BackForward = 0.0f;    
    camYaw = 0.0f;
    camPitch = 0.0f;    	    
    
	camPosition = XMVectorSet( 0.0f, 0.0f, -20.0f, 0.0f );
	camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
}


void CameraClass::SetPosition(float x, float y, float z)
{   
    camPosition = XMVectorSet( x, y, z, 0.0f );
}


XMVECTOR CameraClass::getPos()
{
    return camPosition;
}


void CameraClass::UpdateCamera()
{
    camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix );
	camTarget = XMVector3Normalize(camTarget);

	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);

	camPosition += LeftRight * camRight;
	camPosition += BackForward * camForward;

	LeftRight = 0.0f;
	BackForward = 0.0f;

	camTarget += camPosition;	

	camView = XMMatrixLookAtLH(camPosition, camTarget, camUp);
}


void CameraClass::moveBackForward(float x)
{
    BackForward += x;
}


void CameraClass::moveLeftRight(float x)
{
    LeftRight += x;
}


XMMATRIX CameraClass::GetViewMatrix()
{
	return camView;
}

XMVECTOR CameraClass::GetCamTarget()
{
    return camTarget;
}


CameraClass::~CameraClass()
{
}