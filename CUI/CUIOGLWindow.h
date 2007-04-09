#ifndef __CUIOGLWINDOW_H__
#define __CUIOGLWINDOW_H__

#include <CUI/CUIWindow.h>
#include <GL/gl.h>

class CUIExport CUIOGLWindow : public CUIWindow
{
	public:
		CUIOGLWindow(void);
		CUIOGLWindow(CUCSTR, HINSTANCE, int, int, int, int);
		CUIOGLWindow(CUIWindow*, int, int, int, int);

		virtual void perspectiveView(void);
		void orthoView(void);
		void setFieldOfView(double fov, float nClip, float fClip);
		BOOL initWindow(void);
		BOOL isInitializedGL(void) { return initializedGL; }

		static const char *interpretGLError(GLenum errorCode);
	protected:
		~CUIOGLWindow(void);
		virtual void dealloc(void);
		void init(void);
		virtual BOOL setupPFD(void);
		virtual BOOL setPixelFormat(int pfIndex);
		virtual BOOL setupRenderingContext(void);
		virtual void initFail(char*);
		virtual void destroyWindow(void);
		virtual void makeCurrent(void);
		void initGL(void);
		virtual void setupLighting(void);
		virtual void setupLight(GLenum);
		virtual void setupMaterial(void);
		virtual void sizeView(void);
		virtual void drawLights(void);
		virtual void drawLight(GLenum, TCFloat, TCFloat, TCFloat);

		virtual LRESULT doSize(WPARAM, int, int);
		virtual LRESULT doCreate(HWND, LPCREATESTRUCT);
		virtual LRESULT doNCDestroy(void);
		virtual LRESULT doDestroy(void);

		int pfIndex;
		HGLRC hglrc;
		double fieldOfView;
		float nearClipPlane;
		float farClipPlane;
		BOOL initializedGL;
		BOOL hasParent;
};

#endif
