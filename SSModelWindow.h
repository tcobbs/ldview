#ifndef __SSMODELWINDOW_H__
#define __SSMODELWINDOW_H__

#include "ModelWindow.h"

class LDViewWindow;

class SSModelWindow : public ModelWindow
{
public:
	SSModelWindow(CUIWindow*, int, int, int, int);
	virtual int loadModel(void);
	virtual void finalSetup(void);
	virtual bool checkForExit(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);
protected:
	virtual ~SSModelWindow(void);
	virtual void dealloc(void);
	virtual void doPaint(void);
	virtual BOOL showProgress(void);
	virtual void hideProgress(void);
	virtual int progressCallback(char* message, float progress);
	virtual void verifySsFilename(void);
	virtual void swapBuffers(void);
	virtual void reallySwapBuffers(void);
	virtual void showErrorsIfNeeded(BOOL onlyIfNeeded);
	virtual LRESULT windowProc(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam);
	virtual void closeWindow(void);
	virtual LRESULT doSize(WPARAM sizeType, int newWidth, int newHeight);
	void initSSFilename(void);
	void getMatchingFiles(char *dir, char *filespec, TCStringArray *filenames);

	BOOL shownOnce;
	int screenWidth;
	int screenHeight;
	long ssSize;
	long ssSpeed;
	long ssRotationSpeed;
	ULONGLONG startTick;
	int powerSaveTimeout;
	ucstring ssFilename;
	long ssFileMode;
	bool ssSleepWorkaround;
	bool ssRandomPrefSet;
};

#endif // __SSMODELWINDOW_H__