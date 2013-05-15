#include "modelLoader.h"
#define points_per_vertex 3
#define total_floats_in_triangle 9



modelLoader::modelLoader()
{
    Total_connected_triangles = 0; 
    total_connected_points = 0;
}
 
  
int modelLoader::Load(char* filename, vector<Vertex> &vertices, vector<DWORD> &indices)
{ 
    ifstream objFile (filename);

    if (objFile.is_open())													
    {
	    objFile.seekg (0, ios::end);										// go to end of file
	    long fileSize = objFile.tellg();									
	    objFile.seekg (0, ios::beg);										
	    vertexBuffer = (float*) malloc (fileSize);							// memory for verteces
	    triangle_faces = (float*) malloc(fileSize*sizeof(float));			// memory for triangles

	    int triangle_index = 0;		
        Indices_index = 0;
        triangles = 0;
        totalVerts = 0;
        texture_index = 0;
        textureCoordCount = 0;
        normals_given = false;
        texture_given = false;
 
	    while (!objFile.eof())			
	    {		
		    getline (objFile,line);											
 
		    if ((line.c_str()[0] == 'v')&&(line.c_str()[1] == ' '))										// The first character is 'v' = vertex
		    {
			    line[0] = ' ';												

                sscanf(line.c_str(),"%f %f %f",	&vertexBuffer[total_connected_points], &vertexBuffer[total_connected_points + 1],	&vertexBuffer[total_connected_points + 2]);
			    vertexBuffer[total_connected_points];
			    vertexBuffer[total_connected_points + 1];
			    vertexBuffer[total_connected_points + 2];   

			    total_connected_points += points_per_vertex;					// Add 3 to the total connected points
                totalVerts++;
		    }

		    if (line.c_str()[0] == 'f')										// The first character is 'f' = point
		    {
	    	    line[0] = ' ';	

                int vertexNumber[6] = { 0, 0, 0, 0, 0, 0 };

                if (line.find('/') != -1)	
                {	
                    sscanf(line.c_str(),"%i//%i %i//%i %i//%i",							
			        &vertexNumber[0], &vertexNumber[1], &vertexNumber[2], &vertexNumber[3], &vertexNumber[4], &vertexNumber[5]);										

			        vertexNumber[0] -= 1;										
			        vertexNumber[1] -= 1;										
			        vertexNumber[2] -= 1;	
                    vertexNumber[3] -= 1;										
			        vertexNumber[4] -= 1;										
			        vertexNumber[5] -= 1;


                    normal_indices.push_back(vertexNumber[1]);
                    normal_indices.push_back(vertexNumber[3]);
                    normal_indices.push_back(vertexNumber[5]);

                /*
			      Makes triangles using thr stored points 		  
			    */

                    triangle_indices.push_back(vertexNumber[0]);
                    triangle_indices.push_back(vertexNumber[2]);
                    triangle_indices.push_back(vertexNumber[4]);                
                
                }

                else
                {         
                    sscanf(line.c_str(),"%i %i %i",								// Read integers from the line:  f 1 2 3
			        &vertexNumber[0], &vertexNumber[1], &vertexNumber[2]);										

			        vertexNumber[0] -= 1;										
			        vertexNumber[1] -= 1;										
			        vertexNumber[2] -= 1;	

                 /*
			      Makes triangles using thr stored points 		  
			    */

                    triangle_indices.push_back(vertexNumber[0]);
                    triangle_indices.push_back(vertexNumber[1]);
                    triangle_indices.push_back(vertexNumber[2]);
                           

                }


  
			    int tCounter = 0;
			    for (int i = 0; i < points_per_vertex; i++)					
			    {
				    triangle_faces[triangle_index + tCounter    ] = vertexBuffer[3*vertexNumber[i]    ];
				    triangle_faces[triangle_index + tCounter + 1] = vertexBuffer[3*vertexNumber[i] + 1];
				    triangle_faces[triangle_index + tCounter + 2] = vertexBuffer[3*vertexNumber[i] + 2];
				    tCounter += points_per_vertex;
			    }


                triangles++; 
				triangle_index += total_floats_in_triangle;		
				Total_connected_triangles += total_floats_in_triangle;		
                Indices_index += 3;

			    }	

            if ((line.c_str()[0] == 'v') && (line.c_str()[1] == 'n'))
            {	
                normals_given = true;

                line[0] = ' ';	
                line[1] = ' ';

                float Normals[4] = { 0, 0, 0 };
                sscanf(line.c_str(),"%f %f %f", &Normals[0], &Normals[1], &Normals[2]);

                normals.push_back(XMFLOAT3(Normals[0], Normals[1], Normals[2]));
            }

		             
            if ((line.c_str()[0] == 'v') && (line.c_str()[1] == 't'))
            {
                texture_given = true;
                line[0] = ' ';	
                line[1] = ' ';

                float TexCoords[3] = { 0, 0 };

                sscanf(line.c_str(),"%f %f", &TexCoords[0], &TexCoords[1]);

                textureCoords.push_back(TexCoords[0]);
                textureCoords.push_back(TexCoords[1]);
                
                texture_index += 2;
            }

        }

              

        if (!texture_given)
         {
             textureCoords.resize(totalVerts * 2);
         }
        if (!normals_given)
        {
             normals.resize(totalVerts);        
             normal_indices.resize(triangle_indices.size());             
        }
        
        // set the vertices
        int pointCount = 0, normal_count = 0;
        while(pointCount < totalVerts * 3)
        {
            vertices.push_back(Vertex(vertexBuffer[pointCount], vertexBuffer[pointCount + 1], vertexBuffer[pointCount + 2],
                                textureCoords[textureCoordCount], textureCoords[textureCoordCount + 1],
                                normals[normal_indices[normal_count]].x, normals[normal_indices[normal_count]].y, normals[normal_indices[normal_count]].z));  
            pointCount += 3;
            normal_count ++;
            textureCoordCount += 2;
        }
        
        pointCount = 0;

        while(pointCount < triangle_indices.size())
        {
           vertices[triangle_indices[pointCount]].normal = normals[normal_indices[pointCount]];
           // vertices[pointCount].normal = vertices[pointCount].pos;

            pointCount ++;
        }

        

        
        if (!normals_given)
        {
            std::vector<XMFLOAT3> tempNormal;

		    //normalized and unnormalized normals
		    XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

		    //Used to get vectors (sides) from the position of the verts
		    float vecX, vecY, vecZ;

		    //Two edges of our triangle
		    XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		    XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		    //Compute face normals
		    for(int i = 0; i < triangles; ++i)
		    {
			    //Get the vector describing one edge of our triangle (edge 0,2)
			    vecX = vertices[triangle_indices[(i*3)]].pos.x - vertices[triangle_indices[(i*3)+2]].pos.x;
			    vecY = vertices[triangle_indices[(i*3)]].pos.y - vertices[triangle_indices[(i*3)+2]].pos.y;
			    vecZ = vertices[triangle_indices[(i*3)]].pos.z - vertices[triangle_indices[(i*3)+2]].pos.z;		
			    edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our first edge

			    //Get the vector describing another edge of our triangle (edge 2,1)
			    vecX = vertices[triangle_indices[(i*3)+2]].pos.x - vertices[triangle_indices[(i*3)+1]].pos.x;
			    vecY = vertices[triangle_indices[(i*3)+2]].pos.y - vertices[triangle_indices[(i*3)+1]].pos.y;
			    vecZ = vertices[triangle_indices[(i*3)+2]].pos.z - vertices[triangle_indices[(i*3)+1]].pos.z;		
			    edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);	//Create our second edge

			    //Cross multiply the two edge vectors to get the un-normalized face normal
			    XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));
			    tempNormal.push_back(unnormalized);			//Save unormalized normal (for normal averaging)
		    }
        
            XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	        int facesUsing = 0;
	        float tX = 0;
	        float tY = 0;
	        float tZ = 0;
        
            
		    //Go through each vertex
		    for(int i = 0; i < totalVerts; ++i)
		    {
			    //Check which triangles use this vertex
			    for(int j = 0; j < triangles; ++j)
			    {
				    if(triangle_indices[j*3] == i ||
					    triangle_indices[(j*3)+1] == i ||
					    triangle_indices[(j*3)+2] == i)
				    {
					    tX = XMVectorGetX(normalSum) + tempNormal[j].x;
					    tY = XMVectorGetY(normalSum) + tempNormal[j].y;
					    tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

					    normalSum = XMVectorSet(tX, tY, tZ, 0.0f);	//If a face is using the vertex, add the unormalized face normal to the normalSum
					    facesUsing++;
				    }
			    }
            
			    //Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
			    normalSum = normalSum / facesUsing;
           
			    //Normalize the normalSum vector
			    normalSum = XMVector3Normalize(normalSum);

			    //Store the normal in our current vertex
			    vertices[i].normal.x = XMVectorGetX(-normalSum);
			    vertices[i].normal.y = XMVectorGetY(-normalSum);
			    vertices[i].normal.z = XMVectorGetZ(-normalSum);

			    //Clear normalSum and facesUsing for next vertex
			    normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
			    facesUsing = 0;
            
		    }


            
            
            XMFLOAT3 temp = XMFLOAT3(0.0f, 0.0f, 0.0f);

            for(int i = 0; i < totalVerts; ++i)
            {
                for(int i2 = 0; i2 < totalVerts; ++i2)
                {               
                    if((vertices[i].pos.x == vertices[i2].pos.x)&&(vertices[i].pos.y == vertices[i2].pos.y)&&(vertices[i].pos.z == vertices[i2].pos.z))
                    {
                        temp.x += vertices[i].normal.x;
                        temp.y += vertices[i].normal.y;
                        temp.z += vertices[i].normal.z;
                    }
                }



                XMVECTOR tempVector = XMVectorSet(temp.x, temp.y ,temp.z, 0);
                tempVector = XMVector3Normalize(tempVector);

                vertices[i].normal.x = XMVectorGetX(tempVector);
                vertices[i].normal.y = XMVectorGetY(tempVector);
                vertices[i].normal.z = XMVectorGetZ(tempVector);

                temp.x = 0;
                temp.y = 0;
                temp.z = 0;
            }

        }
        

        // set the indices
        pointCount = 0;
        while(pointCount < getIndicesNumber())
        {
            indices.push_back((DWORD)triangle_indices[pointCount]);
            pointCount++;
        }
		   
        objFile.close();														
    }    
    
	return 0;
}
 
void modelLoader::Release()
{
	delete(triangle_faces);
	delete(vertexBuffer);  
}
 

vector<XMFLOAT3> modelLoader::CreateAABB(vector<Vertex> &vertPosArray)
{
	XMFLOAT3 minVertex = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	XMFLOAT3 maxVertex = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for(UINT i = 0; i < totalVerts; i++)
	{		
		// The minVertex and maxVertex will most likely not be actual vertices in the model, but vertices that use the smallest and largest x, y, 
        //and z values from the model to be sure ALL vertices are covered by the bounding volume

		//Get the smallest vertex 
        minVertex.x = min(minVertex.x, vertPosArray[i].pos.x);	// Find smallest x value in model
		minVertex.y = min(minVertex.y, vertPosArray[i].pos.y);	// Find smallest y value in model
		minVertex.z = min(minVertex.z, vertPosArray[i].pos.z);	// Find smallest z value in model

		//Get the largest vertex 
		maxVertex.x = max(maxVertex.x, vertPosArray[i].pos.x);	// Find largest x value in model
		maxVertex.y = max(maxVertex.y, vertPosArray[i].pos.y);	// Find largest y value in model
		maxVertex.z = max(maxVertex.z, vertPosArray[i].pos.z);	// Find largest z value in model
	}	

	std::vector<XMFLOAT3> AABB;

	// Our AABB [0] is the min vertex and [1] is the max
	AABB.push_back(minVertex);
	AABB.push_back(maxVertex);

	return AABB;
}


float* modelLoader::getTriangleFaces()
{
    return triangle_faces;
}

int modelLoader::getTotalPoints()
{
    return totalVerts;
}

int modelLoader::getIndicesNumber()
{
    return Indices_index;
}

modelLoader::~modelLoader()
{

}
