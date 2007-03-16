#include "LDVExtensionsSetup.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TRE/TREVertexStore.h>
#include <TRE/TREShapeGroup.h>
#include <TRE/TREGLExtensions.h>
#include <LDLib/LDUserDefaultsKeys.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

// WGL_EXT_pixel_format
PFNWGLGETPIXELFORMATATTRIBIVEXTPROC
	LDVExtensionsSetup::wglGetPixelFormatAttribivARB = NULL;
PFNWGLGETPIXELFORMATATTRIBFVEXTPROC
	LDVExtensionsSetup::wglGetPixelFormatAttribfvARB = NULL;
PFNWGLCHOOSEPIXELFORMATEXTPROC
	LDVExtensionsSetup::wglChoosePixelFormatARB = NULL;
// WGL_ARB_extensions_string
PFNWGLGETEXTENSIONSSTRINGARBPROC
	LDVExtensionsSetup::wglGetExtensionsStringARB = NULL;
// WGL_ARB_pbuffer
PFNWGLCREATEPBUFFERARBPROC LDVExtensionsSetup::wglCreatePbufferARB = NULL;
PFNWGLGETPBUFFERDCARBPROC LDVExtensionsSetup::wglGetPbufferDCARB = NULL;
PFNWGLRELEASEPBUFFERDCARBPROC LDVExtensionsSetup::wglReleasePbufferDCARB = NULL;
PFNWGLDESTROYPBUFFERARBPROC LDVExtensionsSetup::wglDestroyPbufferARB = NULL;
PFNWGLQUERYPBUFFERARBPROC LDVExtensionsSetup::wglQueryPbufferARB = NULL;
// WGL_NV_allocate_memory
PFNWGLALLOCATEMEMORYNVPROC LDVExtensionsSetup::wglAllocateMemoryNV = NULL;
PFNWGLFREEMEMORYNVPROC LDVExtensionsSetup::wglFreeMemoryNV = NULL;
/*
// GL_NV_vertex_array_range
PFNGLVERTEXARRAYRANGENVPROC LDVExtensionsSetup::glVertexArrayRangeNV = NULL;
// GL_EXT_multi_draw_arrays
PFNGLMULTIDRAWELEMENTSEXTPROC LDVExtensionsSetup::glMultiDrawElementsEXT = NULL;
// GL_ARB_vertex_buffer_object
PFNGLBINDBUFFERARBPROC LDVExtensionsSetup::glBindBufferARB = NULL;
PFNGLDELETEBUFFERSARBPROC LDVExtensionsSetup::glDeleteBuffersARB = NULL;
PFNGLGENBUFFERSARBPROC LDVExtensionsSetup::glGenBuffersARB = NULL;
PFNGLISBUFFERARBPROC LDVExtensionsSetup::glIsBufferARB = NULL;
PFNGLBUFFERDATAARBPROC LDVExtensionsSetup::glBufferDataARB = NULL;
PFNGLBUFFERSUBDATAARBPROC LDVExtensionsSetup::glBufferSubDataARB = NULL;
PFNGLGETBUFFERSUBDATAARBPROC LDVExtensionsSetup::glGetBufferSubDataARB = NULL;
PFNGLMAPBUFFERARBPROC LDVExtensionsSetup::glMapBufferARB = NULL;
PFNGLUNMAPBUFFERARBPROC LDVExtensionsSetup::glUnmapBufferARB = NULL;
PFNGLGETBUFFERPARAMETERIVARBPROC LDVExtensionsSetup::glGetBufferParameterivARB =
	NULL;
PFNGLGETBUFFERPOINTERVARBPROC LDVExtensionsSetup::glGetBufferPointervARB = NULL;
*/

char *LDVExtensionsSetup::wglExtensions = NULL;
//char *LDVExtensionsSetup::glExtensions = NULL;
bool LDVExtensionsSetup::performedInitialSetup = false;
bool LDVExtensionsSetup::stencilPresent = false;
bool LDVExtensionsSetup::alphaPresent = false;
TCIntArray *LDVExtensionsSetup::fsaaModes = NULL;
TCIntArrayArray *LDVExtensionsSetup::pfIntValues = NULL;
LDVExtensionsSetup *LDVExtensionsSetup::extensionsSetup = NULL;

LDVExtensionsSetup::LDVExtensionsSetupCleanup
	LDVExtensionsSetup::extensionsSetupCleanup;


static char *pfIntAttribNames[] =
{
	"WGL_MAX_PBUFFER_WIDTH_ARB",
	"WGL_DRAW_TO_WINDOW_ARB",
	"WGL_DRAW_TO_BITMAP_ARB",
	"WGL_ACCELERATION_ARB",
	"WGL_NEED_PALETTE_ARB",
	"WGL_NEED_SYSTEM_PALETTE_ARB",
	"WGL_SWAP_LAYER_BUFFERS_ARB",
	"WGL_SWAP_METHOD_ARB",
	"WGL_NUMBER_OVERLAYS_ARB",
	"WGL_NUMBER_UNDERLAYS_ARB",
	"WGL_TRANSPARENT_ARB",
	"WGL_TRANSPARENT_RED_VALUE_ARB",
	"WGL_TRANSPARENT_GREEN_VALUE_ARB",
	"WGL_TRANSPARENT_BLUE_VALUE_ARB",
	"WGL_TRANSPARENT_ALPHA_VALUE_ARB",
	"WGL_TRANSPARENT_INDEX_VALUE_ARB",
	"WGL_SHARE_DEPTH_ARB",
	"WGL_SHARE_STENCIL_ARB",
	"WGL_SHARE_ACCUM_ARB",
	"WGL_SUPPORT_GDI_ARB",
	"WGL_SUPPORT_OPENGL_ARB",
	"WGL_DOUBLE_BUFFER_ARB",
	"WGL_STEREO_ARB",
	"WGL_PIXEL_TYPE_ARB",
	"WGL_COLOR_BITS_ARB",
	"WGL_RED_BITS_ARB",
	"WGL_RED_SHIFT_ARB",
	"WGL_GREEN_BITS_ARB",
	"WGL_GREEN_SHIFT_ARB",
	"WGL_BLUE_BITS_ARB",
	"WGL_BLUE_SHIFT_ARB",
	"WGL_ALPHA_BITS_ARB",
	"WGL_ALPHA_SHIFT_ARB",
	"WGL_ACCUM_BITS_ARB",
	"WGL_ACCUM_RED_BITS_ARB",
	"WGL_ACCUM_GREEN_BITS_ARB",
	"WGL_ACCUM_BLUE_BITS_ARB",
	"WGL_ACCUM_ALPHA_BITS_ARB",
	"WGL_DEPTH_BITS_ARB",
	"WGL_STENCIL_BITS_ARB",
	"WGL_AUX_BUFFERS_ARB",
	"WGL_DRAW_TO_PBUFFER_ARB",
	"WGL_SAMPLE_BUFFERS_EXT",
	"WGL_SAMPLES_EXT",
};
static int pfIntAttribs[] =
{
	WGL_MAX_PBUFFER_WIDTH_ARB,
	WGL_DRAW_TO_WINDOW_ARB,
	WGL_DRAW_TO_BITMAP_ARB,
	WGL_ACCELERATION_ARB,
	WGL_NEED_PALETTE_ARB,
	WGL_NEED_SYSTEM_PALETTE_ARB,
	WGL_SWAP_LAYER_BUFFERS_ARB,
	WGL_SWAP_METHOD_ARB,
	WGL_NUMBER_OVERLAYS_ARB,
	WGL_NUMBER_UNDERLAYS_ARB,
	WGL_TRANSPARENT_ARB,
	WGL_TRANSPARENT_RED_VALUE_ARB,
	WGL_TRANSPARENT_GREEN_VALUE_ARB,
	WGL_TRANSPARENT_BLUE_VALUE_ARB,
	WGL_TRANSPARENT_ALPHA_VALUE_ARB,
	WGL_TRANSPARENT_INDEX_VALUE_ARB,
	WGL_SHARE_DEPTH_ARB,
	WGL_SHARE_STENCIL_ARB,
	WGL_SHARE_ACCUM_ARB,
	WGL_SUPPORT_GDI_ARB,
	WGL_SUPPORT_OPENGL_ARB,
	WGL_DOUBLE_BUFFER_ARB,
	WGL_STEREO_ARB,
	WGL_PIXEL_TYPE_ARB,
	WGL_COLOR_BITS_ARB,
	WGL_RED_BITS_ARB,
	WGL_RED_SHIFT_ARB,
	WGL_GREEN_BITS_ARB,
	WGL_GREEN_SHIFT_ARB,
	WGL_BLUE_BITS_ARB,
	WGL_BLUE_SHIFT_ARB,
	WGL_ALPHA_BITS_ARB,
	WGL_ALPHA_SHIFT_ARB,
	WGL_ACCUM_BITS_ARB,
	WGL_ACCUM_RED_BITS_ARB,
	WGL_ACCUM_GREEN_BITS_ARB,
	WGL_ACCUM_BLUE_BITS_ARB,
	WGL_ACCUM_ALPHA_BITS_ARB,
	WGL_DEPTH_BITS_ARB,
	WGL_STENCIL_BITS_ARB,
	WGL_AUX_BUFFERS_ARB,
	WGL_DRAW_TO_PBUFFER_ARB,
	WGL_SAMPLE_BUFFERS_EXT,
	WGL_SAMPLES_EXT,
};
static int pfIntAttribCount = sizeof(pfIntAttribs) / sizeof(pfIntAttribs[0]);

LDVExtensionsSetup::LDVExtensionsSetupCleanup::~LDVExtensionsSetupCleanup(void)
{
	delete LDVExtensionsSetup::wglExtensions;
	LDVExtensionsSetup::wglExtensions = NULL;
	//delete LDVExtensionsSetup::glExtensions;
	//LDVExtensionsSetup::glExtensions = NULL;
	if (LDVExtensionsSetup::fsaaModes)
	{
		LDVExtensionsSetup::fsaaModes->release();
	}
	if (LDVExtensionsSetup::extensionsSetup)
	{
		LDVExtensionsSetup::extensionsSetup->closeWindow();
		LDVExtensionsSetup::extensionsSetup->release();
		LDVExtensionsSetup::extensionsSetup = NULL;
	}
	if (LDVExtensionsSetup::pfIntValues)
	{
		LDVExtensionsSetup::pfIntValues->release();
		LDVExtensionsSetup::pfIntValues = NULL;
	}
}


LDVExtensionsSetup::LDVExtensionsSetup(HINSTANCE hInstance):
	CUIOGLWindow("", hInstance, 100, 100, 100, 100)
{
}

LDVExtensionsSetup::~LDVExtensionsSetup(void)
{
}

BOOL LDVExtensionsSetup::initWindow(void)
{
	if (CUIOGLWindow::initWindow())
	{
		GLint intValue;

		TREGLExtensions::setup();
		if (!wglExtensions)
		{
			wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)
				wglGetProcAddress("wglGetExtensionsStringARB");

			if (!wglGetExtensionsStringARB)
			{
				wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)
					wglGetProcAddress("wglGetExtensionsStringEXT");
			}
			if (wglGetExtensionsStringARB)
			{
				wglExtensions = copyString(wglGetExtensionsStringARB(hdc));
			}
		}
/*
		if (!glExtensions)
		{
			glExtensions = copyString((char*)glGetString(GL_EXTENSIONS));
		}
*/
		// Note that when we load the function pointers, don't want to pay
		// attention to any ignore flags in the registry, so all the checks for
		// extensions have the force flag set to true.  Otherwise, if the
		// program starts with the ignore flag set, and it later gets cleared,
		// the function pointers won't be loaded.
		if (checkForWGLExtension("WGL_ARB_pixel_format", true))
		{
			wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVEXTPROC)
				wglGetProcAddress("wglGetPixelFormatAttribivARB");
			wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVEXTPROC)
				wglGetProcAddress("wglGetPixelFormatAttribfvARB");
			wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATEXTPROC)
				wglGetProcAddress("wglChoosePixelFormatARB");
		}
		if (havePixelBufferExtension(true))
		{
			wglCreatePbufferARB = (PFNWGLCREATEPBUFFERARBPROC)
				wglGetProcAddress("wglCreatePbufferARB");
			wglGetPbufferDCARB = (PFNWGLGETPBUFFERDCARBPROC)
				wglGetProcAddress("wglGetPbufferDCARB");
			wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)
				wglGetProcAddress("wglReleasePbufferDCARB");
			wglDestroyPbufferARB = (PFNWGLDESTROYPBUFFERARBPROC)
				wglGetProcAddress("wglDestroyPbufferARB");
			wglQueryPbufferARB = (PFNWGLQUERYPBUFFERARBPROC)
				wglGetProcAddress("wglQueryPbufferARB");
		}
		if (haveVARExtension(true))
		{
			wglAllocateMemoryNV = (PFNWGLALLOCATEMEMORYNVPROC)
				wglGetProcAddress("wglAllocateMemoryNV");
			wglFreeMemoryNV = (PFNWGLFREEMEMORYNVPROC)
				wglGetProcAddress("wglFreeMemoryNV");
			TREVertexStore::setWglAllocateMemoryNV(wglAllocateMemoryNV);
			TREVertexStore::setWglFreeMemoryNV(wglFreeMemoryNV);
		}
/*
		if (haveMultiDrawArraysExtension(true))
		{
			glMultiDrawElementsEXT = (PFNGLMULTIDRAWELEMENTSEXTPROC)
				wglGetProcAddress("glMultiDrawElementsEXT");
			TREShapeGroup::setGlMultiDrawElementsEXT(glMultiDrawElementsEXT);
		}
		if (haveVBOExtension(true))
		{
			glBindBufferARB = (PFNGLBINDBUFFERARBPROC)
				wglGetProcAddress("glBindBufferARB");
			glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)
				wglGetProcAddress("glDeleteBuffersARB");
			glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)
				wglGetProcAddress("glGenBuffersARB");
			glIsBufferARB = (PFNGLISBUFFERARBPROC)
				wglGetProcAddress("glIsBufferARB");
			glBufferDataARB = (PFNGLBUFFERDATAARBPROC)
				wglGetProcAddress("glBufferDataARB");
			glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)
				wglGetProcAddress("glBufferSubDataARB");
			glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)
				wglGetProcAddress("glGetBufferSubDataARB");
			glMapBufferARB = (PFNGLMAPBUFFERARBPROC)
				wglGetProcAddress("glMapBufferARB");
			glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)
				wglGetProcAddress("glUnmapBufferARB");
			glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)
				wglGetProcAddress("glGetBufferParameterivARB");
			glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)
				wglGetProcAddress("glGetBufferPointervARB");
			TREVertexStore::setGlBindBufferARB(glBindBufferARB);
			TREVertexStore::setGlDeleteBuffersARB(glDeleteBuffersARB);
			TREVertexStore::setGlGenBuffersARB(glGenBuffersARB);
			TREVertexStore::setGlBufferDataARB(glBufferDataARB);
		}
*/
		glGetIntegerv(GL_STENCIL_BITS, &intValue);
		if (intValue)
		{
			stencilPresent = true;
		}
		glGetIntegerv(GL_ALPHA_BITS, &intValue);
		if (intValue)
		{
			alphaPresent = true;
		}
		recordPixelFormats();
		scanFSAAModes();
		return TRUE;
	}
	return FALSE;
}

void LDVExtensionsSetup::recordPixelFormats(void)
{
	if (havePixelFormatExtension() && !pfIntValues)
	{
		GLint intValues[] = {
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 16,
			WGL_DEPTH_BITS_ARB, 16,
			0, 0
		};
		GLfloat floatValues[] = { 0.0f, 0.0f };
		GLint indexes[1024];
		GLuint count;

		pfIntValues = new TCIntArrayArray;
		// Get a list of all acceptable pixel formats.
		if (wglChoosePixelFormatARB(hdc, intValues,
			floatValues, sizeof(indexes) / sizeof(indexes[0]), indexes, &count)
			&& count)
		{
			GLuint i;
			GLint *values = new GLint[pfIntAttribCount];

			// Record info about all pixel formats.
			for (i = 0; i < count; i++)
			{
				TCIntArray *valueArray = new TCIntArray;
				int j;

				memset(values, -1, pfIntAttribCount * sizeof(GLint));
				wglGetPixelFormatAttribivARB(hdc, indexes[i], 0,
					pfIntAttribCount, pfIntAttribs, values);
				for (j = 0; j < pfIntAttribCount; j++)
				{
					valueArray->addValue(values[j]);
				}
				valueArray->addValue(indexes[i]);
				pfIntValues->addObject(valueArray);
				valueArray->release();
			}
			delete values;
		}
	}
}

void LDVExtensionsSetup::scanFSAAModes(void)
{
	if (fsaaModes)
	{
		fsaaModes->removeAll();
	}
	else
	{
		fsaaModes = new TCIntArray;
	}
	if (haveMultisampleExtension())
	{
		GLint intValues[] = {
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 16,
			WGL_DEPTH_BITS_ARB, 16,
			0, 0
		};
		GLfloat floatValues[] = { 0.0f, 0.0f };
		GLint indexes[1024];
		GLuint count;

		// Get a list of all acceptable pixel formats.
		if (wglChoosePixelFormatARB(hdc, intValues,
			floatValues, sizeof(indexes) / sizeof(indexes[0]), indexes, &count)
			&& count)
		{
			GLuint i;
			GLint attributes[2] = { WGL_SAMPLE_BUFFERS_EXT, WGL_SAMPLES_EXT };
			GLint values[2];

			// Scan the list for multisample pixel formats.
			for (i = 0; i < count; i++)
			{
				wglGetPixelFormatAttribivARB(hdc, indexes[i], 0, 2, attributes,
					values);
				if (values[0] && values[1] > 1)
				{
					int value = values[1];

					if (fsaaModes->indexOfValue(value) == -1)
					{
						fsaaModes->addValue(value);
					}
				}
			}
		}
		fsaaModes->sort();
	}
}

bool LDVExtensionsSetup::haveMultisampleExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_MULTISAMPLE_KEY, 0, false)
		!= 0;

	return (!ignore || force) && checkForWGLExtension("WGL_ARB_multisample");
}

bool LDVExtensionsSetup::havePixelBufferExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_PBUFFER_KEY, 0, false) != 0;

	return (!ignore || force) && checkForWGLExtension("WGL_ARB_pbuffer", force);
}

bool LDVExtensionsSetup::haveVARExtension(bool force)
{
	return TREGLExtensions::haveVARExtension(force) &&
		checkForWGLExtension("WGL_NV_allocate_memory");
}

/*
bool LDVExtensionsSetup::haveNvMultisampleFilterHintExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_MS_FILTER_HINT_KEY, 0,
		false) != 0;

	return (!ignore || force) &&
		checkForExtension("GL_NV_multisample_filter_hint");
}

bool LDVExtensionsSetup::haveVARExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_VAR_KEY, 0, false) != 0;

	return (!ignore || force) && checkForExtension("GL_NV_vertex_array_range")
		&& checkForWGLExtension("WGL_NV_allocate_memory");
}

bool LDVExtensionsSetup::haveMultiDrawArraysExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_MULTI_DRAW_ARRAYS_KEY, 0,
		false) != 0;

	return (!ignore || force) && checkForExtension("GL_EXT_multi_draw_arrays");
}

bool LDVExtensionsSetup::haveVBOExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_VBO_KEY, 0, false) != 0;

	return (!ignore || force) &&
		checkForExtension("GL_ARB_vertex_buffer_object");
}
*/

bool LDVExtensionsSetup::havePixelFormatExtension(bool force)
{
	bool ignore = TCUserDefaults::longForKey(IGNORE_PIXEL_FORMAT_KEY, 0, false)
		!= 0;

	return (!ignore || force) && checkForWGLExtension("WGL_ARB_pixel_format");
}

/*
bool LDVExtensionsSetup::checkForExtension(char* extensionsString,
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

bool LDVExtensionsSetup::checkForExtension(char* extension, bool force)
{
	return checkForExtension(glExtensions, extension, force);
}
*/

bool LDVExtensionsSetup::checkForWGLExtension(char* extension, bool force)
{
	return TREGLExtensions::checkForExtension(wglExtensions, extension, force);
}

void LDVExtensionsSetup::closeWindow(void)
{
	if (hWindow)
	{
		ShowWindow(hWindow, SW_HIDE);
	}
	CUIOGLWindow::closeWindow();
}

void LDVExtensionsSetup::setup(HINSTANCE hInstance)
{
	if (!performedInitialSetup)
	{
		extensionsSetup = new LDVExtensionsSetup(hInstance);
		extensionsSetup->initWindow();
		performedInitialSetup = true;
	}
}

int LDVExtensionsSetup::matchPixelFormat(int *intValues)
{
	int count = pfIntValues->getCount();
	int i;

	for (i = 0; i < count; i++)
	{
		int index = pixelFormatMatches(i, intValues);

		if (index != -1)
		{
			return index;
		}
	}
	return -1;
}

int LDVExtensionsSetup::pixelFormatMatches(int index, int *intValues)
{
	if (pfIntValues)
	{
		int i, j;
		TCIntArray *pfValues = (*pfIntValues)[index];

		for (i = 0; intValues[i]; i += 2)
		{
			int attrib = intValues[i];
			int value = intValues[i + 1];

			if (value)
			{
				for (j = 0; j < pfIntAttribCount; j++)
				{
					if (pfIntAttribs[j] == attrib)
					{
						if ((*pfValues)[j] < value)
						{
							return -1;
						}
						break;
					}
				}
			}
		}
		return (*pfValues)[pfIntAttribCount];
	}
	else
	{
		return -1;
	}
}

int LDVExtensionsSetup::choosePixelFormat(HDC hdc, GLint customValues[])
{
	GLint commonValues[] = {
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 16,
		WGL_DEPTH_BITS_ARB, 16,
		0, 0
	};
	GLint *intValues;
	GLfloat floatValues[] = { 0.0f, 0.0f };
	GLint indexes[100];
	GLuint count;
	int customSize;
	int retValue = -1;
	int i;

	for (count = 0; customValues[count]; count++)
	{
		// Do nothing; we just want to know how many values there are.
	}
	customSize = count * sizeof(GLint);
	intValues = new GLint[customSize + sizeof(commonValues)];
	memcpy(intValues, customValues, customSize);
	memcpy(intValues + count, commonValues, sizeof(commonValues));
	for (i = 0; intValues[i]; i += 2)
	{
		debugPrintf("%d %d\n", intValues[i], intValues[i + 1]);
	}
	debugPrintf("%d %d\n", intValues[i], intValues[i + 1]);
	for (i = 0; floatValues[i]; i += 2)
	{
		debugPrintf("%g %g\n", floatValues[i], floatValues[i + 1]);
	}
	debugPrintf("%g %g\n", floatValues[i], floatValues[i + 1]);
	if (wglChoosePixelFormatARB(hdc, intValues,
		floatValues, 100, indexes, &count))
	{
		if (count)
		{
			printPixelFormats(indexes, count);
			printPixelFormat(hdc, indexes[0]);
			retValue = indexes[0];
		}
		else
		{
			// There seems to be a bug in my ATI drivers that causes
			// wglChoosePixelFormatARB to stop working, so use my own matching
			// code if the standard matching code fails.
			retValue = matchPixelFormat(intValues);
			debugPrintf("matchPixelFormat returned: %d\n", retValue);
			printPixelFormat(hdc, indexes[0]);
		}
	}
	delete intValues;
	return retValue;
}

void LDVExtensionsSetup::printPixelFormats(int *indexes, GLuint count)
{
	debugPrintf("Got %d pixel formats.\n", count);
	for (unsigned int i = 0; i < count; i++)
	{
		debugPrintf("%5d", indexes[i]);
	}
	debugPrintf("\n");
}

void LDVExtensionsSetup::printPixelFormat(HDC hdc, int index)
{
	int i;
	int values[1024];

	memset(values, -1, sizeof(values));
	wglGetPixelFormatAttribivARB(hdc, index, 0, pfIntAttribCount, pfIntAttribs,
		values);
	for (i = 0; i < pfIntAttribCount; i++)
	{
		printf("%31s: 0x%08X %d\n", pfIntAttribNames[i], values[i], values[i]);
	}
}
