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

	std::string discard;

	if ( ! plyFile.is_open() )
	{	// Didn't open file, so return
		return false;
	}
	// File is open, let's read it

	ReadFileToToken( plyFile, "NumberOfVertices" );
	plyFile >> discard;
	plyFile >> theMesh.numberOfVertices;

	// Allocate the appropriate sized array (+a little bit)
	theMesh.pVertices = new cVertex_xyz_rgb[theMesh.numberOfVertices];
	
	// Read vertices
	for ( int index = 0; index < theMesh.numberOfVertices; index++ )
	{
		//end_header
		//-0.0312216 0.126304 0.00514924 0.850855 0.5 		
		float x, y, z, nx, ny, nz, confidence, intensity;
		std::string sX, sY, sZ;

		plyFile >> z;
		plyFile >> y;
		plyFile >> x;
		plyFile >> nz;
		plyFile >> ny;
		plyFile >> nx;

		theMesh.pVertices[index].x = x;
		theMesh.pVertices[index].y = y;
		theMesh.pVertices[index].z = z; 
		theMesh.pVertices[index].r = 1.0f;
		theMesh.pVertices[index].g = 1.0f;
		theMesh.pVertices[index].b = 1.0f;
		theMesh.pVertices[index].nx = nx;
		theMesh.pVertices[index].ny = ny;
		theMesh.pVertices[index].nz = nz;
	}
	// GO BACK TO THE BEGINING OF THE FILE
	plyFile.seekg( 0 );

	ReadFileToToken( plyFile, "NumberOfTriangles" );
	plyFile >> discard;
	plyFile >> theMesh.numberOfTriangles;

	// Allocate the appropriate sized array (+a little bit)
	theMesh.pTriangles = new cTriangle[theMesh.numberOfTriangles];

	// Load the triangle (or face) information, too
	for( int count = 0; count < theMesh.numberOfTriangles; count++ )
	{
		// 3 164 94 98 
		plyFile >> theMesh.pTriangles[count].vertex_ID_0;
		plyFile >> theMesh.pTriangles[count].vertex_ID_1;
		plyFile >> theMesh.pTriangles[count].vertex_ID_2;
	}

	theMesh.CalculateNormals();

	return true;
}
