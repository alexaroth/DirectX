#pragma once
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <sstream>
#include <vector>
#include <fstream>
#include "scruts.h"

using namespace std;

class modelLoader
{
    public:
        modelLoader();
        ~modelLoader();      

        int Load(char* filename, vector<Vertex> &vertices, vector<DWORD> &indices);	// Loads the model with specified coordinates
	    void Release();		
        float* getNormals();
        float* getTriangleFaces();
        int getIndicesNumber();        
        int getTotalPoints();
        vector<XMFLOAT3> CreateAABB(vector<Vertex> &vertPosArray);

    private:

        float* vertexBuffer;	
        vector<XMFLOAT3> normals;        
        float* triangle_faces;	

        vector<int> normal_indices;
        vector<int> triangle_indices;
        vector<float> textureCoords;
        int triangles;
        int totalVerts;
        int Indices_index, texture_index, textureCoordCount;
	    long total_connected_points;			
	    long Total_connected_triangles;		
        string line;   
        bool normals_given, texture_given;
};