#ifndef __LOCATIONDIALOG_H__
#define __LOCATIONDIALOG_H__

class ModelWindow;
class LDrawModelViewer;

#include <CUI/CUIDialog.h>

class LocationDialog: public CUIDialog
{
public:
	LocationDialog(HINSTANCE hInstance, int dialogId);
	INT_PTR doModal(ModelWindow *modelWindow);
	float getX(void) const { return m_x; }
	void setX(float value) { m_x = value; }
	float getY(void) const { return m_y; }
	void setY(float value) { m_y = value; }
	float getZ(void) const { return m_z; }
	void setZ(float value) { m_z = value; }
protected:
	virtual ~LocationDialog(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doTextFieldChange(int controlId, HWND control);

	BOOL verifyField(UINT fieldID, float &value, float min, float max,
		bool checkMin = true, bool checkMax = true);

	virtual void updateEnabled(void);

	int m_dialogId;
	float m_x;
	float m_y;
	float m_z;
};

#endif // __LOCATIONDIALOG_H__
