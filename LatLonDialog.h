#ifndef __LATLONDIALOG_H__
#define __LATLONDIALOG_H__

class ModelWindow;
class LDrawModelViewer;

#include <CUI/CUIDialog.h>
#include <TCFoundation/TCStlIncludes.h>
#include <LDLoader/LDLFileLine.h>

typedef std::vector<HWND> HWndVector;
typedef std::map<UINT, LDLLineType> UIntLineTypeMap;
typedef std::map<LDLLineType, UINT> LineTypeUIntMap;

class LatLonDialog: public CUIDialog
{
public:
	LatLonDialog(HINSTANCE hInstance);
	INT_PTR doModal(ModelWindow *modelWindow);
	float getLat(void) const { return m_lat; }
	float getLon(void) const { return m_lon; }
protected:
	virtual ~LatLonDialog(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doTextFieldChange(int controlId, HWND control);
	virtual void doOK(void);

	void updateEnabled(void);

	float m_lat;
	float m_lon;
};

#endif // __LATLONDIALOG_H__
