#ifndef __LDVEXTENSIONSSETUP_H__
#define __LDVEXTENSIONSSETUP_H__

#include <CUI/CUIOGLWindow.h>
#include <GL/glext.h>
#include <GL/wglext.h>
#include <TCFoundation/TCTypedValueArray.h>
#include <TCFoundation/TCTypedObjectArray.h>

typedef TCTypedObjectArray<TCIntArray> TCIntArrayArray;

#ifndef GL_MULTISAMPLE_FILTER_HINT_NV
/* NV_multisample_filter_hint */
#define GL_MULTISAMPLE_FILTER_HINT_NV     0x8534
#endif

class LDVExtensionsSetup : public CUIOGLWindow
{
public:
	static void setup(HINSTANCE hInstance);
	static PFNWGLGETPIXELFORMATATTRIBIVEXTPROC
		getWglGetPixelFormatAttribivARB(void)
	{
		return wglGetPixelFormatAttribivARB;
	}
	static PFNWGLGETPIXELFORMATATTRIBFVEXTPROC
		getWglGetPixelFormatAttribfvARB(void)
	{
		return wglGetPixelFormatAttribfvARB;
	}
	static PFNWGLCHOOSEPIXELFORMATEXTPROC getWglChoosePixelFormatARB(void)
	{
		return wglChoosePixelFormatARB;
	}
	static PFNWGLCREATEPBUFFERARBPROC getWglCreatePbufferARB(void)
	{
		return wglCreatePbufferARB;
	}
	static PFNWGLGETPBUFFERDCARBPROC getWglGetPbufferDCARB(void)
	{
		return wglGetPbufferDCARB;
	}
	static PFNWGLRELEASEPBUFFERDCARBPROC getWglReleasePbufferDCARB(void)
	{
		return wglReleasePbufferDCARB;
	}
	static PFNWGLDESTROYPBUFFERARBPROC getWglDestroyPbufferARB(void)
	{
		return wglDestroyPbufferARB;
	}
	static PFNWGLQUERYPBUFFERARBPROC getWglQueryPbufferARB(void)
	{
		return wglQueryPbufferARB;
	}
	static PFNWGLALLOCATEMEMORYNVPROC getWglAllocateMemoryNV(void)
	{
		return wglAllocateMemoryNV;
	}
	static PFNWGLFREEMEMORYNVPROC getWglFreeMemoryNV(void)
	{
		return wglFreeMemoryNV;
	}
	static PFNGLVERTEXARRAYRANGENVPROC getGlVertexArrayRangeNV(void)
	{
		return glVertexArrayRangeNV;
	}
	static PFNGLMULTIDRAWELEMENTSEXTPROC getGlMultiDrawElementsEXT(void)
	{
		return glMultiDrawElementsEXT;
	}
	static PFNGLBINDBUFFERARBPROC getGlBindBufferARB(void)
	{
		return glBindBufferARB;
	}
	static PFNGLDELETEBUFFERSARBPROC getGlDeleteBuffersARB(void)
	{
		return glDeleteBuffersARB;
	}
	static PFNGLGENBUFFERSARBPROC getGlGenBuffersARB(void)
	{
		return glGenBuffersARB;
	}
	static PFNGLISBUFFERARBPROC getGlIsBufferARB(void)
	{
		return glIsBufferARB;
	}
	static PFNGLBUFFERDATAARBPROC getGlBufferDataARB(void)
	{
		return glBufferDataARB;
	}
	static PFNGLBUFFERSUBDATAARBPROC getGlBufferSubDataARB(void)
	{
		return glBufferSubDataARB;
	}
	static PFNGLGETBUFFERSUBDATAARBPROC getGlGetBufferSubDataARB(void)
	{
		return glGetBufferSubDataARB;
	}
	static PFNGLMAPBUFFERARBPROC getGlMapBufferARB(void)
	{
		return glMapBufferARB;
	}
	static PFNGLUNMAPBUFFERARBPROC getGlUnmapBufferARB(void)
	{
		return glUnmapBufferARB;
	}
	static PFNGLGETBUFFERPARAMETERIVARBPROC getGlGetBufferParameterivARB(void)
	{
		return glGetBufferParameterivARB;
	}
	static PFNGLGETBUFFERPOINTERVARBPROC getGlGetBufferPointervARB(void)
	{
		return glGetBufferPointervARB;
	}

	static bool checkForWGLExtension(char* extension, bool force = false);
	static bool checkForExtension(char* extension, bool force = false);
	static bool checkForExtension(char* extensionsString, char* extension,
		bool force = false);
	static bool haveMultisampleExtension(bool force = false);
	static bool havePixelBufferExtension(bool force = false);
	static bool haveNvMultisampleFilterHintExtension(bool force = false);
	static bool havePixelFormatExtension(bool force = false);
	static bool haveVARExtension(bool force = false);
	static bool haveMultiDrawArraysExtension(bool force = false);
	static bool haveVBOExtension(bool force = false);
	static char *getWglExtensions(void) { return wglExtensions; }
	static bool haveStencil(void) { return stencilPresent; }
	static bool haveAlpha(void) { return alphaPresent; }
	static int choosePixelFormat(HDC hdc, GLint customValues[]);
	static void printPixelFormats(int *indexes, GLuint count);
	static void printPixelFormat(HDC hdc, int index);
	static TCIntArray *getFSAAModes(void) { return fsaaModes; }
protected:
	LDVExtensionsSetup(HINSTANCE hInstance);
	virtual ~LDVExtensionsSetup(void);
	virtual BOOL initWindow(void);
	virtual void closeWindow(void);
	virtual void scanFSAAModes(void);
	virtual void recordPixelFormats(void);

	static int matchPixelFormat(int *intValues);
	static int pixelFormatMatches(int index, int *intValues);

	// WGL_EXT_pixel_format
	static PFNWGLGETPIXELFORMATATTRIBIVEXTPROC wglGetPixelFormatAttribivARB;
	static PFNWGLGETPIXELFORMATATTRIBFVEXTPROC wglGetPixelFormatAttribfvARB;
	static PFNWGLCHOOSEPIXELFORMATEXTPROC wglChoosePixelFormatARB;
	// WGL_ARB_extensions_string
	static PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
	// WGL_ARB_pbuffer
	static PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB;
	static PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB;
	static PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
	static PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB;
	static PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB;
	// WGL_NV_allocate_memory
	static PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV;
	static PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV;
	// GL_NV_vertex_array_range
	static PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;
	// GL_EXT_multi_draw_arrays
	static PFNGLMULTIDRAWELEMENTSEXTPROC glMultiDrawElementsEXT;
	// GL_ARB_vertex_buffer_object
	static PFNGLBINDBUFFERARBPROC glBindBufferARB;
	static PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
	static PFNGLGENBUFFERSARBPROC glGenBuffersARB;
	static PFNGLISBUFFERARBPROC glIsBufferARB;
	static PFNGLBUFFERDATAARBPROC glBufferDataARB;
	static PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;
	static PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB;
	static PFNGLMAPBUFFERARBPROC glMapBufferARB;
	static PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;
	static PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
	static PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB;

	static char *wglExtensions;
	static char *glExtensions;
	static bool performedInitialSetup;
	static bool stencilPresent;
	static bool alphaPresent;
	static TCIntArray *fsaaModes;
	static TCIntArrayArray *pfIntValues;
	static LDVExtensionsSetup *extensionsSetup;

	static class LDVExtensionsSetupCleanup
	{
	public:
		~LDVExtensionsSetupCleanup(void);
	} extensionsSetupCleanup;
	friend class LDVExtensionsSetupCleanup;
};

#endif // __LDVEXTENSIONSSETUP_H__