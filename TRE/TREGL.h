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
#include <GL/glext.h>
#include <GL/glu.h>
#endif	// __APPLE__

#ifdef WIN32
#include <GL/wglext.h>
#else // WIN32

//	WGL ext stuff
typedef void * (APIENTRY * PFNWGLALLOCATEMEMORYNVPROC) (int size, float readfreq, float writefreq, float priority);
typedef void (APIENTRY * PFNWGLFREEMEMORYNVPROC) (void *pointer);

//	ext stuff
typedef void (APIENTRY * PFNGLFLUSHVERTEXARRAYRANGENVPROC) (void);
typedef void (APIENTRY * PFNGLVERTEXARRAYRANGENVPROC) (GLsizei size, const GLvoid *pointer);

#endif // WIN32