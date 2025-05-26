#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <map>
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
#include "BatchStlConverter.h" // Added for batch conversion
#include <iostream> // Added for error messages in batch mode

typedef std::map<std::string, std::string> StringMap;

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
	// IniFile can be specified on the command line; if so, don't load a
	// different one.
	if (!TCUserDefaults::isIniFileSet())
	{
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
				printf("Error setting INI File to %s or %s\n", rcFilename,
				    rcFilename2);
			}
			delete rcFilename;
			delete rcFilename2;
		}
		else
		{
			printf("HOME environment variable not defined: cannot use "
				"~/.ldviewrc.\n");
		}
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
	if (OSMesaMakeCurrent(ctx, buffer, GL_UNSIGNED_BYTE, width, height))
	{
		GLint viewport[4] = {0};
		glGetIntegerv(GL_VIEWPORT, viewport);
		if (viewport[2] != width || viewport[3] != height)
		{
			printf("OSMesa not working!\n");
			printf("viewport: %d, %d, %d, %d\n", (int)viewport[0],
				(int)viewport[1], (int)viewport[2], (int)viewport[3]);
			free(buffer);
			buffer = NULL;
		}
	}
	else
	{
		printf("Error attaching buffer to context.\n");
		free(buffer);
		buffer = NULL;
	}
	return buffer;
}

bool dirExists(const std::string &path)
{
	struct stat buf;
	if (stat(path.c_str(), &buf) != 0)
	{
		return false;
	}
	return S_ISDIR(buf.st_mode);
}

bool fileOrDirExists(const std::string &path)
{
	struct stat buf;
	if (stat(path.c_str(), &buf) != 0)
	{
		return false;
	}
	return S_ISREG(buf.st_mode) || S_ISDIR(buf.st_mode);
}

bool findDirEntry(std::string &path)
{
	size_t lastSlash = path.rfind('/');
	if (lastSlash >= path.size())
	{
		return false;
	}
	std::string dirName = path.substr(0, lastSlash);
	std::string lowerFilename = lowerCaseString(path.substr(lastSlash + 1));
	DIR *dir = opendir(dirName.c_str());
	if (dir == NULL)
	{
		return false;
	}
	bool found = false;
	while (!found)
	{
		struct dirent *entry = readdir(dir);
		if (entry == NULL)
		{
			break;
		}
		std::string name = lowerCaseString(entry->d_name);
		if (name == lowerFilename)
		{
			path = dirName + '/' + entry->d_name;
			found = true;
		}
	}
	closedir(dir);
	return found;
}

bool fileCaseCallback(char *filename)
{
	StringMap s_pathMap;
	size_t count;
	char **components = componentsSeparatedByString(filename, "/", count);
	std::string lowerFilename = lowerCaseString(filename);

	StringMap::iterator it = s_pathMap.find(lowerFilename);
	if (it != s_pathMap.end())
	{
		strcpy(filename, it->second.c_str());
		return true;
	}
	if (count > 1)
	{
		bool ok = true;
		std::string builtPath = "/";
		for (size_t i = 1; i + 1 < count && ok; ++i)
		{
			builtPath += components[i];

			it = s_pathMap.find(builtPath);
			if (it != s_pathMap.end())
			{
				// Do nothing
			}
			else if (dirExists(builtPath))
			{
				s_pathMap[lowerCaseString(builtPath)] = builtPath;
			}
			else if (findDirEntry(builtPath))
			{
				s_pathMap[lowerCaseString(builtPath)] = builtPath;
				if (!dirExists(builtPath))
				{
					ok = false;
				}
			}
			else
			{
				ok = false;
			}
			if (ok)
			{
				builtPath += '/';
			}
		}
		if (ok)
		{
			builtPath += components[count - 1];
			if (findDirEntry(builtPath))
			{
				s_pathMap[lowerCaseString(builtPath)] = builtPath;
				ok = fileOrDirExists(builtPath);
			}
			else
			{
				ok = false;
			}
		}
		if (ok)
		{
			strcpy(filename, builtPath.c_str());
		}
		deleteStringArray(components, count);
		return ok;
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

	// Batch conversion mode check
	bool batchMode = TCUserDefaults::boolForKey("batch-convert-stl", false, false);
	if (batchMode)
	{
		std::string inputDir = TCUserDefaults::commandLineStringForKey("input_dir");
		std::string outputDir = TCUserDefaults::commandLineStringForKey("output_dir");

		if (inputDir.empty() || outputDir.empty())
		{
			std::cerr << "ERROR: Batch mode requires -input_dir and -output_dir arguments." << std::endl;
			delete[] stringTable; // Clean up allocated memory
			TCAutoreleasePool::processReleases();
			return 1;
		}

		BatchStlConverter converter(inputDir, outputDir);
		bool result = converter.runConversion();
		
		delete[] stringTable; // Clean up allocated memory
		TCAutoreleasePool::processReleases();
		return result ? 0 : 1;
	}
	else
	{
		// Proceed with normal OSMesa (snapshot taker) logic
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
	} // End of else (normal mode)
	delete[] stringTable; // Clean up allocated memory
	TCAutoreleasePool::processReleases();
	return 0;
}
