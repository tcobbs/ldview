#ifndef __LDLACTIONLINE_H__
#define __LDLACTIONLINE_H__

#include <LDLoader/LDLFileLine.h>
#include <LDLoader/LDLModel.h>

class LDLActionLine : public LDLFileLine
{
public:
	virtual bool isActionLine(void) { return true; }
	virtual void setBFCSettings(BFCState bfcCertify, bool bfcClip,
		bool bfcWindingCCW, bool bfcInvert);
	bool getBFCClip(void) { return m_actionFlags.bfcClip != false; }
	bool getBFCWindingCCW(void) { return m_actionFlags.bfcWindingCCW != false; }
	bool getBFCInvert(void) { return m_actionFlags.bfcInvert != false; }
	BFCState getBFCState(void) { return m_actionFlags.bfcCertify; }
	bool getBFCOn(void)
	{
		return (m_actionFlags.bfcCertify == BFCOnState ||
			m_actionFlags.bfcCertify == BFCForcedOnState) &&
			m_actionFlags.bfcClip;
	}
protected:
	LDLActionLine(LDLModel *parentModel, const char *line, int lineNumber,
		const char *originalLine = NULL);
	LDLActionLine(const LDLActionLine &other);

	struct
	{
		// Public flags
		BFCState bfcCertify:3;
		bool bfcClip:1;
		bool bfcWindingCCW:1;
		bool bfcInvert:1;
	} m_actionFlags;
};

#endif // __LDLACTIONLINE_H__
