#ifndef _cMesh_HG_
#define _cMesh_HG_

#include "cVertex_xyz_rgb.h"
#include "cTriangle.h" 

//#define MAX_VERTEX_ARRAY_SIZE 2048
//#define MAX_INDEX_ARRAY_SIZE 2048
//static const int MAX_VERTEX_ARRAY_SIZE = 4096;
//static const int MAX_INDEX_ARRAY_SIZE = 4096;
static const int MAX_VERTEX_ARRAY_SIZE = 8192;
static const int MAX_INDEX_ARRAY_SIZE = 8192;

#include <string>
#include <glm/vec3.hpp>

class cMesh
{
public:
	cMesh();
	~cMesh();

	std::string name; // the friendly name of the mesh

	// Array of vertices 
//	cVertex_xyz_rgb vertices[MAX_VERTEX_ARRAY_SIZE]; // Stack
	cVertex_xyz_rgb* pVertices;						// Heap
	int numberOfVertices;

	// Array of triangles (indices)
//	cTriangle triangles[MAX_INDEX_ARRAY_SIZE];	// Stack
	cTriangle* pTriangles;						// Heap
	int numberOfTriangles;

	// Takes an indexed model and makes just a vertex array model
	// WARNING: Will OVERWRITE the vertex array 
	void FlattenIndexedModel(void);

	// First 2 are the "bounding box" edges
	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;
	// This is Ma - Min for each x,y,z
	glm::vec3 maxExtentXYZ;
	float maxExtent;

	// Scale can be 1.0 div maxExtent;
	float scaleForUnitBBox;

	void CalculateExtents(void);

	void CalculateNormals( void );
};


#endif
