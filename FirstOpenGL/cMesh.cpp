#include "cMesh.h"

#include <glm/glm.hpp>		// cross product (I hope)

cMesh::cMesh()
{
	this->numberOfVertices = 0;
	this->numberOfTriangles = 0;
	//
	this->maxExtent = 0.0f;
	this->scaleForUnitBBox = 1.0f;
	return;
}


cMesh::~cMesh()
{

	return;
}

void cMesh::CalculateExtents(void)
{
	// Assume 1st vertx is both max and min
	this->minXYZ.x = this->pVertices[0].x;
	this->minXYZ.y = this->pVertices[0].y;
	this->minXYZ.z = this->pVertices[0].z;
	this->maxXYZ.x = this->pVertices[0].x;
	this->maxXYZ.y = this->pVertices[0].y;
	this->maxXYZ.z = this->pVertices[0].z;


	for (int index = 0; index != this->numberOfVertices; index++)
	{
		if (this->pVertices[index].x < this->minXYZ.x)
		{
			this->minXYZ.x = this->pVertices[index].x;
		}
		if (this->pVertices[index].x > this->maxXYZ.x)
		{
			this->maxXYZ.x = this->pVertices[index].x;
		}
		
		// Y
		if (this->pVertices[index].y < this->minXYZ.y)
		{
			this->minXYZ.y = this->pVertices[index].y;
		}
		if (this->pVertices[index].y > this->maxXYZ.y)
		{
			this->maxXYZ.y = this->pVertices[index].y;
		}

		// Z
		if (this->pVertices[index].z < this->minXYZ.z)
		{
			this->minXYZ.z = this->pVertices[index].z;
		}
		if (this->pVertices[index].z > this->maxXYZ.z)
		{
			this->maxXYZ.z = this->pVertices[index].z;
		}

	}// for (int index...

	this->maxExtentXYZ.x = this->maxXYZ.x - this->minXYZ.x;
	this->maxExtentXYZ.y = this->maxXYZ.y - this->minXYZ.y;
	this->maxExtentXYZ.z = this->maxXYZ.z - this->minXYZ.z;

	this->maxExtent = this->maxExtentXYZ.x;
	if (this->maxExtent < this->maxExtentXYZ.y) 
	{	// Y is bigger
		this->maxExtent = this->maxExtentXYZ.y;
	}
	if (this->maxExtent < this->maxExtentXYZ.z) 
	{	// Z is bigger
		this->maxExtent = this->maxExtentXYZ.z;
	}
	this->scaleForUnitBBox = 1.0f / this->maxExtent;

	return;
}

// Takes an indexed model and makes just a vertex array model
void cMesh::FlattenIndexedModel(void)
{
	// 1. Make a copy of the original vertices
	// 2. Using the triangle array, replace the original data

	int origNumVertices = this->numberOfVertices;

//	cVertex_xyz_rgb vertOrig[MAX_VERTEX_ARRAY_SIZE]; // Stack

	cVertex_xyz_rgb* pVertOrig = new cVertex_xyz_rgb[this->numberOfVertices]; // Heap

	for ( int index = 0; index < origNumVertices; index++ )
	{
		pVertOrig[index] = this->pVertices[index];
	}
	// We now have a copy of the original vertices

	// Adjust the original vertex array
	delete [] this->pVertices;		// GOTCHA!

//	int numberOfVertsTOTALGUESS = this->numberOfVertices * 4;
//	this->pVertices = new cVertex_xyz_rgb[numberOfVertsTOTALGUESS];

	// Actually, the number of vertices would be 
	//	the number of triangles x 3...
	int numberOfVertsNeeded = this->numberOfTriangles * 3;
	numberOfVertsNeeded += 30;	// Add a few, just in case...
	this->pVertices = new cVertex_xyz_rgb[numberOfVertsNeeded];

	int triIndex = 0;
	int vertIndex = 0;
	for ( ; triIndex < this->numberOfTriangles; 
		  triIndex++, vertIndex += 3 )
	{
		// 3 4 132 80 
		int triVert0_index = this->pTriangles[triIndex].vertex_ID_0;
		int triVert1_index = this->pTriangles[triIndex].vertex_ID_1;
		int triVert2_index = this->pTriangles[triIndex].vertex_ID_2;

		cVertex_xyz_rgb V0 = pVertOrig[triVert0_index];
		cVertex_xyz_rgb V1 = pVertOrig[triVert1_index];
		cVertex_xyz_rgb V2 = pVertOrig[triVert2_index];

		this->pVertices[vertIndex + 0] = V0;
		this->pVertices[vertIndex + 1] = V1;
		this->pVertices[vertIndex + 2] = V2;
	}//for ( ; 
	
	// Update the number of vertices
	// HACK: -3 YIKES!!! 
	this->numberOfVertices = vertIndex - 3;	

	return;
}

void cMesh::CalculateNormals( void )
{
	// Go through each triangle
	// calculate normal per vertex (3 vertices)
	// ADD this normal to the corresponding vertex

	// 2nd pass
	// go through all the vertices and normalize them
	// 2nd pass: normalize the normals
	for ( int vertIndex = 0; vertIndex != this->numberOfVertices; vertIndex++ )
	{
		this->pVertices[vertIndex].nx = 0.0f;
		this->pVertices[vertIndex].ny = 0.0f;
		this->pVertices[vertIndex].nz = 0.0f;
	}

	for ( int triIndex = 0; triIndex != this->numberOfTriangles; triIndex++ )
	{
		cTriangle curTri = this->pTriangles[triIndex];

		//Calculate normal for each vertex
		glm::vec3 vertA = glm::vec3( this->pVertices[curTri.vertex_ID_0].x,
			this->pVertices[curTri.vertex_ID_0].y,
			this->pVertices[curTri.vertex_ID_0].z );

		glm::vec3 vertB = glm::vec3( this->pVertices[curTri.vertex_ID_1].x,
			this->pVertices[curTri.vertex_ID_1].y,
			this->pVertices[curTri.vertex_ID_1].z );

		glm::vec3 vertC = glm::vec3( this->pVertices[curTri.vertex_ID_2].x,
			this->pVertices[curTri.vertex_ID_2].y,
			this->pVertices[curTri.vertex_ID_2].z );

		// Cross of A-B and A-C (normal at vertex A)
		glm::vec3 normVec0 = glm::normalize( glm::cross( vertB - vertA, vertC - vertA ) );

		// Cross of B-A and B-C (normal at vertex B)
		glm::vec3 normVec1 = glm::normalize( glm::cross( vertA - vertB, vertC - vertB ) );

		// Cross of C-A and C-B (normal at vertex C)
		glm::vec3 normVec2 = glm::normalize( glm::cross( vertA - vertC, vertB - vertC ) );

		// HACK
		if ( this->name == "sphere" )
		{
			// Invert Normals so it will be iluminated "from the inside"
			normVec0.x = -normVec0.x; //( normVec0.x < 0.0 ? normVec0.x : -normVec0.x );
			normVec0.y = -normVec0.y; // ( normVec0.y < 0.0 ? normVec0.y : -normVec0.y );
			normVec0.z = -normVec0.z; // ( normVec0.z < 0.0 ? normVec0.z : -normVec0.z );
		}

		//normVec0 = glm::normalize( glm::cross( vertB - vertA, vertC - vertA ) );
		normVec1 = normVec0;
		normVec2 = normVec0;

		// Add the values to the current normals (vert A)
		this->pVertices[curTri.vertex_ID_0].nx += normVec0.x;
		this->pVertices[curTri.vertex_ID_0].ny += normVec0.y;
		this->pVertices[curTri.vertex_ID_0].nz += normVec0.z;

		// Add the values to the current normals (vert B)
		this->pVertices[curTri.vertex_ID_1].nx += normVec1.x;
		this->pVertices[curTri.vertex_ID_1].ny += normVec1.y;
		this->pVertices[curTri.vertex_ID_1].nz += normVec1.z;

		// Add the values to the current normals (vert C)
		this->pVertices[curTri.vertex_ID_2].nx += normVec2.x;
		this->pVertices[curTri.vertex_ID_2].ny += normVec2.y;
		this->pVertices[curTri.vertex_ID_2].nz += normVec2.z;

	}

	//// 2nd pass: normalize the normals
	//for (int vertIndex = 0; vertIndex != this->numberOfVertices; vertIndex++)
	//{
	//	glm::vec3 norm = glm::vec3(this->pVertices[vertIndex].nx,
	//								this->pVertices[vertIndex].ny,
	//								this->pVertices[vertIndex].nz);

	//	// It's value DIV length
	//	glm::normalize(norm);

	//	this->pVertices[vertIndex].nx = norm.x;
	//	this->pVertices[vertIndex].ny = norm.y;
	//	this->pVertices[vertIndex].nz = norm.z;
	//}

	return;
}