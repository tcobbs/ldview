#ifndef __LATLONDIALOG_H__
#define __LATLONDIALOG_H__

class ModelWindow;
class LDrawModelViewer;

#include <CUI/CUIDialog.h>

class LatLonDialog: public CUIDialog
{
public:
	LatLonDialog(HINSTANCE hInstance);
	INT_PTR doModal(ModelWindow *modelWindow);
	float getLat(void) const { return m_lat; }
	float getLon(void) const { return m_lon; }
	float getDistance(void) const;
	void setDefaultDistance(float value) { m_defaultDistance = value; }
	void setCurrentDistance(float value) { m_currentDistance = value; }
protected:
	virtual ~LatLonDialog(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doTextFieldChange(int controlId, HWND control);
	virtual LRESULT doCommand(int notifyCode, int commandId, HWND control);
	virtual void doOK(void);

	LRESULT doDistanceCheck(void);
	LRESULT doDistanceReset(float value);

	BOOL verifyField(UINT fieldID, float &value, float min, float max,
		bool checkMin = true, bool checkMax = true);

	void updateEnabled(void);
	void updateDistanceField(void);

	float m_lat;
	float m_lon;
	bool m_haveDistance;
	float m_distance;
	float m_defaultDistance;
	float m_currentDistance;
};

#endif // __LATLONDIALOG_H__
