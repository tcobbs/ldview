#include "LDViewPoint.h"
#include <TRE/TRECamera.h>
#include <TCFoundation/mystring.h>

LDViewPoint::LDViewPoint(void):
	m_camera(NULL)
{
}

LDViewPoint::~LDViewPoint(void)
{
}

void LDViewPoint::dealloc(void)
{
	delete m_camera;
	TCObject::dealloc();
}

void LDViewPoint::setCamera(const TRECamera &value)
{
	delete m_camera;
	m_camera = new TRECamera(value);
}

void LDViewPoint::setBackgroundColor(
	GLclampf r,
	GLclampf g,
	GLclampf b,
	GLclampf a)
{
	m_backgroundR = r;
	m_backgroundG = g;
	m_backgroundB = b;
	m_backgroundA = a;
}

void LDViewPoint::getBackgroundColor(
	GLclampf &r,
	GLclampf &g,
	GLclampf &b,
	GLclampf &a) const
{
	r = m_backgroundR;
	g = m_backgroundG;
	b = m_backgroundB;
	a = m_backgroundA;
}


void LDViewPoint::setRotationMatrix(const float *value)
{
	memcpy(m_rotationMatrix, value, sizeof(m_rotationMatrix));
}
