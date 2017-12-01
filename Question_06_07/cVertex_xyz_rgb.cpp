#include "cVertex_xyz_rgb.h"

float x = 0.0f;

// Constructor
cVertex_xyz_rgb::cVertex_xyz_rgb()	
{
	this->x = 0.0f; 
	this->y = 0.0f;
	this->z = 0.0f;
	this->r = 0.0f;
	this->g = 0.0f;
	this->b = 0.0f;
	return;
}

	// Destructor
cVertex_xyz_rgb::~cVertex_xyz_rgb()
{
	// Doesn't really do anything
	return;
}
