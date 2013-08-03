#include <stdio.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/mystring.h>
#include <LDLib/LDSnapshotTaker.h>
#include <LDLoader/LDLModel.h>
#include <TCFoundation/TCAutoreleasePool.h>
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCLocalStrings.h>
#include <GL/osmesa.h>
#include <TRE/TREMainModel.h>
#include "StudLogo.h"
#include "LDViewMessages.h"

#define DEPTH_BPP 24
// Note: buffer contains only color buffer, not depth and stencil.
#define BYTES_PER_PIXEL 4

class ProgressHandler: public TCObject
{
public:
	ProgressHandler(void)
	{
		TCAlertManager::registerHandler(TCProgressAlert::alertClass(), this,
			(TCAlertCallback)&ProgressHandler::alertCallback);
	}
protected:
	~ProgressHandler(void)
	{
	}
	void dealloc(void)
	{
		TCAlertManager::unregisterHandler(this);
		TCObject::dealloc();
	}
	void alertCallback(TCProgressAlert *progress)
	{
		if (progress->getMessage() && strlen(progress->getMessage()))
		{
			printf("%s: %f%%\n", progress->getMessage(),
				progress->getProgress() * 100.0f);
		}
	}
};

void setupDefaults(char *argv[])
{
	TCUserDefaults::setCommandLine(argv);
	char *homeDir = getenv("HOME");

	if (homeDir)
	{
		char *rcFilename = copyString(homeDir, 128);

		strcat(rcFilename, "/.ldviewrc");

		char *rcFilename2 = copyString(homeDir, 128);

		strcat(rcFilename2, "/.config/LDView/ldviewrc");
		if (!TCUserDefaults::setIniFile(rcFilename) &&
		    !TCUserDefaults::setIniFile(rcFilename2))
		{
			printf("Error setting INI File to %s or %s\n", rcFilename,rcFilename2);
		}
		delete rcFilename;
		delete rcFilename2;
	}
	else
	{
		printf("HOME environment variable not defined: cannot use "
			"~/.ldviewrc.\n");
	}
	setDebugLevel(TCUserDefaults::longForKey("DebugLevel", 0, false));
}

void *setupContext(OSMesaContext &ctx)
{
	void *buffer = NULL;
	int width = TCUserDefaults::longForKey("TileWidth", 1024, false);
	int height = TCUserDefaults::longForKey("TileHeight", 1024, false);
	int tileSize = TCUserDefaults::longForKey("TileSize", -1, false);

	if (tileSize > 0)
	{
		width = height = tileSize;
	}
	ctx = OSMesaCreateContextExt(OSMESA_RGBA, DEPTH_BPP, 8, 0, NULL);
	if (!ctx)
	{
		printf("Error creating OSMesa context.\n");
		return NULL;
	}
	buffer = malloc(width * height * BYTES_PER_PIXEL * sizeof(GLubyte));
	if (!OSMesaMakeCurrent(ctx, buffer, GL_UNSIGNED_BYTE, width, height))
	{
		printf("Error attaching buffer to context.\n");
		free(buffer);
		buffer = NULL;
	}
	return buffer;
}

bool fileCaseCallback(char *filename)
{
	FILE *file;

	convertStringToLower(filename);
	file = fopen(filename, "r");
	if (file)
	{
		fclose(file);
		return true;
	}
	return false;
}

int main(int argc, char *argv[])
{
	void *buffer;
	OSMesaContext ctx;
	int stringTableSize = sizeof(LDViewMessages_bytes);
	char *stringTable = new char[sizeof(LDViewMessages_bytes) + 1];

	memcpy(stringTable, LDViewMessages_bytes, stringTableSize);
	stringTable[stringTableSize] = 0;
	TCLocalStrings::setStringTable(stringTable);
	setupDefaults(argv);
	if ((buffer = setupContext(ctx)) != NULL)
	{
		//ProgressHandler *progressHandler = new ProgressHandler;

		TREMainModel::setStudTextureData(StudLogo_bytes,
			sizeof(StudLogo_bytes));
		LDLModel::setFileCaseCallback(fileCaseCallback);
		LDSnapshotTaker::doCommandLine();
		OSMesaDestroyContext(ctx);
		free(buffer);
		//TCObject::release(progressHandler);
	}
	TCAutoreleasePool::processReleases();
	return 0;
}
