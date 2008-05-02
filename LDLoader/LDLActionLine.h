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
	bool getBFCOn(void) const
	{
		return (m_actionFlags.bfcCertify == BFCOnState ||
			m_actionFlags.bfcCertify == BFCForcedOnState) &&
			m_actionFlags.bfcClip;
	}
	virtual void scanPoints(TCObject *scanner,
		LDLScanPointCallback scanPointCallback, const TCFloat *matrix,
		LDLModel::ScanPointType types) const = 0;
	// OBI
	bool getObiOverrideColor(void) const
	{
		return m_actionFlags.obiOverrideColor != false;
	}
	void setObiOverrideColor(bool value)
	{
		m_actionFlags.obiOverrideColor = value;
	}
	int getObiOverrideColorNumber(void) const
	{
		return m_obiOverrideColorNumber;
	}
	void setObiOverrideColorNumber(int value)
	{
		setObiOverrideColor(true);
		m_obiOverrideColorNumber = value;
	}
	bool getObiOverrideActive(void) const
	{
		return m_actionFlags.obiOverrideActive != false;
	}
	void setObiOverrideActive(bool value)
	{
		m_actionFlags.obiOverrideActive = value;
	}
	// /OBI
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
		bool obiOverrideColor;		// in OBI mode, whether color is currently
									// getting overridden
		// Temporal flags
		bool obiOverrideActive;		// Whether or not to return the override
									// color or the original color
	} m_actionFlags;
	int m_colorNumber;

	int m_obiOverrideColorNumber;	// in OBI mode, the color used to override
									// either color "16" or "24"
};

#endif // __LDLACTIONLINE_H__
