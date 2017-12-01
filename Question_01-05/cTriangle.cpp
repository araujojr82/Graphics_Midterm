#include "cTriangle.h"

cTriangle::cTriangle()
{	// Set initial values for triangle
	// (set to zero? Set to something else?)
	this->vertex_ID_0 = -1;		// -1 is invalid value
	this->vertex_ID_1 = -1;		// -1 is invalid value
	this->vertex_ID_2 = -1;		// -1 is invalid value
	return;
}

cTriangle::~cTriangle()
{
	return;
}