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

	static bool checkForWGLExtension(char* extension);
	static bool checkForExtension(char* extension);
	static bool checkForExtension(char* extensionsString, char* extension);
	static bool haveMultisampleExtension(void);
	static bool havePixelBufferExtension(void);
	static bool haveNvMultisampleFilterHintExtension(void);
	static bool havePixelFormatExtension(void);
	static bool haveVARExtension(void);
	static bool haveMultiDrawArraysExtension(void);
	static char *getWglExtensions(void) { return wglExtensions; }
	static bool haveStencil(void) { return stencilPresent; }
	static bool haveAlpha(void) { return alphaPresent; }
	static int choosePixelFormat(HDC hdc, GLint customValues[]);
	static void printPixelFormats(int *indexes, GLuint count);
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

	static PFNWGLGETPIXELFORMATATTRIBIVEXTPROC wglGetPixelFormatAttribivARB;
	static PFNWGLGETPIXELFORMATATTRIBFVEXTPROC wglGetPixelFormatAttribfvARB;
	static PFNWGLCHOOSEPIXELFORMATEXTPROC wglChoosePixelFormatARB;
	static PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
	static PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB;
	static PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB;
	static PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB;
	static PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB;
	static PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB;
	static PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV;
	static PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV;
	static PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;
	static PFNGLMULTIDRAWELEMENTSEXTPROC glMultiDrawElementsEXT;

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
	friend LDVExtensionsSetupCleanup;
};

#endif // __LDVEXTENSIONSSETUP_H__