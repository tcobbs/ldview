#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#else // WIN32
#include <netinet/in.h>
#endif // WIN32

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#define APIENTRY
#else	// __APPLE__
#include <GL/gl.h>
#endif	// __APPLE__
