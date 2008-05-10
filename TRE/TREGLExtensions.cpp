#include "TREGLExtensions.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCUserDefaults.h>
// !!!WARNING!!!
// The following is kind of bad form, but I'm going to do it anyway.  I'm
// including a header file from a library that is dependent on this library.
// This works fine, because I'm not using any classes in the other library,
// just #defines, but it is still very strange behavior.
#include <LDLib/LDUserDefaultsKeys.h>

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

namespace TREGLExtensionsNS
{
#ifndef GL_GLEXT_PROTOTYPES
	// GL_NV_vertex_array_range
	PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV = NULL;
	// GL_EXT_multi_draw_arrays
	PFNGLMULTIDRAWELEMENTSEXTPROC glMultiDrawElementsEXT = NULL;
	// GL_ARB_vertex_buffer_object
	PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;
	PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;
	PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;
	PFNGLISBUFFERARBPROC glIsBufferARB = NULL;
	PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;
	PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB = NULL;
	PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB = NULL;
	PFNGLMAPBUFFERARBPROC glMapBufferARB = NULL;
	PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB = NULL;
	PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB = NULL;
	PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB = NULL;
	// GL_ARB_occlusion_query
    PFNGLGENQUERIESARBPROC glGenQueriesARB = NULL;
    PFNGLDELETEQUERIESARBPROC glDeleteQueriesARB = NULL;
    PFNGLISQUERYARBPROC glIsQueryARB = NULL;
    PFNGLBEGINQUERYARBPROC glBeginQueryARB = NULL;
    PFNGLENDQUERYARBPROC glEndQueryARB = NULL;
    PFNGLGETQUERYIVARBPROC glGetQueryivARB = NULL;
    PFNGLGETQUERYOBJECTIVARBPROC glGetQueryObjectivARB = NULL;
    PFNGLGETQUERYOBJECTUIVARBPROC glGetQueryObjectuivARB = NULL;

#ifdef WIN32
	// WGL_EXT_pixel_format
	PFNWGLGETPIXELFORMATATTRIBIVEXTPROC wglGetPixelFormatAttribivARB = NULL;
	PFNWGLGETPIXELFORMATATTRIBFVEXTPROC wglGetPixelFormatAttribfvARB = NULL;
	PFNWGLCHOOSEPIXELFORMATEXTPROC wglChoosePixelFormatARB = NULL;
	// WGL_ARB_extensions_string
	PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = NULL;
	// WGL_ARB_pbuffer
	PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB = NULL;
	PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB = NULL;
	PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB = NULL;
	PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB = NULL;
	PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB = NULL;
	// WGL_NV_allocate_memory
	PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV = NULL;
	PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV = NULL;
#endif WIN32
#endif // GL_GLEXT_PROTOTYPES
}

// GL_NV_vertex_array_range
PFNGLVERTEXARRAYRANGENVPROC TREGLExtensions::sm_glVertexArrayRangeNV = NULL;
// GL_EXT_multi_draw_arrays
PFNGLMULTIDRAWELEMENTSEXTPROC TREGLExtensions::sm_glMultiDrawElementsEXT = NULL;
// GL_ARB_vertex_buffer_object
PFNGLBINDBUFFERARBPROC TREGLExtensions::sm_glBindBufferARB = NULL;
PFNGLDELETEBUFFERSARBPROC TREGLExtensions::sm_glDeleteBuffersARB = NULL;
PFNGLGENBUFFERSARBPROC TREGLExtensions::sm_glGenBuffersARB = NULL;
PFNGLISBUFFERARBPROC TREGLExtensions::sm_glIsBufferARB = NULL;
PFNGLBUFFERDATAARBPROC TREGLExtensions::sm_glBufferDataARB = NULL;
PFNGLBUFFERSUBDATAARBPROC TREGLExtensions::sm_glBufferSubDataARB = NULL;
PFNGLGETBUFFERSUBDATAARBPROC TREGLExtensions::sm_glGetBufferSubDataARB = NULL;
PFNGLMAPBUFFERARBPROC TREGLExtensions::sm_glMapBufferARB = NULL;
PFNGLUNMAPBUFFERARBPROC TREGLExtensions::sm_glUnmapBufferARB = NULL;
PFNGLGETBUFFERPARAMETERIVARBPROC TREGLExtensions::sm_glGetBufferParameterivARB =
	NULL;
PFNGLGETBUFFERPOINTERVARBPROC TREGLExtensions::sm_glGetBufferPointervARB = NULL;
// GL_ARB_occlusion_query
PFNGLGENQUERIESARBPROC TREGLExtensions::sm_glGenQueriesARB = NULL;
PFNGLDELETEQUERIESARBPROC TREGLExtensions::sm_glDeleteQueriesARB = NULL;
PFNGLISQUERYARBPROC TREGLExtensions::sm_glIsQueryARB = NULL;
PFNGLBEGINQUERYARBPROC TREGLExtensions::sm_glBeginQueryARB = NULL;
PFNGLENDQUERYARBPROC TREGLExtensions::sm_glEndQueryARB = NULL;
PFNGLGETQUERYIVARBPROC TREGLExtensions::sm_glGetQueryivARB = NULL;
PFNGLGETQUERYOBJECTIVARBPROC TREGLExtensions::sm_glGetQueryObjectivARB = NULL;
PFNGLGETQUERYOBJECTUIVARBPROC TREGLExtensions::sm_glGetQueryObjectuivARB = NULL;

StringSet TREGLExtensions::sm_glExtensions;
GLfloat TREGLExtensions::sm_maxAnisoLevel = 1.0f;
bool TREGLExtensions::sm_rendererIsMesa = false;
bool TREGLExtensions::sm_tempDisable = false;

TREGLExtensions::TREGLExtensionsCleanup TREGLExtensions::sm_extensionsCleanup;

#ifdef WIN32
#define GET_EXTENSION(name) wglGetProcAddress(#name)
#else
#define GET_EXTENSION(name) (name)
#endif

TREGLExtensions::TREGLExtensionsCleanup::~TREGLExtensionsCleanup(void)
{
	TREGLExtensions::cleanup();
}

void TREGLExtensions::disableAll(bool disable)
{
	sm_tempDisable = disable;
}

void TREGLExtensions::cleanup(void)
{
	sm_glExtensions.clear();
}

void TREGLExtensions::initExtensions(
	StringSet &extensions,
	const char *extensionsString)
{
	int count;
	char **components = componentsSeparatedByString(extensionsString, " ",
		count);

	for (int i = 0; i < count; i++)
	{
		extensions.insert(components[i]);
	}
	deleteStringArray(components, count);
}

void TREGLExtensions::setup(void)
{
	cleanup();
	initExtensions(sm_glExtensions, (const char*)glGetString(GL_EXTENSIONS));
	// Note that when we load the function pointers, don't want to pay
	// attention to any ignore flags in the registry, so all the checks for
	// extensions have the force flag set to true.  Otherwise, if the
	// program starts with the ignore flag set, and it later gets cleared,
	// the function pointers won't be loaded.
	if (haveVARExtension(true))
	{
#ifndef __APPLE__
		// NOTE: I should really change the __APPLE__ path to do their
		// equivalent of wglGetProcAddress.  However, since this extension won't
		// work anyway without a bunch of glX stuff that I'm not going to do,
		// I'm not going to make that change just for this.
		sm_glVertexArrayRangeNV = (PFNGLVERTEXARRAYRANGENVPROC)
			GET_EXTENSION(glVertexArrayRangeNV);
#endif // __APPLE__
	}
	const char *renderer = (const char *)glGetString(GL_RENDERER);
	if (stringHasCaseInsensitivePrefix(renderer, "Mesa "))
	{
		sm_rendererIsMesa = true;
	}
	if (haveMultiDrawArraysExtension(true))
	{
		sm_glMultiDrawElementsEXT = (PFNGLMULTIDRAWELEMENTSEXTPROC)
			GET_EXTENSION(glMultiDrawElementsEXT);
	}
	if (haveVBOExtension(true))
	{
		sm_glBindBufferARB = (PFNGLBINDBUFFERARBPROC)
			GET_EXTENSION(glBindBufferARB);
		sm_glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)
			GET_EXTENSION(glDeleteBuffersARB);
		sm_glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)
			GET_EXTENSION(glGenBuffersARB);
		sm_glIsBufferARB = (PFNGLISBUFFERARBPROC)
			GET_EXTENSION(glIsBufferARB);
		sm_glBufferDataARB = (PFNGLBUFFERDATAARBPROC)
			GET_EXTENSION(glBufferDataARB);
		sm_glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)
			GET_EXTENSION(glBufferSubDataARB);
		sm_glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)
			GET_EXTENSION(glGetBufferSubDataARB);
		sm_glMapBufferARB = (PFNGLMAPBUFFERARBPROC)
			GET_EXTENSION(glMapBufferARB);
		sm_glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)
			GET_EXTENSION(glUnmapBufferARB);
		sm_glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)
			GET_EXTENSION(glGetBufferParameterivARB);
		sm_glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)
			GET_EXTENSION(glGetBufferPointervARB);
	}
	if (haveOcclusionQueryExtension(true))
	{
		sm_glGenQueriesARB = (PFNGLGENQUERIESARBPROC)
			GET_EXTENSION(glGenQueriesARB);
    	sm_glDeleteQueriesARB = (PFNGLDELETEQUERIESARBPROC)
			GET_EXTENSION(glDeleteQueriesARB);
    	sm_glIsQueryARB = (PFNGLISQUERYARBPROC)
			GET_EXTENSION(glIsQueryARB);
    	sm_glBeginQueryARB = (PFNGLBEGINQUERYARBPROC)
			GET_EXTENSION(glBeginQueryARB);
    	sm_glEndQueryARB = (PFNGLENDQUERYARBPROC)
			GET_EXTENSION(glEndQueryARB);
    	sm_glGetQueryivARB = (PFNGLGETQUERYIVARBPROC)
			GET_EXTENSION(glGetQueryivARB);
    	sm_glGetQueryObjectivARB = (PFNGLGETQUERYOBJECTIVARBPROC)
			GET_EXTENSION(glGetQueryObjectivARB);
    	sm_glGetQueryObjectuivARB = (PFNGLGETQUERYOBJECTUIVARBPROC)
			GET_EXTENSION(glGetQueryObjectuivARB);
	}
	if (haveAnisoExtension(true))
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &sm_maxAnisoLevel);
	}
	else
	{
		sm_maxAnisoLevel = 1.0f;
	}
#ifndef GL_GLEXT_PROTOTYPES
	using namespace TREGLExtensionsNS;
	glVertexArrayRangeNV = sm_glVertexArrayRangeNV;
	// GL_EXT_multi_draw_arrays
	glMultiDrawElementsEXT = sm_glMultiDrawElementsEXT;
	// GL_ARB_vertex_buffer_object
	glBindBufferARB = sm_glBindBufferARB;
	glDeleteBuffersARB = sm_glDeleteBuffersARB;
	glGenBuffersARB = sm_glGenBuffersARB;
	glIsBufferARB = sm_glIsBufferARB;
	glBufferDataARB = sm_glBufferDataARB;
	glBufferSubDataARB = sm_glBufferSubDataARB;
	glGetBufferSubDataARB = sm_glGetBufferSubDataARB;
	glMapBufferARB = sm_glMapBufferARB;
	glUnmapBufferARB = sm_glUnmapBufferARB;
	glGetBufferParameterivARB = sm_glGetBufferParameterivARB;
	glGetBufferPointervARB = sm_glGetBufferPointervARB;
	// GL_ARB_occlusion_query
    glGenQueriesARB = sm_glGenQueriesARB;
    glDeleteQueriesARB = sm_glDeleteQueriesARB;
    glIsQueryARB = sm_glIsQueryARB;
    glBeginQueryARB = sm_glBeginQueryARB;
    glEndQueryARB = sm_glEndQueryARB;
    glGetQueryivARB = sm_glGetQueryivARB;
    glGetQueryObjectivARB = sm_glGetQueryObjectivARB;
    glGetQueryObjectuivARB = sm_glGetQueryObjectuivARB;
#endif GL_GLEXT_PROTOTYPES
}

bool TREGLExtensions::haveNvMultisampleFilterHintExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_MS_FILTER_HINT_KEY, 0,
		false) != 0;

	return (!ignore || force) &&
		checkForExtension("GL_NV_multisample_filter_hint", force);
}

bool TREGLExtensions::haveVARExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_VAR_KEY, 0, false) != 0;

	return (!ignore || force) && checkForExtension("GL_NV_vertex_array_range",
		force);
}

bool TREGLExtensions::haveMultiDrawArraysExtension(bool force)
{
	if (sm_rendererIsMesa)
	{
		// This extension apparently sucks on Mesa.  As near as I can tell, it
		// won't work inside a display list.
		return false;
	}
#ifdef __APPLE__
	int one = 1;
	TCByte binary[] = {1,0,0,0};

	if (one != *(int*)binary)
	{
		// Doesn't work on PPC Mac (at least in Rosetta).  The above checks to
		// see if the current architecture is big endian or little endian.  If
		// it's big endian (PPC), we'll get here.
		return false;
	}
#endif
	bool ignore = TCUserDefaults::longForKey(IGNORE_MULTI_DRAW_ARRAYS_KEY, 0,
		false) != 0;

	return (!ignore || force) && checkForExtension("GL_EXT_multi_draw_arrays",
		force);
}

bool TREGLExtensions::haveOcclusionQueryExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_OCCLUSION_QUERY_KEY, 0,
		false) != 0;

	return (!ignore || force) && checkForExtension("GL_ARB_occlusion_query",
		force);
}

bool TREGLExtensions::haveAnisoExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_ANISO_KEY, 0, false) != 0;

	return (!ignore || force) &&
		checkForExtension("GL_EXT_texture_filter_anisotropic", force);
}

bool TREGLExtensions::haveVBOExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_VBO_KEY, 0, false) != 0;

	return (!ignore || force) &&
		checkForExtension("GL_ARB_vertex_buffer_object", force);
}

bool TREGLExtensions::checkForExtension(
	const StringSet &extensions,
	const char* extension,
	bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_ALL_OGL_EXTENSIONS, 0,
		false) != 0 || sm_tempDisable;

	if (!ignore || force)
	{
		return extensions.find(extension) != extensions.end();
	}
	return false;
}

bool TREGLExtensions::checkForExtension(const char* extension, bool force)
{
	return checkForExtension(sm_glExtensions, extension, force);
}

GLfloat TREGLExtensions::getMaxAnisoLevel(void)
{
	if (haveAnisoExtension())
	{
		return sm_maxAnisoLevel;
	}
	else
	{
		return 1.0f;
	}
}
