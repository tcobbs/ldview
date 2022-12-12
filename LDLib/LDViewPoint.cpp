#include "LDViewPoint.h"
#include <LDLoader/LDLCamera.h>
#include <TCFoundation/mystring.h>

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

LDViewPoint::LDViewPoint(void)
	: m_camera(NULL)
	, m_autoCenter(false)
	, m_backgroundR(0)
	, m_backgroundG(0)
	, m_backgroundB(0)
	, m_backgroundA(0)
{
#ifndef USE_CPP11
	memset(m_rotationMatrix, 0, sizeof(m_rotationMatrix));
#endif // USE_CPP11
}

LDViewPoint::~LDViewPoint(void)
{
}

void LDViewPoint::dealloc(void)
{
	delete m_camera;
	TCObject::dealloc();
}

void LDViewPoint::setCamera(const LDLCamera &value)
{
	delete m_camera;
	m_camera = new LDLCamera(value);
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
