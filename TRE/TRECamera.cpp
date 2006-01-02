#include <string.h>
#include "TRECamera.h"
#include "TREGL.h"
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/mystring.h>

TRECamera::TRECamera(void)
	:name(NULL)
{
}

TRECamera::TRECamera(const TRECamera &other)
{
	duplicate(other);
}

TRECamera::~TRECamera(void)
{
//	delete name;
}

TRECamera& TRECamera::duplicate(const TRECamera& copyFrom)
{
	position = copyFrom.position;
	facing = copyFrom.facing;
	setName(copyFrom.name);
	return *this;
}

void TRECamera::setName(char* n)
{
	delete name;
	name = copyString(n);
}


void TRECamera::project(const TCVector &distance)
{
	TCFloat inverseMatrix[16];
	TCVector center = -distance;

	facing.getInverseMatrix(inverseMatrix);
	center = -distance.mult(inverseMatrix) - position;
	treGlMultMatrixf(inverseMatrix);
	treGlTranslatef(center[0], center[1], center[2]);
}

void TRECamera::rotate(const TCVector &rotation)
{
	TCFloat inverseMatrix[16];

	facing.getInverseMatrix(inverseMatrix);
	if (!fEq(rotation.get(1), 0.0))
	{
		facing = facing +
			TREFacing(TCVector(-1,0,0).mult(inverseMatrix).normalize(),
			rotation.get(1));
	}
	if (!fEq(rotation.get(0), 0.0))
	{
		facing = facing +
			TREFacing(TCVector(0,1,0).mult(inverseMatrix).normalize(),
			rotation.get(0));
	}
	if (!fEq(rotation.get(2), 0.0))
	{
		facing = facing +
			TREFacing(TCVector(0,0,1).mult(inverseMatrix).normalize(),
			rotation.get(2));
	}
}

void TRECamera::move(const TCVector &distance)
{
	TCFloat inverseMatrix[16];
//	float *matrix;
	TCVector v = distance;

//	matrix = facing.getMatrix();
	facing.getInverseMatrix(inverseMatrix);
	position += v.mult(inverseMatrix);
}
