#include "LDLError.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStringArray.h>

LDLError::LDLError(LDLErrorType type, const char *message, const char *filename,
				   const char *fileLine, int lineNumber,
				   TCStringArray *extraInfo)
	:TCAlert(LDLError::alertClass(), message, extraInfo),
	m_type(type),
	m_filename(copyString(filename)),
	m_fileLine(copyString(fileLine)),
	m_lineNumber(lineNumber),
	m_level(1)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "LDLError");
#endif
}

LDLError::~LDLError(void)
{
}

void LDLError::dealloc(void)
{
	delete m_filename;
	delete m_fileLine;
	TCAlert::dealloc();
}

const char *LDLError::getTypeName(void)
{
	switch (m_type)
	{
	case LDLEGeneral:
		return "General error";
	case LDLEParse:
		return "Parse error";
	case LDLEFileNotFound:
		return "File not found error";
	case LDLEMatrix:
		return "Degenerate matrix error";
	case LDLEPartDeterminant:
		return "Part transformed non-uniformly error";
	case LDLENonFlatQuad:
		return "Non-flat quad error";
	case LDLEConcaveQuad:
		return "Concave quad warning";
	case LDLEConcaveQuadSplit:
		return "Error splitting concave quad";
	case LDLEOpenGL:
		return "OpenGL error";
	case LDLEColor:
		return "Error parsing color";
	case LDLEColinear:
		return "Co-linear points warning";
	case LDLEBFCWarning:
		return "BFC warning";
	case LDLEBFCError:
		return "BFC error";
	case LDLEMPDError:
		return "MPD error";
	default:
		return "Unknown error";
	}
}