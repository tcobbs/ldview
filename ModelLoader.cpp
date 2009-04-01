#include "ModelLoader.h"
#include "ModelWindow.h"
#include "SSModelWindow.h"
#include "LDViewWindow.h"
#include <LDLib/LDUserDefaultsKeys.h>
#include <LDLib/LDSnapshotTaker.h>
#include "LDVExtensionsSetup.h"

#include <TCFoundation/mystring.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/TCStringArray.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

#define TITLE _UC("LDView")

#define WIN_WIDTH 640
#define WIN_HEIGHT 480

ModelLoader::ModelLoader(HINSTANCE hInstance, int nCmdShow, bool screenSaver)
			:modelWindow(NULL),
			 parentWindow(NULL),
			 hInstance(hInstance),
			 nCmdShow(nCmdShow),
			 screenSaver(screenSaver)
{
	init();
}

ModelLoader::~ModelLoader(void)
{
}

void ModelLoader::dealloc(void)
{
	if (modelWindow)
	{
		modelWindow->release();
	}
	if (parentWindow)
	{
		parentWindow->release();
	}
	TCObject::dealloc();
}

void ModelLoader::init(void)
{
	startup();
}

void ModelLoader::update(void)
{
	modelWindow->update();
}

void ModelLoader::startup(void)
{
	int width;
	int height;
	int widthDelta = 0;
	bool maximized;
	TCStringArray *commandLine = TCUserDefaults::getProcessedCommandLine();
	HWND hParentWindow = NULL;

	TCUserDefaults::removeValue(HFOV_KEY, false);
	TCUserDefaults::removeValue(CAMERA_GLOBE_KEY, false);
	if (commandLine)
	{
		int i;
		int count = commandLine->getCount();

		for (i = 0; i < count; i++)
		{
			char *command = (*commandLine)[i];
			int num;

			if (stringHasCaseInsensitivePrefix(command, "-ca"))
			{
				float value;

				if (sscanf(command + 3, "%f", &value) == 1)
				{
					TCUserDefaults::setFloatForKey(value, HFOV_KEY, false);
				}
			}
			else if (stringHasCaseInsensitivePrefix(command, "-cg"))
			{
				TCUserDefaults::setStringForKey(command + 3, CAMERA_GLOBE_KEY,
					false);
			}
			else if (strcasecmp(command, "-float") == 0 && i + 1 < count &&
				sscanf((*commandLine)[i + 1], "%i", &num) == 1 && num != 0)
			{
				hParentWindow = (HWND)num;
			}
		}
	}
	width = TCUserDefaults::longForKey(WINDOW_WIDTH_KEY, WIN_WIDTH, false);
	height = TCUserDefaults::longForKey(WINDOW_HEIGHT_KEY, WIN_HEIGHT, false);
	maximized = TCUserDefaults::longForKey(WINDOW_MAXIMIZED_KEY, 0, false) != 0;
	parentWindow = new LDViewWindow(TITLE, hInstance, CW_USEDEFAULT,
		CW_USEDEFAULT, width + widthDelta, height);
	parentWindow->setMinWidth(320);
	parentWindow->setMinHeight(240);
	parentWindow->setScreenSaver(screenSaver);
	if (hParentWindow)
	{
		parentWindow->setHParentWindow(hParentWindow);
	}
	if (parentWindow->initWindow())
	{
		char *commandLineFilename = getCommandLineFilename();
		char *snapshotFilename =
			TCUserDefaults::stringForKey(SAVE_SNAPSHOT_KEY, NULL, false);
		bool savedSnapshot = false;
		bool saveSnapshots = TCUserDefaults::boolForKey(SAVE_SNAPSHOTS_KEY, 0,
			false);

		modelWindow = parentWindow->getModelWindow();
		modelWindow->retain();
		if (snapshotFilename)
		{
			char originalDir[MAX_PATH];
			char fullFilename[MAX_PATH];

			GetCurrentDirectory(sizeof(originalDir), originalDir);
			if (ModelWindow::chDirFromFilename(snapshotFilename, fullFilename))
			{
				delete snapshotFilename;
				snapshotFilename = copyString(fullFilename);
				SetCurrentDirectory(originalDir);
			}
		}
		if (!screenSaver && commandLineFilename &&
			(snapshotFilename || saveSnapshots))
		{
			if (modelWindow->setupOffscreen(1600, 1200,
				TCUserDefaults::longForKey(FSAA_MODE_KEY) > 0))
			{
				// Note: even if the snapshot save fails, we don't want to continue.
				// The user will get an error in the event of a snapshot save
				// failure.
				LDSnapshotTaker::doCommandLine();
				parentWindow->shutdown();
				savedSnapshot = true;
				modelWindow->cleanupOffscreen();
			}
		}
		if (!savedSnapshot)
		{
			parentWindow->showWindow(nCmdShow);
			if (maximized)
			{
				parentWindow->maximize();
			}
			if (!screenSaver)
			{
				if (commandLineFilename && snapshotFilename)
				{
					parentWindow->openModel(commandLineFilename);
					if (modelWindow->saveSnapshot(snapshotFilename, true))
					{
						parentWindow->shutdown();
					}
				}
				else
				{
					std::string stepString =
						TCUserDefaults::commandLineStringForKey(STEP_KEY);
					long step;

					parentWindow->openModel(commandLineFilename);
					if (commandLineFilename != NULL && stepString.size() > 0 &&
						sscanf(stepString.c_str(), "%li", &step) == 1)
					{
						parentWindow->setStep(step);
					}
				}
			}
		}
		delete commandLineFilename;
		delete snapshotFilename;
	}
}

char *ModelLoader::getCommandLineFilename(void)
{
	char *commandLineFilename = NULL;
	TCStringArray *commandLine = TCUserDefaults::getProcessedCommandLine();

	if (commandLine)
	{
		int i;
		int count = commandLine->getCount();

		for (i = 0; i < count && !commandLineFilename; i++)
		{
			char *arg = commandLine->stringAtIndex(i);

			if (arg[0] != '-')
			{
				commandLineFilename = arg;
			}
		}
	}
	return copyString(commandLineFilename);
}
