#include "cLightManager.h"
#include <vector>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

cLight::cLight()
{
	this->position = glm::vec3( 0.0f, 0.0f, 0.0f );

	this->diffuse = glm::vec3( 1.0f, 1.0f, 1.0f );
	this->ambient = glm::vec3( 0.2f, 0.2f, 0.2f );
	// Colour (xyz), intensity (w)
	this->specular = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	// x = constant, y = linear, z = quadratic
	this->attenuation = glm::vec3( 0.0f );
	this->attenuation.x = 0.0f;	// Constant atten
	this->attenuation.y = 1.0f;	// Linear
	this->attenuation.z = 0.0f;	// Quadratic

	this->direction = glm::vec3( 0.0f );

	// x = type, y = distance cut-off, z angle1, w = angle2
	this->typeParams = glm::vec4( 0.0f );
	// Set distance to infinity
	// (i.e. at this point, the light won't be calculated)
	this->typeParams.y = 1000000.0f;		// Huge number

	this->shaderlocID_position = -1;
	this->shaderlocID_diffuse = -1;
	this->shaderlocID_ambient = -1;
	this->shaderlocID_specular = -1;
	this->shaderlocID_attenuation = -1;
	this->shaderlocID_direction = -1;
	this->shaderlocID_typeParams = -1;

	return;
}

cLightManager::cLightManager()
{

	return;
}

cLightManager::~cLightManager()
{

	return;
}

void cLightManager::CreateLights( int numberOfLights, bool bKeepOldValues /*=true*/ )
{
	int howManyLightsToAdd = numberOfLights - this->vecLights.size();
	howManyLightsToAdd = abs( howManyLightsToAdd );
	// Resize the vector
	this->vecLights.resize( howManyLightsToAdd, cLight() );

	// If we DON'T want the original values, then clear the old ones
	cLight tempLight;
	for( int index = 0; index != this->vecLights.size(); index++ )
	{
		this->vecLights[index] = tempLight;
	}

	return;
}

std::string genUniName( int lightIndex, std::string paramName )
{
	// uniform vec4 myLight[0].position;
	std::stringstream ssUniName;
	ssUniName << "myLight[" << lightIndex << "]." << paramName;
	return ssUniName.str();
}


void cLightManager::LoadShaderUniformLocations( int shaderID )
{
	//struct sLightDesc {
	//vec4 position;
	//vec4 diffuse;
	//vec4 ambient;
	//vec4 specular;		// Colour (xyz), intensity (w)
	//vec4 attenuation;	// x = constant, y = linear, z = quadratic
	//vec4 direction;
	//vec4 typeParams;	// x = type, y = distance cut-off
	//                    // z angle1, w = angle2
	//};
	//const int NUMBEROFLIGHTS = 10;
	//uniform sLightDesc myLight[NUMBEROFLIGHTS];

	for( int index = 0; index != this->vecLights.size(); index++ )
	{
		//"myLight[0].position"
		this->vecLights[index].shaderlocID_position
			= glGetUniformLocation( shaderID, genUniName( index, "position" ).c_str() );

		this->vecLights[index].shaderlocID_diffuse = glGetUniformLocation( shaderID, genUniName( index, "diffuse" ).c_str() );
		this->vecLights[index].shaderlocID_ambient = glGetUniformLocation( shaderID, genUniName( index, "ambient" ).c_str() );
		this->vecLights[index].shaderlocID_specular = glGetUniformLocation( shaderID, genUniName( index, "specular" ).c_str() );
		this->vecLights[index].shaderlocID_attenuation = glGetUniformLocation( shaderID, genUniName( index, "attenuation" ).c_str() );
		this->vecLights[index].shaderlocID_direction = glGetUniformLocation( shaderID, genUniName( index, "direction" ).c_str() );
		this->vecLights[index].shaderlocID_typeParams = glGetUniformLocation( shaderID, genUniName( index, "typeParams" ).c_str() );
	}
	return;
}

void cLightManager::CopyLightInformationToCurrentShader( void )
{
	for( int index = 0; index != this->vecLights.size(); index++ )
	{
		cLight& pCurLight = this->vecLights[index];

		glUniform4f( pCurLight.shaderlocID_position,
			pCurLight.position.x,
			pCurLight.position.y,
			pCurLight.position.z,
			1.0f );

		glUniform4f( pCurLight.shaderlocID_diffuse,
			pCurLight.diffuse.r,
			pCurLight.diffuse.g,
			pCurLight.diffuse.b,
			1.0f );

		glUniform4f( pCurLight.shaderlocID_ambient,
			pCurLight.ambient.r,
			pCurLight.ambient.g,
			pCurLight.ambient.b,
			1.0f );

		glUniform4f( pCurLight.shaderlocID_specular,
			pCurLight.specular.r,
			pCurLight.specular.g,
			pCurLight.specular.b,
			pCurLight.specular.w );

		glUniform4f( pCurLight.shaderlocID_attenuation,
			pCurLight.attenuation.x,
			pCurLight.attenuation.y,
			pCurLight.attenuation.z,
			1.0f );

		glUniform4f( pCurLight.shaderlocID_direction,
			pCurLight.direction.x,
			pCurLight.direction.y,
			pCurLight.direction.z,
			1.0f );

		// x = type, y = distance cut-off
		// z angle1, w = angle2		
		glUniform4f( pCurLight.shaderlocID_typeParams,
			pCurLight.typeParams.x,
			pCurLight.typeParams.y,
			pCurLight.typeParams.z,
			pCurLight.typeParams.w );
	}// for ( int index = 0;...

	return;
}