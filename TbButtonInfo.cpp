#include "TbButtonInfo.h"
#include <CommCtrl.h>
#include <TCFoundation/mystring.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

TbButtonInfo::TbButtonInfo(void)
	:m_commandId(-1),
	m_stdBmpId(-1),
	m_tbBmpId(-1),
	m_state(TBSTATE_ENABLED),
	m_style(TBSTYLE_BUTTON)
{
}

TbButtonInfo::TbButtonInfo(const TbButtonInfo &other)
{
	*this = other;
}

TbButtonInfo &TbButtonInfo::operator=(const TbButtonInfo &other)
{
	m_tooltipText = other.m_tooltipText;
	m_commandId = other.m_commandId;
	m_stdBmpId = other.m_stdBmpId ;
	m_tbBmpId = other.m_tbBmpId;
	m_state = other.m_state;
	m_style = other.m_style;
	return *this;
}

int TbButtonInfo::getBmpId(int stdBitmapStartId, int tbBitmapStartId)
{
	if (m_stdBmpId == -1)
	{
		return tbBitmapStartId + m_tbBmpId;
	}
	else
	{
		return stdBitmapStartId + m_stdBmpId;
	}
}
