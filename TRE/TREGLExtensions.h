#ifndef __TREGLEXTENSIONS_H__
#define __TREGLEXTENSIONS_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCStlIncludes.h>
#include <TRE/TREGL.h>

struct TREVertex;
class TREVertexArray;
class TCVector;

typedef std::set<std::string> StringSet;

namespace TREGLExtensionsNS
{
#ifndef GL_GLEXT_PROTOTYPES
	// GL_NV_vertex_array_range
	extern PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;
	// GL_EXT_multi_draw_arrays
	extern PFNGLMULTIDRAWELEMENTSEXTPROC glMultiDrawElementsEXT;
	// GL_ARB_vertex_buffer_object
	extern PFNGLBINDBUFFERARBPROC glBindBufferARB;
	extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
	extern PFNGLGENBUFFERSARBPROC glGenBuffersARB;
	extern PFNGLISBUFFERARBPROC glIsBufferARB;
	extern PFNGLBUFFERDATAARBPROC glBufferDataARB;
	extern PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;
	extern PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB;
	extern PFNGLMAPBUFFERARBPROC glMapBufferARB;
	extern PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;
	extern PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
	extern PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB;
	// GL_ARB_occlusion_query
    extern PFNGLGENQUERIESARBPROC glGenQueriesARB;
    extern PFNGLDELETEQUERIESARBPROC glDeleteQueriesARB;
    extern PFNGLISQUERYARBPROC glIsQueryARB;
    extern PFNGLBEGINQUERYARBPROC glBeginQueryARB;
    extern PFNGLENDQUERYARBPROC glEndQueryARB;
    extern PFNGLGETQUERYIVARBPROC glGetQueryivARB;
    extern PFNGLGETQUERYOBJECTIVARBPROC glGetQueryObjectivARB;
    extern PFNGLGETQUERYOBJECTUIVARBPROC glGetQueryObjectuivARB;
#endif // GL_GLEXT_PROTOTYPES

#ifdef WIN32
	// WGL_EXT_pixel_format
	extern PFNWGLGETPIXELFORMATATTRIBIVEXTPROC wglGetPixelFormatAttribivARB;
	extern PFNWGLGETPIXELFORMATATTRIBFVEXTPROC wglGetPixelFormatAttribfvARB;
	extern PFNWGLCHOOSEPIXELFORMATEXTPROC wglChoosePixelFormatARB;
	// WGL_ARB_extensions_string
	extern PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
	// WGL_ARB_pbuffer
	extern PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB;
	extern PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB;
	extern PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
	extern PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB;
	extern PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB;
#endif // WIN32
	// Windows doesn't define GL_GLEXT_PROTOTYPES, and none of the WGL function
	// pointers will exist outside Windows, so always define.
	// WGL_NV_allocate_memory
	extern PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV;
	extern PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV;
}

class TREGLExtensions
{
public:
	static void setup(void);

	//// GL_NV_vertex_array_range
	//static PFNGLVERTEXARRAYRANGENVPROC getGlVertexArrayRangeNV(void)
	//{
	//	return sm_glVertexArrayRangeNV;
	//}
	//// GL_EXT_multi_draw_arrays
	//static PFNGLMULTIDRAWELEMENTSEXTPROC getGlMultiDrawElementsEXT(void)
	//{
	//	return sm_glMultiDrawElementsEXT;
	//}
	//// GL_ARB_vertex_buffer_object
	//static PFNGLBINDBUFFERARBPROC getGlBindBufferARB(void)
	//{
	//	return sm_glBindBufferARB;
	//}
	//static PFNGLDELETEBUFFERSARBPROC getGlDeleteBuffersARB(void)
	//{
	//	return sm_glDeleteBuffersARB;
	//}
	//static PFNGLGENBUFFERSARBPROC getGlGenBuffersARB(void)
	//{
	//	return sm_glGenBuffersARB;
	//}
	//static PFNGLISBUFFERARBPROC getGlIsBufferARB(void)
	//{
	//	return sm_glIsBufferARB;
	//}
	//static PFNGLBUFFERDATAARBPROC getGlBufferDataARB(void)
	//{
	//	return sm_glBufferDataARB;
	//}
	//static PFNGLBUFFERSUBDATAARBPROC getGlBufferSubDataARB(void)
	//{
	//	return sm_glBufferSubDataARB;
	//}
	//static PFNGLGETBUFFERSUBDATAARBPROC getGlGetBufferSubDataARB(void)
	//{
	//	return sm_glGetBufferSubDataARB;
	//}
	//static PFNGLMAPBUFFERARBPROC getGlMapBufferARB(void)
	//{
	//	return sm_glMapBufferARB;
	//}
	//static PFNGLUNMAPBUFFERARBPROC getGlUnmapBufferARB(void)
	//{
	//	return sm_glUnmapBufferARB;
	//}
	//static PFNGLGETBUFFERPARAMETERIVARBPROC getGlGetBufferParameterivARB(void)
	//{
	//	return sm_glGetBufferParameterivARB;
	//}
	//static PFNGLGETBUFFERPOINTERVARBPROC getGlGetBufferPointervARB(void)
	//{
	//	return sm_glGetBufferPointervARB;
	//}
	//// GL_ARB_occlusion_query
	//static PFNGLGENQUERIESARBPROC getGlGenQueriesARB(void)
	//{
	//	return sm_glGenQueriesARB;
	//}
	//static PFNGLDELETEQUERIESARBPROC getGlDeleteQueriesARB(void)
	//{
	//	return sm_glDeleteQueriesARB;
	//}
	//static PFNGLISQUERYARBPROC getGlIsQueryARB(void)
	//{
	//	return sm_glIsQueryARB;
	//}
	//static PFNGLBEGINQUERYARBPROC getGlBeginQueryARB(void)
	//{
	//	return sm_glBeginQueryARB;
	//}
	//static PFNGLENDQUERYARBPROC getGlEndQueryARB(void)
	//{
	//	return sm_glEndQueryARB;
	//}
	//static PFNGLGETQUERYIVARBPROC getGlGetQueryivARB(void)
	//{
	//	return sm_glGetQueryivARB;
	//}
	//static PFNGLGETQUERYOBJECTIVARBPROC getGlGetQueryObjectivARB(void)
	//{
	//	return sm_glGetQueryObjectivARB;
	//}
	//static PFNGLGETQUERYOBJECTUIVARBPROC getGlGetQueryObjectuivARB(void)
	//{
	//	return sm_glGetQueryObjectuivARB;
	//}

	static bool checkForExtension(const char* extension,
		bool force = false);
	static bool checkForExtension(const StringSet &extensions,
		const char* extension, bool force = false);
	static bool haveNvMultisampleFilterHintExtension(bool force = false);
	static bool haveVARExtension(bool force = false);
	static bool haveMultiDrawArraysExtension(bool force = false);
	static bool haveVBOExtension(bool force = false);
	static bool haveAnisoExtension(bool force = false);
	static bool haveOcclusionQueryExtension(bool force = false);
	static GLfloat getMaxAnisoLevel(void);
	static void disableAll(bool disable);
	static void initExtensions(StringSet &extensions,
		const char *extensionsString);

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
	// GL_ARB_occlusion_query
    static PFNGLGENQUERIESARBPROC sm_glGenQueriesARB;
    static PFNGLDELETEQUERIESARBPROC sm_glDeleteQueriesARB;
    static PFNGLISQUERYARBPROC sm_glIsQueryARB;
    static PFNGLBEGINQUERYARBPROC sm_glBeginQueryARB;
    static PFNGLENDQUERYARBPROC sm_glEndQueryARB;
    static PFNGLGETQUERYIVARBPROC sm_glGetQueryivARB;
    static PFNGLGETQUERYOBJECTIVARBPROC sm_glGetQueryObjectivARB;
    static PFNGLGETQUERYOBJECTUIVARBPROC sm_glGetQueryObjectuivARB;

	static StringSet sm_glExtensions;
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
