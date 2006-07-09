#include "TCUnzip.h"
#include "mystring.h"

#ifdef WIN32
#include <UnzipWinDll/structs.h>
#include <UnzipWinDll/decs.h>

int WINAPI passwordFunc(char *, int, const char *, const char *);
int WINAPI printFunc(LPSTR, unsigned long);
int WINAPI replaceFunc(char *);
void WINAPI receiveZipMessage(unsigned long, unsigned long, unsigned,
    unsigned, unsigned, unsigned, unsigned, unsigned,
    char, LPSTR, LPSTR, unsigned long, char);

#else // WIN32
#ifdef UNZIP_CMD
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#endif // UNZIP_CMD
#endif // WIN32


TCUnzip::TCUnzip(void)
{
}

TCUnzip::~TCUnzip(void)
{
}

void TCUnzip::dealloc(void)
{
	TCObject::dealloc();
}

int TCUnzip::unzip(const char *filename, const char *outputDir)
{
#ifdef WIN32
	return unzipWin32(filename, outputDir);
#else // WIN32
#ifdef UNZIP_CMD
	return unzipExec(filename, outputDir);
#else
	return -1;
#endif // UNZIP_CMD
#endif // WIN32
}

#ifdef WIN32

int TCUnzip::unzipWin32(const char *filename, const char *outputDir)
{
	DCL dcl;
	USERFUNCTIONS userfunctions;
	char filenameBuf[2048];
	char outputDirBuf[2048];

	memset(&dcl, 0, sizeof(dcl));
	memset(&userfunctions, 0, sizeof(userfunctions));
	strncpy(filenameBuf, filename, sizeof(filenameBuf));
	filenameBuf[sizeof(filenameBuf) - 1] = 0;
	if (outputDir)
	{
		strncpy(outputDirBuf, outputDir, sizeof(outputDirBuf));
		outputDirBuf[sizeof(outputDirBuf) - 1] = 0;
	}
	dcl.ncflag = 0; // Write to stdout if true
	dcl.fQuiet = 0; // We want all messages.
					//  1 = fewer messages,
					//  2 = no messages
	dcl.ntflag = 0; // test zip file if true
	dcl.nvflag = 0; // give a verbose listing if true
	dcl.nzflag = 0; // display a zip file comment if true
	dcl.ndflag = 1; // Recreate directories != 0, skip "../" if < 2
	dcl.naflag = 0; // Do not convert CR to CRLF
	dcl.nfflag = 0; // Do not freshen existing files only
	dcl.noflag = 1; // Over-write all files if true
	dcl.ExtractOnlyNewer = 0; // Do not extract only newer
	dcl.PromptToOverwrite = 0; // "Overwrite all" selected -> no query mode
	dcl.SpaceToUnderscore = 0;
	dcl.lpszZipFN = filenameBuf; // The archive name

	userfunctions.password = passwordFunc;
	userfunctions.print = printFunc;
	userfunctions.sound = NULL;
	userfunctions.replace = replaceFunc;
	userfunctions.SendApplicationMessage = receiveZipMessage;

	if (outputDir)
	{
		dcl.lpszExtractDir = outputDirBuf;
	}
	else
	{
		dcl.lpszExtractDir = NULL;
	}
	return Wiz_SingleEntryUnzip(0, NULL, 0, NULL, &dcl, &userfunctions);
}

#else // WIN32
#ifdef UNZIP_CMD

void TCUnzip::unzipChildExec(const char *filename, const char *outputDir)
{
	char *path = getenv("PATH");
	char *unzipPath = NULL;
	int pathCount;
	char **pathComponents = componentsSeparatedByString(path, ":", pathCount);
	int i;

	for (i = 0; i < pathCount && unzipPath == NULL; i++)
	{
		FILE *file;

		unzipPath = copyString(pathComponents[i], 7);
		strcat(unzipPath, "/unzip");
		file = fopen(unzipPath, "r");
		if (file)
		{
			fclose(file);
		}
		else
		{
			delete unzipPath;
			unzipPath = NULL;
		}
	}
	deleteStringArray(pathComponents, pathCount);
	if (unzipPath)
	{
		char *zipFile = copyString(filename);

		if (chdir(outputDir) == 0)
		{
			char *const argv[] = {unzipPath, "-o", zipFile, NULL};

			// Squash the console output from the unzip program.
			freopen("/dev/null", "w", stdout);
			execv(unzipPath, argv);
		}
		delete zipFile;
		delete unzipPath;
	}
}

int TCUnzip::unzipExec(const char *filename, const char *outputDir)
{
	pid_t forkResult = fork();
	switch (forkResult)
	{
	case 0:
		// child process
		unzipChildExec(filename, outputDir);
		// If we get here at all, there was an error executing unzip.  Since
		// we've already forked, we have to exit with an error status.
		exit(1);
		break;
	case -1:
		// error
		return -1;
		break;
	default:
		// Parent process
		int status;
		wait(&status);
		if (status == 0)
		{
			return 0;
		}
		else
		{
			return -1;
		}
		break;
	}
	// We'll never get to the following, but we get a warning otherwise.
	return -1;
}

#endif // UNZIP_CMD
#endif // WIN32

#ifdef WIN32

int WINAPI passwordFunc(char *, int, const char *, const char *)
{
	return 1;
}

int WINAPI printFunc(LPSTR buf, unsigned long size)
{
	debugPrintf("%s", buf);
	return size;
}

int WINAPI replaceFunc(char *)
{
	return 1;
}

void WINAPI receiveZipMessage(unsigned long, unsigned long, unsigned,
    unsigned, unsigned, unsigned, unsigned, unsigned,
    char, LPSTR, LPSTR, unsigned long, char)
{
	// Don't care
}

#endif

bool TCUnzip::supported(void)
{
#if defined(WIN32) || defined(UNZIP_CMD)
	return true;
#else // WIN32 || UNZIP_CMD
	return false;
#endif // WIN32 || UNZIP_CMD
}
