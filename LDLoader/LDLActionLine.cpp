#include "LDLActionLine.h"

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

LDLActionLine::LDLActionLine(LDLModel *parentModel, const char *line,
							 int lineNumber, const char *originalLine)
	:LDLFileLine(parentModel, line, lineNumber, originalLine)
{
	m_actionFlags.bfcCertify = BFCUnknownState;
	m_actionFlags.bfcClip = false;
	m_actionFlags.bfcWindingCCW = true;
	m_actionFlags.bfcInvert = false;
}

LDLActionLine::LDLActionLine(const LDLActionLine &other)
	:LDLFileLine(other),
	m_actionFlags(other.m_actionFlags),
	m_colorNumber(other.m_colorNumber)
{
}

void LDLActionLine::setBFCSettings(BFCState bfcCertify, bool bfcClip,
								  bool bfcWindingCCW, bool bfcInvert)
{
	m_actionFlags.bfcCertify = bfcCertify;
	m_actionFlags.bfcClip = bfcClip;
	m_actionFlags.bfcWindingCCW = bfcWindingCCW;
	m_actionFlags.bfcInvert = bfcInvert;
}
