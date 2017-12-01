#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>						// C++ cin, cout, etc.
#include <glm/vec3.hpp>					// glm::vec3
#include <glm/vec4.hpp>					// glm::vec4
#include <glm/mat4x4.hpp>				// glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>			// glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
// Add the file stuff library (file stream>
#include <fstream>
#include <sstream>						// "String stream"
#include <istream>
#include <string>
#include <vector>						// smart array, "array" in most languages
#include <random>
#include <chrono>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Utilities.h"
#include "ModelUtilities.h"
#include "cMesh.h"
#include "cShaderManager.h" 
#include "cGameObject.h"
#include "cVAOMeshManager.h"

#include "Physics.h"

#include "cLightManager.h"

// Euclides: Control selected object for movement
int g_GameObjNumber = 0;				// game object vector position number 
int g_LightObjNumber = 0;				// light object vector position

int g_targetShip = 0;
int g_selectedShip = 0;

bool g_lookAtON = false;
bool g_moveRiders = false;
bool g_movingViper = false;

int g_NUMBER_OF_LIGHTS = 9;

bool bIsWireframe = false;

// Remember to #include <vector>...
std::vector< cGameObject* > g_vecGameObjects;

glm::vec3 g_cameraXYZ = glm::vec3( -82.0f, 107.0f, 68.0f );
glm::vec3 g_cameraTarget_XYZ = glm::vec3( 0.0f, 0.0f, 0.0f );

// TODO include camera new code

cVAOMeshManager*	g_pVAOManager = 0;		// or NULL or nullptr

cShaderManager*		g_pShaderManager;		// Heap, new (and delete)
cLightManager*		g_pLightManager;

struct sWindowConfig
{
public:
	int height = 480;
	int width = 640;
	std::string title = "TO BE REPLACED...";
};

struct sGOparameters		// for the Game Objects' input file
{
	std::string meshname;
	int nObjects;
	float x, y, z, scale;
	std::string random;
	float rangeX, rangeY, rangeZ, rangeScale;
};

struct sMeshparameters		// for the Meshes' input file
{
	std::string meshname;
	std::string meshFilename;
};

// Forward declare the Functions
void loadConfigFile( std::string fileName, sWindowConfig& wConfig );
sGOparameters parseObjLine( std::ifstream &source );
void loadObjectsFile( std::string fileName );
sMeshparameters parseMeshLine( std::ifstream &source );
void loadMeshesFile( std::string fileName, GLint ShaderID );
void loadLightObjects();
void PhysicsStep( double deltaTime );

static void error_callback( int error, const char* description )
{
	fprintf( stderr, "Error: %s\n", description );
}

// All the keyboard input logic is here
static void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{	
	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose( window, GLFW_TRUE );

	//// Change object in g_GameObject
	//if ( key == GLFW_KEY_ENTER && action == GLFW_PRESS )
	//{
	//	if ( bIsWireframe ) bIsWireframe = false;
	//	else bIsWireframe = true;
	//}

	switch( key )
	{
		
	case GLFW_KEY_UP:		// Up arrow
		break;
	case GLFW_KEY_DOWN:		// Down arrow
		break;
	case GLFW_KEY_LEFT:		// Left arrow
		break;
	case GLFW_KEY_RIGHT:	// Right arrow
		break;
	case GLFW_KEY_LEFT_BRACKET:		// [{ key
		break;
	case GLFW_KEY_RIGHT_BRACKET:		// ]} key
		break;
	}

	// Change Camera Position
	const float CAMERAMOVEMENT = 1.0f;
	switch( key )
	{
	case GLFW_KEY_A:		// Left
		g_cameraXYZ.x -= CAMERAMOVEMENT;
		break;
	case GLFW_KEY_D:		// Right
		g_cameraXYZ.x += CAMERAMOVEMENT;
		break;
	case GLFW_KEY_W:		// Forward (along z)
		g_cameraXYZ.z += CAMERAMOVEMENT;
		break;
	case GLFW_KEY_S:		// Backwards (along z)
		g_cameraXYZ.z -= CAMERAMOVEMENT;
		break;
	case GLFW_KEY_Q:		// "Down" (along y axis)
		g_cameraXYZ.y -= CAMERAMOVEMENT;
		break;
	case GLFW_KEY_E:		// "Up" (along y axis)
		g_cameraXYZ.y += CAMERAMOVEMENT;
		break;
	}

	if( key == GLFW_KEY_P && action == GLFW_PRESS )
	{	
		bool newTarget = false;
		int newTargetPos = ::g_targetShip;
		while( !newTarget )
		{	
			newTargetPos++;
			if( newTargetPos >= ::g_vecGameObjects.size() ) newTargetPos = 0;
			if( ::g_vecGameObjects[newTargetPos]->meshName == "viper" ||
				::g_vecGameObjects[newTargetPos]->meshName == "raider" )
			{
				::g_targetShip = newTargetPos;
				newTarget = true;
			}
		}

		::g_cameraTarget_XYZ = glm::vec3( ::g_vecGameObjects[::g_targetShip]->position.x,
										::g_vecGameObjects[::g_targetShip]->position.y,
										::g_vecGameObjects[::g_targetShip]->position.z );
	}

	if( key == GLFW_KEY_C && action == GLFW_PRESS )
	{
		if( !::g_lookAtON )
		{
			//::g_cameraXYZ = glm::vec3( 0.0f, 17.0f, 30.0f );
			::g_cameraXYZ = glm::vec3( 0.0f, 9.0f, 20.0f );
			g_lookAtON = true;
		}
		else
		{
			::g_cameraTarget_XYZ = glm::vec3( 0.0f, 0.0f, 0.0f );
			::g_cameraXYZ = glm::vec3( -82.0f, 107.0f, 68.0f );
			::g_lookAtON = false;
		}
		
	}

	if( key == GLFW_KEY_SPACE && action == GLFW_PRESS )
	{
		if( ::g_moveRiders == false ) ::g_moveRiders = true;
		else ::g_moveRiders = false;

		for( int i = 0; i != ::g_vecGameObjects.size(); i++ )
		{
			if( ::g_vecGameObjects[i]->meshName == "raider" )
			{
				if( ::g_moveRiders == true )
					::g_vecGameObjects[i]->vel = glm::vec3( 0.0f, 0.0f, 3.0f );
				else
					::g_vecGameObjects[i]->vel = glm::vec3( 0.0f, 0.0f, 0.0f );

			}
		}
	}

	if( key == GLFW_KEY_V && action == GLFW_PRESS )
	{
		bool newTarget = false;
		int newTargetPos = ::g_selectedShip;
		while( !newTarget )
		{
			newTargetPos++;
			if( newTargetPos >= ::g_vecGameObjects.size() ) newTargetPos = 0;
			if( ::g_vecGameObjects[newTargetPos]->meshName == "viper" )
			{
				::g_selectedShip = newTargetPos;
				newTarget = true;
			}
		}
		::g_vecGameObjects[::g_selectedShip]->position = glm::vec3( -14.0f, -0.1f, 80.0f );
		::g_cameraTarget_XYZ = glm::vec3( -13.7f, 0.5f, 0.0f );
		::g_cameraXYZ = ::g_vecGameObjects[::g_selectedShip]->position;
		::g_cameraXYZ.y += 0.1f;
	}

	if( key == GLFW_KEY_ENTER &&  action == GLFW_PRESS )
	{
		::g_vecGameObjects[::g_selectedShip]->vel = glm::vec3( 0.0f, 0.0f, -2.0f );
		::g_vecGameObjects[::g_selectedShip]->isMoving = true;
		::g_movingViper = true;
	}
	if( key == GLFW_KEY_ENTER &&  action == GLFW_RELEASE )
	{
		::g_vecGameObjects[::g_selectedShip]->vel = glm::vec3( 0.0f, 0.0f, 0.0f );
		::g_vecGameObjects[::g_selectedShip]->isMoving = false;
		::g_movingViper = false;
	}


	// Change Camera
	switch ( key )
	{
	case GLFW_KEY_1:
		//g_LightObjNumber = 0;
		::g_cameraXYZ = glm::vec3( -82.0f, 107.0f, 68.0f );
		break;
	case GLFW_KEY_2:
		//g_LightObjNumber = 1;
		::g_cameraXYZ = glm::vec3( -100.0f, 65.0f, 115.0f );
		break;
	case GLFW_KEY_3:
		::g_cameraXYZ = glm::vec3( -23.0f, 12.0f, 131.0f );
		break;
	case GLFW_KEY_4:
		::g_cameraXYZ = glm::vec3( -14.0f, 0.0f, 68.0f );
		break;
	case GLFW_KEY_5:
		::g_cameraXYZ = glm::vec3( 14.0f, 0.0f, 68.0f );
		break;
	case GLFW_KEY_6:
		g_LightObjNumber = 5;
		break;
	case GLFW_KEY_7:
		g_LightObjNumber = 6;
		break;
	case GLFW_KEY_8:
		g_LightObjNumber = 7;
		break;
	case GLFW_KEY_9:
		g_LightObjNumber = 8;
		break;
	}

	return;
}

void UpdateCameraPosition( void )
{
	for( int i = 0; i != ::g_vecGameObjects.size(); i++ )
	{
		if( ::g_vecGameObjects[i]->isMoving == true )
		{
			::g_cameraXYZ = ::g_vecGameObjects[i]->position;
			::g_cameraXYZ.y += 0.1f;
		}
	}
}

int main( void )
{
	GLFWwindow* window;
	//GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location; //vpos_location, vcol_location;
	glfwSetErrorCallback( error_callback );

	// Other uniforms:
	GLint uniLoc_materialDiffuse_r = -1;
	GLint uniLoc_materialDiffuse_g = -1;
	GLint uniLoc_materialDiffuse_b = -1;
	GLint uniLoc_materialDiffuse_a = -1;
	GLint uniLoc_materialAmbient = -1;
	GLint uniLoc_ambientToDiffuseRatio = -1; 	// Maybe	// 0.2 or 0.3
	GLint uniLoc_materialSpecular = -1;  // rgb = colour of HIGHLIGHT only
										 // w = shininess of the 
	GLint uniLoc_eyePosition = -1;	// Camera position
	GLint uniLoc_mModel = -1;
	GLint uniLoc_mView = -1;
	GLint uniLoc_mProjection = -1;

	if( !glfwInit() )
		exit( EXIT_FAILURE );

	sWindowConfig wConfig;

	loadConfigFile( "config.txt", wConfig );
	loadObjectsFile( "objects.txt" );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );

	window = glfwCreateWindow( wConfig.width, wConfig.height,
		wConfig.title.c_str(),
		NULL, // glfwGetPrimaryMonitor(), //
		NULL );
	if( !window )
	{
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	glfwSetKeyCallback( window, key_callback );
	glfwMakeContextCurrent( window );
	gladLoadGLLoader( ( GLADloadproc )glfwGetProcAddress );
	glfwSwapInterval( 1 );

	std::cout << glGetString( GL_VENDOR ) << " "
		<< glGetString( GL_RENDERER ) << ", "
		<< glGetString( GL_VERSION ) << std::endl;
	std::cout << "Shader language version: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;

	::g_pShaderManager = new cShaderManager();

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";
	fragShader.fileName = "simpleFrag.glsl";

	::g_pShaderManager->setBasePath( "assets//shaders//" );

	// Shader objects are passed by reference so that
	//	we can look at the results if we wanted to. 
	if( !::g_pShaderManager->createProgramFromFile(
		"mySexyShader", vertShader, fragShader ) )
	{
		std::cout << "Oh no! All is lost!!! Blame Loki!!!" << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
		// Should we exit?? 
		return -1;
		//		exit(
	}
	std::cout << "The shaders compiled and linked OK" << std::endl;

	//Load models
	::g_pVAOManager = new cVAOMeshManager();

	GLint sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );

	loadMeshesFile( "meshlist.txt", sexyShaderID );

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );

	// Get the uniform locations for this shader
	mvp_location = glGetUniformLocation( currentProgID, "MVP" );		// program, "MVP");
	uniLoc_materialDiffuse_r = glGetUniformLocation( currentProgID, "materialDiffuse_r" );
	uniLoc_materialDiffuse_g = glGetUniformLocation( currentProgID, "materialDiffuse_g" );
	uniLoc_materialDiffuse_b = glGetUniformLocation( currentProgID, "materialDiffuse_b" );
	uniLoc_materialDiffuse_a = glGetUniformLocation( currentProgID, "materialDiffuse_a" );
	uniLoc_materialAmbient = glGetUniformLocation( currentProgID, "materialAmbient" );
	uniLoc_ambientToDiffuseRatio = glGetUniformLocation( currentProgID, "ambientToDiffuseRatio" );
	uniLoc_materialSpecular = glGetUniformLocation( currentProgID, "materialSpecular" );
	uniLoc_eyePosition = glGetUniformLocation( currentProgID, "eyePosition" );

	uniLoc_mModel = glGetUniformLocation( currentProgID, "mModel" );
	uniLoc_mView = glGetUniformLocation( currentProgID, "mView" );
	uniLoc_mProjection = glGetUniformLocation( currentProgID, "mProjection" );

	::g_pLightManager = new cLightManager();

	::g_pLightManager->CreateLights( g_NUMBER_OF_LIGHTS );	// There are 10 lights in the shader
	::g_pLightManager->LoadShaderUniformLocations( currentProgID );

	// Change ZERO (the SUN) light position
	::g_pLightManager->vecLights[0].position = glm::vec3( -500.0f, 500.0f, 500.0f );
	::g_pLightManager->vecLights[0].diffuse = glm::vec3( 1.0f, 1.0f, 1.0f );
	::g_pLightManager->vecLights[0].ambient = glm::vec3( 0.8f, 0.8f, 0.8f );
	::g_pLightManager->vecLights[0].attenuation.y = 0.003f;		// Change the linear attenuation

	// ADD 8 MORE LIGHTS========================================
	{
		// ENGINE LIGHTS
		::g_pLightManager->vecLights[1].position = glm::vec3( -6.5f, 4.5f, 78.0f );
		::g_pLightManager->vecLights[2].position = glm::vec3( 6.5f,  4.5f, 78.0f );
		::g_pLightManager->vecLights[3].position = glm::vec3( -5.5f, -2.8f, 75.0f );
		::g_pLightManager->vecLights[4].position = glm::vec3( 5.5f,  -2.8f, 75.0f );
		::g_pLightManager->vecLights[1].diffuse = glm::vec3( 0.5f, 0.5f, 1.0f );
		::g_pLightManager->vecLights[2].diffuse = glm::vec3( 0.5f, 0.5f, 1.0f );
		::g_pLightManager->vecLights[3].diffuse = glm::vec3( 0.5f, 0.5f, 1.0f );
		::g_pLightManager->vecLights[4].diffuse = glm::vec3( 0.5f, 0.5f, 1.0f );
		::g_pLightManager->vecLights[1].attenuation.y = 0.45;
		::g_pLightManager->vecLights[2].attenuation.y = 0.45;
		::g_pLightManager->vecLights[3].attenuation.y = 0.45;
		::g_pLightManager->vecLights[4].attenuation.y = 0.45;
		
		// LANDING BAY LEFT
		::g_pLightManager->vecLights[5].position = glm::vec3( -14.5f, 0.75f, 23.0f );
		::g_pLightManager->vecLights[6].position = glm::vec3( -14.5f, 0.75f, 47.0f );
		::g_pLightManager->vecLights[5].diffuse = glm::vec3( 1.0f, 1.0f, 1.0f );
		::g_pLightManager->vecLights[6].diffuse = glm::vec3( 1.0f, 1.0f, 1.0f );

		// LANDING BAY RIGHT
		::g_pLightManager->vecLights[7].position = glm::vec3( 14.5f, 0.75f, 23.0f );
		::g_pLightManager->vecLights[8].position = glm::vec3( 14.5f, 0.75f, 47.0f );
		::g_pLightManager->vecLights[7].diffuse = glm::vec3( 1.0f, 1.0f, 1.0f );
		::g_pLightManager->vecLights[8].diffuse = glm::vec3( 1.0f, 1.0f, 1.0f );

	}
	//=========================================================

	//loadLightObjects();

	glEnable( GL_DEPTH );

	// Gets the "current" time "tick" or "step"
	double lastTimeStep = glfwGetTime();

	// Main game or application loop
	while( !glfwWindowShouldClose( window ) )
	{
		float ratio;
		int width, height;
		glm::mat4x4 m, p, mvp;			//  mat4x4 m, p, mvp;

		glfwGetFramebufferSize( window, &width, &height );
		ratio = width / ( float )height;
		glViewport( 0, 0, width, height );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Update all the light uniforms...
		// (for the whole scene)
		::g_pLightManager->CopyLightInformationToCurrentShader(); 

		// "Draw scene" loop
		//for ( int index = 0; index != MAXNUMBEROFGAMEOBJECTS; index++ )

		unsigned int sizeOfVector = ::g_vecGameObjects.size();
		for ( int index = 0; index != sizeOfVector; index++ )
		{
			// Is there a game object? 
			if ( ::g_vecGameObjects[index] == 0 )
			{	// Nothing to draw
				continue;		// Skip all for loop code and go to next
			}

			// Was near the draw call, but we need the mesh name
			std::string meshToDraw = ::g_vecGameObjects[index]->meshName;		//::g_GameObjects[index]->meshName;

			sVAOInfo VAODrawInfo;
			if ( ::g_pVAOManager->lookupVAOFromName( meshToDraw, VAODrawInfo ) == false )
			{	// Didn't find mesh
				continue;
			}

			// Change Light Objects position based on light position
			// The game object sphere that "contains" the light follows the light
			if ( ::g_vecGameObjects[index]->bIsLight == true )
			{
				int lightIndex = ::g_vecGameObjects[index]->myLight;
				::g_vecGameObjects[index]->position = ::g_pLightManager->vecLights[lightIndex].position;
				::g_vecGameObjects[index]->diffuseColour = glm::vec4( ::g_pLightManager->vecLights[lightIndex].diffuse, 1.0f );
			}

			// There IS something to draw
			m = glm::mat4x4( 1.0f );	//		mat4x4_identity(m);

			glm::mat4 matRreRotZ = glm::mat4x4( 1.0f );
			matRreRotZ = glm::rotate( matRreRotZ, ::g_vecGameObjects[index]->orientation.z,
				glm::vec3( 0.0f, 0.0f, 1.0f ) );
			m = m * matRreRotZ;

			glm::mat4 trans = glm::mat4x4( 1.0f );
			trans = glm::translate( trans,
				::g_vecGameObjects[index]->position );
			m = m * trans;

			glm::mat4 matPostRotZ = glm::mat4x4( 1.0f );
			matPostRotZ = glm::rotate( matPostRotZ, ::g_vecGameObjects[index]->orientation2.z,
				glm::vec3( 0.0f, 0.0f, 1.0f ) );
			m = m * matPostRotZ;

			// IF the game object isn't a light object, it will rotate as normal
			if ( !::g_vecGameObjects[index]->bIsLight )
			{
				::g_vecGameObjects[index]->orientation2.x += ::g_vecGameObjects[index]->rotation.x;
				::g_vecGameObjects[index]->orientation2.y += ::g_vecGameObjects[index]->rotation.y;
				::g_vecGameObjects[index]->orientation2.z += ::g_vecGameObjects[index]->rotation.z;
			}

			glm::mat4 matPostRotY = glm::mat4x4( 1.0f );
			matPostRotY = glm::rotate( matPostRotY, ::g_vecGameObjects[index]->orientation2.y,
				glm::vec3( 0.0f, 1.0f, 0.0f ) );
			m = m * matPostRotY;

			glm::mat4 matPostRotX = glm::mat4x4( 1.0f );
			matPostRotX = glm::rotate( matPostRotX, ::g_vecGameObjects[index]->orientation2.x,
				glm::vec3( 1.0f, 0.0f, 0.0f ) );
			m = m * matPostRotX;
			// TODO: add the other rotation matrix (i.e. duplicate code above)

			float finalScale = ::g_vecGameObjects[index]->scale;

			glm::mat4 matScale = glm::mat4x4( 1.0f );
			matScale = glm::scale( matScale,
				glm::vec3( finalScale,
					finalScale,
					finalScale ) );
			m = m * matScale;

			// Change from an orthographic view to a perspective view
			p = glm::perspective( 0.6f,			// FOV
				ratio,		// Aspect ratio
				0.1f,			// Near (as big as possible)
				1000.0f );	// Far (as small as possible)

			// View or "camera" matrix
			glm::mat4 v = glm::mat4( 1.0f );	// identity

			if( ::g_movingViper = true )
			{
				UpdateCameraPosition();
			}


			v = glm::lookAt( g_cameraXYZ,			// "eye" or "camera" position
				g_cameraTarget_XYZ,					// "At" or "target"							 
				//glm::vec3( 0.0f, 1.0f, 0.0f ) );	// "up" vector
				glm::vec3( 0.0f, 1.0f, 0.0f ) );	// "up" vector

			//mat4x4_mul(mvp, p, m);
			mvp = p * v * m;			// This way (sort of backwards)

			::g_pShaderManager->useShaderProgram( "mySexyShader" );
			GLint shaderID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );
			
			glUniformMatrix4fv( uniLoc_mModel, 1, GL_FALSE,
				( const GLfloat* )glm::value_ptr( m ) );
			glUniformMatrix4fv( uniLoc_mView, 1, GL_FALSE,
				( const GLfloat* )glm::value_ptr( v ) );
			glUniformMatrix4fv( uniLoc_mProjection, 1, GL_FALSE,
				( const GLfloat* )glm::value_ptr( p ) );

			glm::mat4 mWorldInTranpose = glm::inverse( glm::transpose( m ) );
			
			glUniform1f( uniLoc_materialDiffuse_r,
				::g_vecGameObjects[index]->diffuseColour.r );
			glUniform1f( uniLoc_materialDiffuse_g,
				::g_vecGameObjects[index]->diffuseColour.g );
			glUniform1f( uniLoc_materialDiffuse_b,
				::g_vecGameObjects[index]->diffuseColour.b );
			glUniform1f( uniLoc_materialDiffuse_a,
				::g_vecGameObjects[index]->diffuseColour.a );
			//... and all the other object material colours
			

			//		glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );

			// Check if the bIsWireframe is true and set PolygonMonde to GL_LINE
			if ( bIsWireframe )	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			else glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );	// Default
			
			glEnable( GL_DEPTH_TEST );	// Test for Z and store in z buffer
			glCullFace( GL_BACK );		// Draw only the normals that are "front-facing"

			glBindVertexArray( VAODrawInfo.VAO_ID );

			glDrawElements( GL_TRIANGLES,
				VAODrawInfo.numberOfIndices,
				GL_UNSIGNED_INT,	// 32 bit int
				0 ); // g_numberOfVertices

			glBindVertexArray( 0 );

		}//for ( int index = 0...


		std::stringstream ssTitle;
		ssTitle << "Graphics Midterm: Battlestar Galactica - "
			<< "Camera (xyz): "
			<< g_cameraXYZ.x << ", "
			<< g_cameraXYZ.y << ", "
			<< g_cameraXYZ.z;

		glfwSetWindowTitle( window, ssTitle.str().c_str() );

		glfwSwapBuffers( window );
		glfwPollEvents();

		// Essentially the "frame time"
		// Now many seconds that have elapsed since we last checked
		double curTime = glfwGetTime();
		double deltaTime = curTime - lastTimeStep;
		
		// Physics Calculation
		PhysicsStep( deltaTime );

		lastTimeStep = curTime;

	}// while ( ! glfwWindowShouldClose(window) )


	glfwDestroyWindow( window );
	glfwTerminate();

	// 
	delete ::g_pShaderManager;
	delete ::g_pVAOManager;

	//    exit(EXIT_SUCCESS);
	return 0;
}

//Load Config.txt
void loadConfigFile( std::string fileName, sWindowConfig& wConfig )
{
	// TODO change this config formating
	std::ifstream infoFile( fileName );
	if( !infoFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}
	else
	{	// File DID open, so read it... 
		std::string a;

		infoFile >> a;	// "Game"	//std::cin >> a;
		infoFile >> a;	// "Config"
		infoFile >> a;	// "width"
		infoFile >> wConfig.width;	// 1080
		infoFile >> a;	// "height"
		infoFile >> wConfig.height;	// 768
		infoFile >> a;		// Title_Start

		std::stringstream ssTitle;		// Inside "sstream"
		bool bKeepReading = true;
		do
		{
			infoFile >> a;		// Title_End??
			if( a != "Title_End" )
			{
				ssTitle << a << " ";
			}
			else
			{	// it IS the end! 
				bKeepReading = false;
				wConfig.title = ssTitle.str();
			}
		} while( bKeepReading );
	}
}

// Generate real random numbers
float generateRandomNumber( float min, float max )
{

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

	std::default_random_engine generator( seed );
	std::uniform_real_distribution<float> distribution( min, max );

	float randomNumber = 0.0;

	randomNumber = distribution( generator );
	return randomNumber;

}

bool CheckDistance( glm::vec3 position, glm::vec3 fromPoint, float minDistance, float maxDistance )
{
	float thisDistance = 0.0f;
	thisDistance = glm::distance( position, fromPoint );

	if( thisDistance >= minDistance || thisDistance <= maxDistance ) return true;

	return false;
}

glm::vec3 GetRandomPosition()
{
	glm::vec3 center = glm::vec3( 0.0f );
	glm::vec3 newPosition = glm::vec3( 0.0f );
	bool validPosition = false;

	while( !validPosition )
	{
		newPosition = glm::vec3( generateRandomNumber( -200, 200 ),
								 generateRandomNumber( -200, 200 ),
								 generateRandomNumber( -200, 200 ) );
		validPosition = CheckDistance( newPosition, center, 30.0f, 100.0f );
	}

	return newPosition;
}


//Load objects.txt
void loadObjectsFile( std::string fileName )
{
	//sGOparameters sGOpar;
	std::vector <sGOparameters> allObjects;

	std::ifstream objectsFile( fileName );
	if( !objectsFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}
	else
	{	// File DID open, so loop through the file and pushback to structure
		while( !objectsFile.eof() && objectsFile.is_open() ) {
			allObjects.push_back( parseObjLine( objectsFile ) );
		}
		objectsFile.close();  //Closing "costfile.txt"
	}

	for( int index = 0; index != allObjects.size(); index++ )
	{
		// Check, Number of Objects must be at least 1
		if( allObjects[index].nObjects == 0 ) allObjects[index].nObjects = 1;

		// Create the number of gameObjects specified in the file for each line 
		for ( int i = 0; i != allObjects[index].nObjects; i++ )
		{
			// Create a new GO
			cGameObject* pTempGO = new cGameObject();

			pTempGO->meshName = allObjects[index].meshname; // Set the name of the mesh

			pTempGO->diffuseColour = glm::vec4( 0.5f, 0.5f, 0.5f, 1.0f );
			pTempGO->rotation = glm::vec3( 0.0f );
			
			// SOME OBJECTS ARE RANDOMLY PLACED WHEN RANDOM=TRUE ON FILE
			if ( allObjects[index].random == "true" )
			{  
				if( allObjects[index].meshname == "viper" )
				{   // Vipers have their on random placement
					float random = generateRandomNumber( -5000.0f, 5000.0f );

					if ( random < 0 )
						pTempGO->position.x = generateRandomNumber( -30.0f, -20.0f );
					else 
						pTempGO->position.x = generateRandomNumber( 20.0f, 30.0f );
					
					pTempGO->position.y = generateRandomNumber( 0.0f, 10.0f );
					pTempGO->position.z = generateRandomNumber( -10.0f, 20.0f );

					pTempGO->orientation2.y = 180.0f * ( M_PI / 180 );

				}
				else if( allObjects[index].meshname == "asteroid1" ||
					allObjects[index].meshname == "asteroid2" ||
					allObjects[index].meshname == "asteroid3" ||
					allObjects[index].meshname == "asteroid4" )
				{   // Asteroids have their on random placement
					pTempGO->position = GetRandomPosition();
					pTempGO->rotation.x = generateRandomNumber( -0.05f, 0.05f );
					pTempGO->rotation.y = generateRandomNumber( -0.05f, 0.05f );
					pTempGO->rotation.z = generateRandomNumber( -0.05f, 0.05f );
				}
				else
				{
					pTempGO->position.x = generateRandomNumber( -allObjects[index].rangeX, allObjects[index].rangeX );
					pTempGO->position.y = generateRandomNumber( -allObjects[index].rangeY, allObjects[index].rangeY );
					pTempGO->position.z = generateRandomNumber( -allObjects[index].rangeZ, allObjects[index].rangeZ );
				}
				pTempGO->scale = allObjects[index].rangeScale;
			}
			else
			{   // position and scale are fixed
				pTempGO->position.x = allObjects[index].x;
				pTempGO->position.y = allObjects[index].y;
				pTempGO->position.z = allObjects[index].z;
				pTempGO->scale = allObjects[index].scale;
			}
			// NO VELOCITY
			pTempGO->vel = glm::vec3( 0.0f );

			::g_vecGameObjects.push_back( pTempGO );
		}
	}
}

// Parse the file line to fit into the structure
sGOparameters parseObjLine( std::ifstream &source ) {

	sGOparameters sGOpar;

	//Scanning a line from the file
	source >> sGOpar.meshname >> sGOpar.nObjects
		>> sGOpar.x >> sGOpar.y >> sGOpar.z >> sGOpar.scale
		>> sGOpar.random
		>> sGOpar.rangeX >> sGOpar.rangeY >> sGOpar.rangeZ
		>> sGOpar.rangeScale;


	return sGOpar;
}

//Load meshlist.txt
void loadMeshesFile( std::string fileName, GLint ShaderID )
{
	std::vector <sMeshparameters> allMeshes;

	std::ifstream objectsFile( fileName );
	if( !objectsFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}
	else
	{	// File DID open, so loop through the file and pushback to structure
		while( !objectsFile.eof() && objectsFile.is_open() ) {
			allMeshes.push_back( parseMeshLine( objectsFile ) );
		}
		objectsFile.close();  //Closing "costfile.txt"
	}

	for( int index = 0; index != allMeshes.size(); index++ )
	{
		cMesh testMesh;
		testMesh.name = allMeshes[index].meshname;
		if( !LoadPlyFileIntoMesh( allMeshes[index].meshFilename, testMesh ) )
		{
			std::cout << "Didn't load model" << std::endl;
			// do something??
		}
		if( !::g_pVAOManager->loadMeshIntoVAO( testMesh, ShaderID ) )
		{
			std::cout << "Could not load mesh into VAO" << std::endl;
		}
	}	
}

// Parse the file line to fit into the structure
sMeshparameters parseMeshLine( std::ifstream &source ) {

	sMeshparameters sMeshpar;

	//Scanning a line from the file
	source >> sMeshpar.meshname >> sMeshpar.meshFilename;

	return sMeshpar;
}

void loadLightObjects()
{
	for ( int index = 0; index < g_NUMBER_OF_LIGHTS; index++ )
	{
		// Create a new GO
		cGameObject* pTempGO = new cGameObject();

		pTempGO->meshName = "sphere"; // Set the name of the mesh
		
		// position is based on light position
		pTempGO->position = ::g_pLightManager->vecLights[index].position;

		if ( index == 0 ) pTempGO->scale = 3.0f;
		else pTempGO->scale = 1.0f;

		// Each light is initially white
		pTempGO->diffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );

		pTempGO->bIsLight = true;
		pTempGO->myLight = index;

		::g_vecGameObjects.push_back( pTempGO );
	}
}

// Update the world 1 "step" in time
void PhysicsStep( double deltaTime )
{
	// Distance                          m
	// Velocity = distance / time		 m/s
	// Accleration = velocity / time     m/s/s

	// Distance = time * velocity
	// velocity = time * acceleration

	// NO GRAVITY
	// const glm::vec3 GRAVITY = glm::vec3( 0.0f, -2.0f, 0.0f );

	// Identical to the 'render' (drawing) loop
	for ( int index = 0; index != ::g_vecGameObjects.size(); index++ )
	{
		cGameObject* pCurGO = ::g_vecGameObjects[index];

		// Is this object to be updated?
		if ( !pCurGO->bIsUpdatedInPhysics )
		{	// DON'T update this
			continue;		// Skip everything else in the for
		}

		// Explicity Euler integration (RK4)
		// New position is based on velocity over time
		glm::vec3 deltaPosition = ( float )deltaTime * pCurGO->vel;
		pCurGO->position += deltaPosition;

		if( pCurGO->meshName == "viper" )
		{
			if( pCurGO->isMoving == true )
			{
				if( pCurGO->position.z <= 23.0f )
				{
					pCurGO->position.z = 23.0f;
					pCurGO->vel = glm::vec3( 0.0f );
					pCurGO->isMoving = false;
					::g_movingViper = false;
				}
			}
		}

		// NO ACCELERATION 
		//// New velocity is based on acceleration over time
		//glm::vec3 deltaVelocity = ( ( float )deltaTime * pCurGO->accel )
		//	+ ( ( float )deltaTime * GRAVITY );

		//break;

	}//for ( int index...

	return;
}