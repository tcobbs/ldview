#include "LDLActionLine.h"
#include "LDLPalette.h"
#include "LDLMainModel.h"

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

LDLActionLine::LDLActionLine(LDLModel *parentModel, const char *line,
							 int lineNumber, const char *originalLine)
	:LDLFileLine(parentModel, line, lineNumber, originalLine)
{
	memset(&m_actionFlags, 0, sizeof(m_actionFlags));
	m_actionFlags.bfcCertify = BFCUnknownState;
	m_actionFlags.bfcWindingCCW = true;
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

int LDLActionLine::getRandomColorNumber(void) const
{
	int r = rand() % 256;
	int g = rand() % 256;
	int b = rand() % 256;
	return LDLPalette::colorNumberForRGBA(r, g, b, 255);
}

int LDLActionLine::getColorNumber(void) const
{
	const LDLMainModel *mainModel = getMainModel();

	if (m_colorNumber != 24 && mainModel->getRandomColors() &&
		(mainModel == m_parentModel || m_colorNumber != 16))
	{
		return getRandomColorNumber();
	}
	else if (mainModel->getForceHighlightColor())
	{
		return mainModel->getHighlightColorNumber();
	}
	else
	{
		return m_colorNumber;
	}
}
