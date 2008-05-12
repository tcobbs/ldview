#ifndef __LDVEXTENSIONSSETUP_H__
#define __LDVEXTENSIONSSETUP_H__

#include <CUI/CUIOGLWindow.h>
#include <GL/glext.h>
#include <GL/wglext.h>
#include <TCFoundation/TCTypedValueArray.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <TCFoundation/TCStlIncludes.h>

typedef TCTypedObjectArray<TCIntArray> TCIntArrayArray;

typedef std::set<std::string> StringSet;

#ifndef GL_MULTISAMPLE_FILTER_HINT_NV
/* NV_multisample_filter_hint */
#define GL_MULTISAMPLE_FILTER_HINT_NV     0x8534
#endif

class LDVExtensionsSetup : public CUIOGLWindow
{
public:
	static void setup(HINSTANCE hInstance);
	static bool checkForWGLExtension(char* extension, bool force = false);
	static bool haveMultisampleExtension(bool force = false);
	static bool havePixelBufferExtension(bool force = false);
	static bool havePixelFormatExtension(bool force = false);
	//static bool haveVARExtension(bool force = false);
	static StringSet &getWglExtensions(void) { return sm_wglExtensions; }
	static const char *getWglExtensionsString(void)
	{
		return sm_wglExtensionsString;
	}
	static bool haveStencil(void) { return sm_stencilPresent; }
	static bool haveAlpha(void) { return sm_alphaPresent; }
	static int choosePixelFormat(HDC hdc, GLint customValues[]);
	static void printPixelFormats(int *indexes, GLuint count);
	static void printPixelFormat(HDC hdc, int index);
	static TCIntArray *getFSAAModes(void) { return sm_fsaaModes; }
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
	static PFNWGLGETPIXELFORMATATTRIBIVEXTPROC sm_wglGetPixelFormatAttribivARB;
	static PFNWGLGETPIXELFORMATATTRIBFVEXTPROC sm_wglGetPixelFormatAttribfvARB;
	static PFNWGLCHOOSEPIXELFORMATEXTPROC sm_wglChoosePixelFormatARB;
	// WGL_ARB_extensions_string
	static PFNWGLGETEXTENSIONSSTRINGARBPROC sm_wglGetExtensionsStringARB;
	// WGL_ARB_pbuffer
	static PFNWGLCREATEPBUFFERARBPROC sm_wglCreatePbufferARB;
	static PFNWGLGETPBUFFERDCARBPROC sm_wglGetPbufferDCARB;
	static PFNWGLRELEASEPBUFFERDCARBPROC sm_wglReleasePbufferDCARB;
	static PFNWGLDESTROYPBUFFERARBPROC sm_wglDestroyPbufferARB;
	static PFNWGLQUERYPBUFFERARBPROC sm_wglQueryPbufferARB;
	// WGL_NV_allocate_memory
	static PFNWGLALLOCATEMEMORYNVPROC sm_wglAllocateMemoryNV;
	static PFNWGLFREEMEMORYNVPROC sm_wglFreeMemoryNV;

	static StringSet sm_wglExtensions;
	static char *sm_wglExtensionsString;
	static bool sm_performedInitialSetup;
	static bool sm_stencilPresent;
	static bool sm_alphaPresent;
	static TCIntArray *sm_fsaaModes;
	static TCIntArrayArray *sm_pfIntValues;
	static LDVExtensionsSetup *sm_extensionsSetup;

	static class LDVExtensionsSetupCleanup
	{
	public:
		~LDVExtensionsSetupCleanup(void);
	} sm_extensionsSetupCleanup;
	friend class LDVExtensionsSetupCleanup;
};

#endif // __LDVEXTENSIONSSETUP_H__