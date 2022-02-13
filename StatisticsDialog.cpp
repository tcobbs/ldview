#include "StatisticsDialog.h"
#include "ModelWindow.h"
#include "Resource.h"
#include <TCFoundation/TCUserDefaults.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

StatisticsDialog::StatisticsDialog(HINSTANCE hInstance):
CUIDialog(hInstance)
, m_statistics()
{
}

StatisticsDialog::~StatisticsDialog(void)
{
}

BOOL StatisticsDialog::doInitDialog(HWND hKbControl)
{
	setAutosaveName("StatisticsDialog");
	windowSetValue(IDC_PARTS, (double)m_statistics.parts);
	windowSetValue(IDC_MODELS, (double)m_statistics.models);
	windowSetValue(IDC_TRIANGLES, (double)m_statistics.triangles);
	windowSetValue(IDC_QUADS, (double)m_statistics.quads);
	windowSetValue(IDC_LINES, (double)m_statistics.lines);
	windowSetValue(IDC_EDGE_LINES, (double)m_statistics.edgeLines);
	windowSetValue(IDC_CONDITIONAL_LINES, (double)m_statistics.conditionalLines);
	return TRUE;
}

INT_PTR StatisticsDialog::doModal(ModelWindow* modelWindow)
{
	HWND hParentWnd = GetParent(modelWindow->getHWindow());

	return CUIDialog::doModal(IDD_STATISTICS, hParentWnd);
}
