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

#endif

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
#else // WIN32
	return -1;
#endif
}

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
#ifdef WIN32
	return true;
#else // WIN32
	return false;
#endif // WIN32
}
