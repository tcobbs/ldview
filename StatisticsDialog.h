#ifndef __StatisticsDialog_H__
#define __StatisticsDialog_H__

class ModelWindow;
class LDrawModelViewer;

#include <CUI/CUIDialog.h>
#include <LDLoader/LDLFileLine.h>

class ModelWindow;

class StatisticsDialog : public CUIDialog
{
public:
	StatisticsDialog(HINSTANCE hInstance);
	void setStatistics(const LDLStatistics& value) { m_statistics = value; }
	INT_PTR doModal(ModelWindow* modelWindow);
protected:
	virtual ~StatisticsDialog(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);

	LDLStatistics m_statistics;
};

#endif // __StatisticsDialog_H__
