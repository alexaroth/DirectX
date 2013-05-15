#pragma once
#include <xnamath.h>

struct cbPerObject
{
	XMMATRIX  WVP;
	XMMATRIX  World;
    XMFLOAT3 cameraPos;

    BOOL lightAffected; 
      
    BOOL specularAffected;        
};

 
struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}

	XMFLOAT3 pos;
	float range;
	XMFLOAT3 att;
	float pad2;
    XMFLOAT3 dir;
	float cone;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
    XMFLOAT4 specular;
    float specularPower;    
    XMFLOAT3 pad3;
};
    

struct cbPerFrame
{
	Light light;   
};


struct Vertex
{
    Vertex(){}
	Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz)
		: pos(x,y,z), texCoord(u, v), normal(nx, ny, nz){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};


struct rawModel
{
    std::vector<DWORD> indices;
    std::vector<Vertex> vertices;
    std::vector<XMFLOAT3> AABB;

    int indices_number;
    int vertices_number;
};