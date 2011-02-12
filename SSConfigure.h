#ifndef __SSCONFIGURE_H__
#define __SSCONFIGURE_H__

#include "LDViewPreferences.h"

#define DEFAULT_SS_SIZE 250
#define DEFAULT_SS_SPEED 5
#define DEFAULT_SS_ROT_SPEED 100
#define DEFAULT_SS_FILE_MODE 0
#define DEFAULT_SS_SLEEP_WORKAROUND false
#define DEFAULT_SS_RANDOM_PREF_SET false

class SSConfigure: public LDViewPreferences
{
public:
	SSConfigure(HINSTANCE);
	virtual INT_PTR run(void);

	static char *defaultFilename(void);
	static char *defaultDir(void);
	static char *getInstallPath(void);
protected:
	virtual ~SSConfigure(void);
	virtual void dealloc(void);
	void setupSSPage(void);
//	void setupLDViewPage(void);
	BOOL doDialogNotify(HWND hDlg, int controlId, LPNMHDR notification);
	void setupSlider(int controlId, short min, short max, WORD frequency,
		int value);
//	BOOL doDialogCommand(HWND hDlg, int controlId, int notifyCode,
//		HWND controlHWnd);
//	BOOL doDialogVScroll(HWND hDlg, int scrollCode, int position,
//		HWND hScrollBar);
	virtual void applyChanges(void);
	virtual bool doBrowseFilename(void);
	virtual bool doBrowseDirectory(void);
	virtual void setupPage(int pageNumber);
//	virtual bool doApply(void);
	virtual void setupGeneralPage(void);
	virtual void setupPrefSetsPage(void);
	virtual void enableCutaway(void);
	virtual void disableCutaway(void);
	virtual void enableStereo(void);
	virtual void disableStereo(void);
	void loadDefaultSSSettings(void);
	void loadSSSettings(void);
//	virtual float getMaxFov(void);
	virtual DWORD getPageDialogID(HWND hDlg);
	virtual DWORD doClick(HWND hPage, int controlId, HWND controlHWnd);
	virtual void updateFileControls(bool includePaths = true);
	static int CALLBACK pathBrowserCallback(HWND hwnd, UINT uMsg, LPARAM lParam,
		LPARAM lpData);

	HWND hSSPage;
	char *ssFilename;
	char *ssDirectory;
	long ssSize;
	long ssSpeed;
	long ssRotationSpeed;
	long ssFileMode;
	bool ssSleepWorkaround;
	bool ssRandomPrefSet;
};

#endif __SSCONFIGURE_H__
