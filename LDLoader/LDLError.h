#ifndef __LDLERROR_H__
#define __LDLERROR_H__

#include <TCFoundation/TCObject.h>
#include <stdlib.h>

class TCStringArray;

typedef enum
{
	LDLEParse,
	LDLEFileNotFound,
	LDLEPartDeterminant,
	LDLEConcaveQuad,
	LDLEConcaveQuadSplit,
	LDLEOpenGL,
	LDLEColor,
	LDLEColinear,
	LDLEBFCWarning,
	LDLEBFCError
} LDLErrorType;

class LDLError: public TCObject
{
	public:
		LDLError(LDLErrorType type, const char *message, const char *filename,
			const char *fileLine, int lineNumber,
			TCStringArray *extraInfo = NULL);
		LDLErrorType getType(void) { return type; }
		char *getMessage(void) { return message; }
		char *getFilename(void) { return filename; }
		char *getFileLine(void) { return fileLine; }
		int getLineNumber(void) { return lineNumber; }
		TCStringArray *getExtraInfo(void) { return extraInfo; }
	protected:
		virtual ~LDLError(void);
		virtual void dealloc(void);

		LDLErrorType type;
		char *message;
		char *filename;
		char *fileLine;
		int lineNumber;
		TCStringArray *extraInfo;
};

#endif // __LDLERROR_H__
