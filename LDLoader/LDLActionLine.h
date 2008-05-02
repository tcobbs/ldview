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
	// BI
	bool getBiOverrideColor(void) const
	{
		return m_actionFlags.biOverrideColor != false;
	}
	void setBiOverrideColor(bool value)
	{
		m_actionFlags.biOverrideColor = value;
	}
	int getBiOverrideColorNumber(void) const { return m_biOverrideColorNumber; }
	void setBiOverrideColorNumber(int value)
	{
		setBiOverrideColor(true);
		m_biOverrideColorNumber = value;
	}
	bool getBiOverrideActive(void) const
	{
		return m_actionFlags.biOverrideActive != false;
	}
	void setBiOverrideActive(bool value)
	{
		m_actionFlags.biOverrideActive = value;
	}
	// /BI
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
		bool biOverrideColor;		// in BI mode, whether color is currently
									// getting overridden
		// Temporal flags
		bool biOverrideActive;		// Whether or not to return the override
									// color or the original color
	} m_actionFlags;
	int m_colorNumber;

	int m_biOverrideColorNumber;	// in BI mode, the color used to override
									// either color "16" or "24"
};

#endif // __LDLACTIONLINE_H__
