#ifndef __LDLACTIONLINE_H__
#define __LDLACTIONLINE_H__

#include <LDLoader/LDLFileLine.h>
#include <LDLoader/LDLModel.h>

class LDLActionLine : public LDLFileLine
{
public:
	virtual bool isActionLine(void) const { return true; }
	virtual void setBFCSettings(BFCState bfcCertify, bool bfcClip,
		bool bfcWindingCCW, bool bfcInvert);
	bool getBFCClip(void) const { return m_actionFlags.bfcClip != false; }
	bool getBFCWindingCCW(void) const { return m_actionFlags.bfcWindingCCW != false; }
	bool getBFCInvert(void) const { return m_actionFlags.bfcInvert != false; }
	BFCState getBFCState(void) const { return m_actionFlags.bfcCertify; }
	virtual int getColorNumber(void) const;
	virtual void setColorNumber(int value) { m_colorNumber = value; }
	bool getBFCOn(void) const
	{
		return (m_actionFlags.bfcCertify == BFCOnState ||
			m_actionFlags.bfcCertify == BFCForcedOnState) &&
			m_actionFlags.bfcClip;
	}
	virtual void scanPoints(TCObject *scanner,
		LDLScanPointCallback scanPointCallback, const TCFloat *matrix)
		const = 0;
protected:
	LDLActionLine(LDLModel *parentModel, const char *line, int lineNumber,
		const char *originalLine = NULL);
	LDLActionLine(const LDLActionLine &other);
	int getRandomColorNumber(void) const;

	struct
	{
		// Public flags
		BFCState bfcCertify:3;
		bool bfcClip:1;
		bool bfcWindingCCW:1;
		bool bfcInvert:1;
	} m_actionFlags;
	int m_colorNumber;
};

#endif // __LDLACTIONLINE_H__
