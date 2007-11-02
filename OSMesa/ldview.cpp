#include <stdio.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/mystring.h>
#include <LDLib/LDSnapshotTaker.h>
#include <TCFoundation/TCAutoreleasePool.h>
#include <GL/osmesa.h>
#include <TRE/TREMainModel.h>

#define BYTES_PER_PIXEL 5

void setupDefaults(char *argv[])
{
	TCUserDefaults::setCommandLine(argv);
	char *homeDir = getenv("HOME");

	if (homeDir)
	{
		char *rcFilename = copyString(homeDir, 128);

		strcat(rcFilename, "/.ldviewrc");
		if (!TCUserDefaults::setIniFile(rcFilename))
		{
			printf("Error setting INI File to %s\n", rcFilename);
		}
		delete rcFilename;
	}
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
	ctx = OSMesaCreateContextExt(OSMESA_RGBA, 23, 8, 0, NULL);
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

int main(int argc, char *argv[])
{
	void *buffer;
	OSMesaContext ctx;

	setupDefaults(argv);
	if ((buffer = setupContext(ctx)) != NULL)
	{
		char *logoFilename = copyString(TCUserDefaults::getAppPath(), 64);

		strcat(logoFilename, "StudLogo.png");
		TREMainModel::loadStudTexture(logoFilename);
		delete logoFilename;
		LDSnapshotTaker *snapshotTaker = new LDSnapshotTaker;
		snapshotTaker->saveImage();
		OSMesaDestroyContext(ctx);
		free(buffer);
	}
	TCAutoreleasePool::processReleases();
	return 0;
}
