#ifndef __TREGLEXTENSIONS_H__
#define __TREGLEXTENSIONS_H__

#include <TCFoundation/TCObject.h>
#include <TRE/TREGL.h>

struct TREVertex;
class TREVertexArray;
class TCVector;

class TREGLExtensions
{
public:
	static void setup(void);

	static PFNGLVERTEXARRAYRANGENVPROC getGlVertexArrayRangeNV(void)
	{
		return sm_glVertexArrayRangeNV;
	}
	static PFNGLMULTIDRAWELEMENTSEXTPROC getGlMultiDrawElementsEXT(void)
	{
		return sm_glMultiDrawElementsEXT;
	}
	static PFNGLBINDBUFFERARBPROC getGlBindBufferARB(void)
	{
		return sm_glBindBufferARB;
	}
	static PFNGLDELETEBUFFERSARBPROC getGlDeleteBuffersARB(void)
	{
		return sm_glDeleteBuffersARB;
	}
	static PFNGLGENBUFFERSARBPROC getGlGenBuffersARB(void)
	{
		return sm_glGenBuffersARB;
	}
	static PFNGLISBUFFERARBPROC getGlIsBufferARB(void)
	{
		return sm_glIsBufferARB;
	}
	static PFNGLBUFFERDATAARBPROC getGlBufferDataARB(void)
	{
		return sm_glBufferDataARB;
	}
	static PFNGLBUFFERSUBDATAARBPROC getGlBufferSubDataARB(void)
	{
		return sm_glBufferSubDataARB;
	}
	static PFNGLGETBUFFERSUBDATAARBPROC getGlGetBufferSubDataARB(void)
	{
		return sm_glGetBufferSubDataARB;
	}
	static PFNGLMAPBUFFERARBPROC getGlMapBufferARB(void)
	{
		return sm_glMapBufferARB;
	}
	static PFNGLUNMAPBUFFERARBPROC getGlUnmapBufferARB(void)
	{
		return sm_glUnmapBufferARB;
	}
	static PFNGLGETBUFFERPARAMETERIVARBPROC getGlGetBufferParameterivARB(void)
	{
		return sm_glGetBufferParameterivARB;
	}
	static PFNGLGETBUFFERPOINTERVARBPROC getGlGetBufferPointervARB(void)
	{
		return sm_glGetBufferPointervARB;
	}

	static bool checkForExtension(char* extension, bool force = false);
	static bool checkForExtension(char* extensionsString, char* extension,
		bool force = false);
	static bool haveNvMultisampleFilterHintExtension(bool force = false);
	static bool haveVARExtension(bool force = false);
	static bool haveMultiDrawArraysExtension(bool force = false);
	static bool haveVBOExtension(bool force = false);
	static bool haveAnisoExtension(bool force = false);
	static GLfloat getMaxAnisoLevel(void);
	static void disableAll(bool disable);

protected:
	static void cleanup(void);

	// GL_NV_vertex_array_range
	static PFNGLVERTEXARRAYRANGENVPROC sm_glVertexArrayRangeNV;
	// GL_EXT_multi_draw_arrays
	static PFNGLMULTIDRAWELEMENTSEXTPROC sm_glMultiDrawElementsEXT;
	// GL_ARB_vertex_buffer_object
	static PFNGLBINDBUFFERARBPROC sm_glBindBufferARB;
	static PFNGLDELETEBUFFERSARBPROC sm_glDeleteBuffersARB;
	static PFNGLGENBUFFERSARBPROC sm_glGenBuffersARB;
	static PFNGLISBUFFERARBPROC sm_glIsBufferARB;
	static PFNGLBUFFERDATAARBPROC sm_glBufferDataARB;
	static PFNGLBUFFERSUBDATAARBPROC sm_glBufferSubDataARB;
	static PFNGLGETBUFFERSUBDATAARBPROC sm_glGetBufferSubDataARB;
	static PFNGLMAPBUFFERARBPROC sm_glMapBufferARB;
	static PFNGLUNMAPBUFFERARBPROC sm_glUnmapBufferARB;
	static PFNGLGETBUFFERPARAMETERIVARBPROC sm_glGetBufferParameterivARB;
	static PFNGLGETBUFFERPOINTERVARBPROC sm_glGetBufferPointervARB;

	static char *sm_glExtensions;
	static GLfloat sm_maxAnisoLevel;
	static bool sm_rendererIsMesa;
	static bool sm_tempDisable;

	static class TREGLExtensionsCleanup
	{
	public:
		~TREGLExtensionsCleanup(void);
	} sm_extensionsCleanup;
	friend class TREGLExtensionsCleanup;
};

#endif // __TREGLEXTENSIONS_H__
