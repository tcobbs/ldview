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
protected:
	LDLActionLine(LDLModel *parentModel, const char *line, int lineNumber);
	LDLActionLine(const LDLActionLine &other);

	struct
	{
		// Public flags
		BFCState bfcCertify:2;
		bool bfcClip:1;
		bool bfcWindingCCW:1;
		bool bfcInvert:1;
	} m_actionFlags;
};

#endif // __LDLACTIONLINE_H__
