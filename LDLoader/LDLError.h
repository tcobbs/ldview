#ifndef __LDLERROR_H__
#define __LDLERROR_H__

#include <TCFoundation/TCAlert.h>
#include <stdlib.h>

class TCStringArray;

typedef enum
{
	LDLEParse,
	LDLEFileNotFound,
	LDLEMatrix,
	LDLEPartDeterminant,
	LDLEConcaveQuad,
	LDLEConcaveQuadSplit,
	LDLEOpenGL,
	LDLEColor,
	LDLEColinear,
	LDLEBFCWarning,
	LDLEBFCError,
	LDLEMPDError
} LDLErrorType;

class LDLError: public TCAlert
{
public:
	LDLError(LDLErrorType type, const char *message, const char *filename,
		const char *fileLine, int lineNumber,
		TCStringArray *extraInfo = NULL);
	LDLErrorType getType(void) { return m_type; }
	char *getFilename(void) { return m_filename; }
	char *getFileLine(void) { return m_fileLine; }
	int getLineNumber(void) { return m_lineNumber; }
	static TCULong alertClass(void) { return 1; }
protected:
	virtual ~LDLError(void);
	virtual void dealloc(void);

	LDLErrorType m_type;
	char *m_filename;
	char *m_fileLine;
	int m_lineNumber;
};

#endif // __LDLERROR_H__
