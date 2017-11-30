#include "ModelUtilities.h" 


void ReadFileToToken( std::ifstream &file, std::string token )
{
	bool bKeepReading = true;
	std::string garbage;
	do
	{
		file >> garbage;		// Title_End??
		if ( garbage == token )
		{
			return;
		}
	}while( bKeepReading );
	return;
}



// Takes a file name, loads a mesh
bool LoadPlyFileIntoMesh( std::string filename, cMesh &theMesh )
{
	// Load the vertices
	// c_str() changes a string to a "c style char* string"
	std::ifstream plyFile( filename.c_str() );

	if ( ! plyFile.is_open() )
	{	// Didn't open file, so return
		return false;
	}
	// File is open, let's read it

	ReadFileToToken( plyFile, "vertex" );
//	int numVertices = 0;
	plyFile >> theMesh.numberOfVertices;

	ReadFileToToken( plyFile, "face" );
//	int numTriangles = 0;
	plyFile >> theMesh.numberOfTriangles;		

	ReadFileToToken( plyFile, "end_header" );

	// Allocate the appropriate sized array (+a little bit)
	theMesh.pVertices = new cVertex_xyz_rgb[theMesh.numberOfVertices];
	theMesh.pTriangles = new cTriangle[theMesh.numberOfTriangles];

	// Read vertices
	for ( int index = 0; index < theMesh.numberOfVertices; index++ )
	{
		//end_header
		//-0.0312216 0.126304 0.00514924 0.850855 0.5 		
		float x, y, z, confidence, intensity;

		plyFile >> x;
		plyFile >> y;
		plyFile >> z;
//		plyFile >> confidence;
//		plyFile >> intensity;

		theMesh.pVertices[index].x = x;	// vertices[index].x = x;
		theMesh.pVertices[index].y = y;	// vertices[index].y = y;
		theMesh.pVertices[index].z = z; 
		theMesh.pVertices[index].r = 1.0f;	// vertices[index].g = 1.0f;
		theMesh.pVertices[index].g = 1.0f;	// vertices[index].b = 1.0f;
		theMesh.pVertices[index].b = 1.0f;	// vertices[index].r = 1.0f;
	}

	// Load the triangle (or face) information, too
	for ( int count = 0; count < theMesh.numberOfTriangles; count++ )
	{
		// 3 164 94 98 
		int discard = 0;
		plyFile >> discard;									// 3
		plyFile >> theMesh.pTriangles[count].vertex_ID_0;	// 164
		plyFile >> theMesh.pTriangles[count].vertex_ID_1;	// 94
		plyFile >> theMesh.pTriangles[count].vertex_ID_2;	// 98
	}

	theMesh.CalculateNormals();

	return true;
}
