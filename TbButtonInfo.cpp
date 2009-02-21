#include "TbButtonInfo.h"
#include <CommCtrl.h>
#include <TCFoundation/mystring.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

TbButtonInfo::TbButtonInfo(void)
	:m_commandId(-1),
	m_bmpIndex(-1),
	m_state(TBSTATE_ENABLED),
	m_style(TBSTYLE_BUTTON),
	m_selection(0)
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
	m_bmpIndex = other.m_bmpIndex;
	m_state = other.m_state;
	m_style = other.m_style;
	m_selection = other.m_selection;
	m_otherCommandIds = other.m_otherCommandIds;
	return *this;
}
