#include "Physics.h"
#include "cGameObject.h"

#include <glm/glm.hpp>		// glm::distance

bool PenetrationTestSphereSphere( cGameObject* pA, cGameObject* pB )
{
	// If the distance between the two sphere is LT the sum or the radii,
	//	they are touching or penetrating

	float totalRadii = pA->radius + pB->radius;

	// The Pythagorean distance 
	float distance = glm::distance( pA->position, pB->position );

	if ( distance <= totalRadii )
	{
		return true;
	}

	return false;
}