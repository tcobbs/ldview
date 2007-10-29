#include <stdio.h>
#include <TCFoundation/TCUserDefaults.h>
#include <TCFoundation/mystring.h>
#include <LDLib/LDSnapshotTaker.h>
#include <TCFoundation/TCAutoreleasePool.h>

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

void setupContext(void)
{
}

int main(int argc, char *argv[])
{
	LDSnapshotTaker *snapshotTaker;

	setupDefaults(argv);
	snapshotTaker = new LDSnapshotTaker;
	setupContext();
	snapshotTaker->saveImage();
	TCAutoreleasePool::processReleases();
	return 0;
}
