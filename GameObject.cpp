#include "GameObject.h"


GameObject::GameObject()
{}

GameObject::GameObject(ID3D11Device* d3d11Device, ID3D11SamplerState* CubesTexSamplerState, ID3D11RasterizerState* CWcullMode, ID3D11RasterizerState* CCWcullMode, 
                       char* objFile, wstring texture, CameraClass &camera, float scale, bool hasLight, rawModel* imported_raw)
{    
    objectModel = nullptr;
    isRotating = false;
    culled = false;
    Scale = XMMatrixScaling(scale, scale, scale);


    this -> hasLight = hasLight;
    this -> camera = &camera;
    this -> CCWcullMode = CCWcullMode;
    this -> CWcullMode = CWcullMode;
    this -> CubesTexSamplerState = CubesTexSamplerState;

    rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    camProjection = XMMatrixPerspectiveFovLH(0.314f * 3.14f, 1200/900, 1.0f, 1000.0f);
    rot = 0;    

    if (imported_raw == nullptr)
    {
        base_model = new rawModel;
        objectModel = new modelLoader();

        objectModel -> Load(objFile, base_model -> vertices, base_model -> indices);
        base_model -> AABB = objectModel -> CreateAABB(base_model -> vertices);
        base_model -> indices_number = objectModel -> getIndicesNumber();
        base_model -> vertices_number = objectModel -> getTotalPoints();
    }

    else
    {
        base_model = imported_raw;
    }

    cbPerObj.lightAffected = hasLight;
    
    D3DX11CreateShaderResourceViewFromFile(d3d11Device, texture.c_str(), nullptr, nullptr, &objectTexture, nullptr);
    
    D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * base_model -> indices_number;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &base_model -> indices[0];
	d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof( Vertex ) * base_model -> vertices_number;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData; 
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = &base_model -> vertices[0];
	d3d11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &squareVertBuffer);    

    if (objectModel != nullptr)
    {
       objectModel -> Release();
       delete(objectModel);
    }
}


void GameObject::update()
{
    localCameraPos = camera -> getPos();

    if (isRotating)
    {
        rot += 0.02f * rotMult;

	    if((rot > 6.28f)||(rot < -6.28))
	    rot = 0.0f;	

        Rotation = XMMatrixRotationAxis(rotaxis, rot);
    }

    //cubeWorld = XMMatrixIdentity();
    //WorldPos = XMMatrixIdentity();    
    
    WorldPos = XMMatrixTranslation(posX, posY, posZ);

    if(isRotating)
    {
        cubeWorld = Rotation * WorldPos;
    }

    else
    {
        cubeWorld = WorldPos;
    }

    
    cbPerObj.cameraPos.x = XMVectorGetX(localCameraPos);
    cbPerObj.cameraPos.y = XMVectorGetY(localCameraPos);
    cbPerObj.cameraPos.z = XMVectorGetZ(localCameraPos);
       
}


void GameObject::draw(ID3D11DeviceContext* d3d11DevCon, ID3D11Buffer* cbPerObjectBuffer)
{
    //culled = cull(getFrustumPlanes(camera -> GetViewMatrix() * camProjection), base_model -> AABB, XMFLOAT3(posX, posY, posZ));
    
    if (!culled)
    {
        cubeWorld = Scale * cubeWorld;
        WVP = cubeWorld * camera -> GetViewMatrix() * camProjection;    
	    cbPerObj.World = XMMatrixTranspose(cubeWorld);	
	    cbPerObj.WVP = XMMatrixTranspose(WVP); 
        cbPerObj.lightAffected = hasLight;
        cbPerObj.specularAffected = true;

        //Set the cubes index buffer
	    d3d11DevCon->IASetIndexBuffer(squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    
        //Set the cubes vertex buffer
	    UINT stride = sizeof(Vertex);
	    UINT offset = 0;
	    d3d11DevCon->IASetVertexBuffers(0, 1, &squareVertBuffer, &stride, &offset);

	
	    d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, nullptr, &cbPerObj, 0, 0);
	    d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	    d3d11DevCon->PSSetShaderResources(0, 1, &objectTexture);
	    d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);


	    d3d11DevCon->RSSetState(CCWcullMode);
        d3d11DevCon->DrawIndexed(base_model -> indices_number, 0, 0);
	    d3d11DevCon->RSSetState(CWcullMode);
	    d3d11DevCon->DrawIndexed(base_model -> indices_number, 0, 0);
    }
}


vector<XMFLOAT4> GameObject::getFrustumPlanes(XMMATRIX& viewProj)
{
	// x, y, z, and w represent A, B, C and D in the plane equation
	// where ABC are the xyz of the planes normal, and D is the plane constant
	std::vector<XMFLOAT4> tempFrustumPlane(6);

	// Left Frustum Plane
	// Add first column of the matrix to the fourth column
	tempFrustumPlane[0].x = viewProj._14 + viewProj._11; 
	tempFrustumPlane[0].y = viewProj._24 + viewProj._21;
	tempFrustumPlane[0].z = viewProj._34 + viewProj._31;
	tempFrustumPlane[0].w = viewProj._44 + viewProj._41;

	// Right Frustum Plane
	// Subtract first column of matrix from the fourth column
	tempFrustumPlane[1].x = viewProj._14 - viewProj._11; 
	tempFrustumPlane[1].y = viewProj._24 - viewProj._21;
	tempFrustumPlane[1].z = viewProj._34 - viewProj._31;
	tempFrustumPlane[1].w = viewProj._44 - viewProj._41;

	// Top Frustum Plane
	// Subtract second column of matrix from the fourth column
	tempFrustumPlane[2].x = viewProj._14 - viewProj._12; 
	tempFrustumPlane[2].y = viewProj._24 - viewProj._22;
	tempFrustumPlane[2].z = viewProj._34 - viewProj._32;
	tempFrustumPlane[2].w = viewProj._44 - viewProj._42;

	// Bottom Frustum Plane
	// Add second column of the matrix to the fourth column
	tempFrustumPlane[3].x = viewProj._14 + viewProj._12;
	tempFrustumPlane[3].y = viewProj._24 + viewProj._22;
	tempFrustumPlane[3].z = viewProj._34 + viewProj._32;
	tempFrustumPlane[3].w = viewProj._44 + viewProj._42;

	// Near Frustum Plane
	// We could add the third column to the fourth column to get the near plane,
	// but we don't have to do this because the third column IS the near plane
	tempFrustumPlane[4].x = viewProj._13;
	tempFrustumPlane[4].y = viewProj._23;
	tempFrustumPlane[4].z = viewProj._33;
	tempFrustumPlane[4].w = viewProj._43;

	// Far Frustum Plane
	// Subtract third column of matrix from the fourth column
	tempFrustumPlane[5].x = viewProj._14 - viewProj._13; 
	tempFrustumPlane[5].y = viewProj._24 - viewProj._23;
	tempFrustumPlane[5].z = viewProj._34 - viewProj._33;
	tempFrustumPlane[5].w = viewProj._44 - viewProj._43;

	// Normalize plane normals (A, B and C (xyz))
	// Also take note that planes face inward
	for(int i = 0; i < 6; ++i)
	{
		float length = sqrt((tempFrustumPlane[i].x * tempFrustumPlane[i].x) + (tempFrustumPlane[i].y * tempFrustumPlane[i].y) + (tempFrustumPlane[i].z * tempFrustumPlane[i].z));
		tempFrustumPlane[i].x /= length;
		tempFrustumPlane[i].y /= length;
		tempFrustumPlane[i].z /= length;
		tempFrustumPlane[i].w /= length;
	}

	return tempFrustumPlane;
}


bool GameObject::cull(vector<XMFLOAT4> &frustumPlanes, vector<XMFLOAT3> AABB, XMFLOAT3 ObjPos)
{
	// This is where we will check all objects for culling. In this lesson, we are only culling the trees, so if the tree is culled,
	// we will not draw it OR it's leaves. You can add other objects in your scene, and check them for culling here

	bool cull = false;

	// Loop through each frustum plane
	for(int planeID = 0; planeID < 6; ++planeID)
	{
		XMVECTOR planeNormal = XMVectorSet(frustumPlanes[planeID].x, frustumPlanes[planeID].y, frustumPlanes[planeID].z, 0.0f);
		float planeConstant = frustumPlanes[planeID].w;

		// Check each axis (x,y,z) to get the AABB vertex furthest away from the direction the plane is facing (plane normal)
		XMFLOAT3 axisVert;

		// x-axis
		if(frustumPlanes[planeID].x < 0.0f)	// Which AABB vertex is furthest down (plane normals direction) the x axis
			axisVert.x = AABB[0].x + ObjPos.x; // min x plus tree positions x
		else
			axisVert.x = AABB[1].x + ObjPos.x; // max x plus tree positions x

		// y-axis
		if(frustumPlanes[planeID].y < 0.0f)	// Which AABB vertex is furthest down (plane normals direction) the y axis
			axisVert.y = AABB[0].y + ObjPos.y; // min y plus tree positions y
		else
			axisVert.y = AABB[1].y + ObjPos.y; // max y plus tree positions y

		// z-axis
		if(frustumPlanes[planeID].z < 0.0f)	// Which AABB vertex is furthest down (plane normals direction) the z axis
			axisVert.z = AABB[0].z + ObjPos.z; // min z plus tree positions z
		else
			axisVert.z = AABB[1].z + ObjPos.z; // max z plus tree positions z

		// Now we get the signed distance from the AABB vertex that's furthest down the frustum planes normal,
		// and if the signed distance is negative, then the entire bounding box is behind the frustum plane, which means
		// that it should be culled
		if(XMVectorGetX(XMVector3Dot(planeNormal, XMLoadFloat3(&axisVert))) + planeConstant < 0.0f)
		{
			return true;
			break;
		}

        return false;

	}
    
}


void GameObject::setWorldPos(float x, float y, float z)
{
    posX = x;
    posY = y;
    posZ = z;
}


void GameObject::setRotAxis(float x, float y, float z)
{
    rotaxis = XMVectorSet(x, y, z, 0);
}


void GameObject::changeObjectWorld(XMMATRIX &change)
{
    cubeWorld = cubeWorld * change;
}


void GameObject::setRotating(bool rotating, float multiplier)
{
    this -> isRotating = rotating;
    this -> rotMult = multiplier;
}


XMMATRIX GameObject::getCubeWorld()
{
    return this -> cubeWorld;
}


XMFLOAT3 GameObject::getPos()
{
    return XMFLOAT3(posX, posY, posZ);
}


rawModel* GameObject::getRawModel()
{
    return base_model;
}


void GameObject::setCulled(bool cull)
{
    this -> culled = cull;
}


GameObject::~GameObject()
{
}