#include "LDLActionLine.h"
#include "LDLPalette.h"

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
	m_actionFlags.biOverrideColor = false;
	m_actionFlags.biOverrideActive = false;
	m_biOverrideColorNumber = 0;	// default to black
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

int LDLActionLine::getColorNumber(void) const
{
	if (getBiOverrideColor() && getBiOverrideActive())
	{
		if (m_colorNumber == 24)
		{
			return 0;
		}
		else if (m_colorNumber == 16)
		{
			return m_biOverrideColorNumber;
		}
	}
	return m_colorNumber;
}
