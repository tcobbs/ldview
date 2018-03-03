#ifndef __MODELLOADER_H__
#define __MODELLOADER_H__

#include <TCFoundation/TCObject.h>

#include <windows.h>

#define MAX_MODELS 16

class ModelWindow;
class LDViewWindow;
class TCAlert;

class ModelLoader: public TCObject
{
	public:
		ModelLoader(HINSTANCE, int, bool);
		void update(void);
	protected:
		~ModelLoader(void);
		virtual void dealloc(void);
		void init(void);
//		virtual void loadModel(void);
		virtual void startup(void);
		virtual char *getCommandLineFilename(void);
		void snapshotCallback(TCAlert *alert);
//		virtual HKEY openLDViewRegistryKey(void);

		ModelWindow* modelWindow;
		LDViewWindow* parentWindow;
		HINSTANCE hInstance;
		int nCmdShow;
		bool screenSaver;
		bool offscreenSetup;
};

#endif