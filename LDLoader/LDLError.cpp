#include "LDLError.h"
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCLocalStrings.h>

LDLError::LDLError(LDLErrorType type, const char *message, const char *filename,
				   const char *fileLine, int lineNumber,
				   TCStringArray *extraInfo)
	:TCAlert(LDLError::alertClass(), message, extraInfo),
	m_type(type),
	m_filename(copyString(filename)),
	m_fileLine(copyString(fileLine)),
	m_lineNumber(lineNumber),
	m_level(LDLAError),
	m_loadCanceled(false)
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
	return getTypeName(m_type);
}

const char *LDLError::getTypeName(LDLErrorType type)
{
	switch (type)
	{
	case LDLEGeneral:
		return TCLocalStrings::get("LDLEGeneral");
	case LDLEParse:
		return TCLocalStrings::get("LDLEParse");
	case LDLEFileNotFound:
		return TCLocalStrings::get("LDLEFileNotFound");
	case LDLEMatrix:
		return TCLocalStrings::get("LDLEMatrix");
	case LDLEPartDeterminant:
		return TCLocalStrings::get("LDLEPartDeterminant");
	case LDLENonFlatQuad:
		return TCLocalStrings::get("LDLENonFlatQuad");
	case LDLEConcaveQuad:
		return TCLocalStrings::get("LDLEConcaveQuad");
//	case LDLEConcaveQuadSplit:
//		return "Error splitting concave quad";
	case LDLEMatchingPoints:
		return TCLocalStrings::get("LDLEMatchingPoints");
//	case LDLEOpenGL:
//		return "OpenGL error";
	case LDLEColinear:
		return TCLocalStrings::get("LDLEColinear");
	case LDLEBFCWarning:
		return TCLocalStrings::get("LDLEBFCWarning");
	case LDLEBFCError:
		return TCLocalStrings::get("LDLEBFCError");
	case LDLEMPDError:
		return TCLocalStrings::get("LDLEMPDError");
	case LDLEVertexOrder:
		return TCLocalStrings::get("LDLEVertexOrder");
	case LDLEWhitespace:
		return TCLocalStrings::get("LDLEWhitespace");
	default:
		return TCLocalStrings::get("LDLEUnknown");
	}
}
