#include "TREGLExtensions.h"
#include "TREVertexStore.h"
#include "TREShapeGroup.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCUserDefaults.h>
// !!!WARNING!!!
// The following is kind of bad form, but I'm going to do it anyway.  I'm
// including a header file from a library that is dependent on this library.
// This works fine, because I'm not using any classes in the other library,
// just #defines, but it is still very strange behavior.
#include <LDLib/LDUserDefaultsKeys.h>

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

char *TREGLExtensions::sm_glExtensions = NULL;
GLfloat TREGLExtensions::sm_maxAnisoLevel = 1.0f;

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

void TREGLExtensions::cleanup(void)
{
	delete sm_glExtensions;
	sm_glExtensions = NULL;
}

void TREGLExtensions::setup(void)
{
	cleanup();
	sm_glExtensions = copyString((char*)glGetString(GL_EXTENSIONS));
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
		TREVertexStore::setGlVertexArrayRangeNV(sm_glVertexArrayRangeNV);
	}
	if (haveMultiDrawArraysExtension(true))
	{
		sm_glMultiDrawElementsEXT = (PFNGLMULTIDRAWELEMENTSEXTPROC)
			GET_EXTENSION(glMultiDrawElementsEXT);
		TREShapeGroup::setGlMultiDrawElementsEXT(sm_glMultiDrawElementsEXT);
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
		TREVertexStore::setGlBindBufferARB(sm_glBindBufferARB);
		TREVertexStore::setGlDeleteBuffersARB(sm_glDeleteBuffersARB);
		TREVertexStore::setGlGenBuffersARB(sm_glGenBuffersARB);
		TREVertexStore::setGlBufferDataARB(sm_glBufferDataARB);
	}
	if (haveAnisoExtension(true))
	{
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &sm_maxAnisoLevel);
	}
	else
	{
		sm_maxAnisoLevel = 1.0f;
	}
}

bool TREGLExtensions::haveNvMultisampleFilterHintExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_MS_FILTER_HINT_KEY, 0,
		false) != 0;

	return (!ignore || force) &&
		checkForExtension("GL_NV_multisample_filter_hint");
}

bool TREGLExtensions::haveVARExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_VAR_KEY, 0, false) != 0;

	return (!ignore || force) && checkForExtension("GL_NV_vertex_array_range");
}

bool TREGLExtensions::haveMultiDrawArraysExtension(bool force)
{
#if !defined(WIN32) && !defined(__APPLE__)
	// This one crashes Mesa in Linux right now.  Disable until I figure out
	// why.  Note that it seems to work fine on a MacBook with Intel Graphics.
	return false;
#endif // !WIN32 && !__APPLE_
	bool ignore = TCUserDefaults::longForKey(IGNORE_MULTI_DRAW_ARRAYS_KEY, 0,
		false) != 0;

	return (!ignore || force) && checkForExtension("GL_EXT_multi_draw_arrays");
}

bool TREGLExtensions::haveAnisoExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_ANISO_KEY, 0, false) != 0;

	return (!ignore || force) &&
		checkForExtension("GL_EXT_texture_filter_anisotropic");
}

bool TREGLExtensions::haveVBOExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_VBO_KEY, 0, false) != 0;

	return (!ignore || force) &&
		checkForExtension("GL_ARB_vertex_buffer_object");
}

bool TREGLExtensions::checkForExtension(char* extensionsString,
										char* extension, bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_ALL_OGL_EXTENSIONS, 0,
		false) != 0;

	if ((!ignore || force) && extensionsString)
	{
		int extensionLen = strlen(extension);
		char* extensions = extensionsString;

		while (extensions)
		{
			if (strcmp(extensions, extension) == 0 ||
				(strncmp(extensions, extension, extensionLen) == 0 &&
				extensions[extensionLen] == ' ') &&
				(extensions == extensionsString || extensions[-1] == ' '))
			{
				return true;
			}
			extensions = strstr(extensions, extension);
		}
	}
	return false;
}

bool TREGLExtensions::checkForExtension(char* extension, bool force)
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
