#ifndef __TBBUTTONINFO_H__
#define __TBBUTTONINFO_H__

#include <TCFoundation/TCStlIncludes.h>
#include <TCFoundation/mystring.h>

class TbButtonInfo
{
public:
	TbButtonInfo(void);
	TbButtonInfo(const TbButtonInfo &other);
	~TbButtonInfo(void) {}
	TbButtonInfo &operator=(const TbButtonInfo &other);

	CUCSTR getTooltipText(void) const { return m_tooltipText.c_str(); }
	void setTooltipText(const ucstring value) { m_tooltipText = value; }
	int getCommandId(void) const { return m_commandId; }
	void setCommandId(int value) { m_commandId = value; }
	int getBmpIndex(void) const { return m_bmpIndex; }
	void setBmpIndex(int value) { m_bmpIndex = value; }
	BYTE getState(void) const { return m_state; }
	void setState(BYTE value) { m_state = value; }
	BYTE getStyle(void) const { return m_style; }
	void setStyle(BYTE value) { m_style = value; }
protected:

	ucstring m_tooltipText;
	int m_commandId;
	int m_bmpIndex;
	BYTE m_state;
	BYTE m_style;
};

typedef std::vector<TbButtonInfo> TbButtonInfoVector;

#endif // __TBBUTTONINFO_H__
