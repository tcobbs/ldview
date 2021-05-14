#ifndef __CAMERALOCATIONDIALOG_H__
#define __CAMERALOCATIONDIALOG_H__

class ModelWindow;
class LDrawModelViewer;

#include <CUI/CUIDialog.h>
#include <TCFoundation/TCStlIncludes.h>
#include <LDLoader/LDLFileLine.h>

typedef std::vector<HWND> HWndVector;
typedef std::map<UINT, LDLLineType> UIntLineTypeMap;
typedef std::map<LDLLineType, UINT> LineTypeUIntMap;

class CameraLocationDialog: public CUIDialog
{
public:
	CameraLocationDialog(HINSTANCE hInstance);
	INT_PTR doModal(ModelWindow *modelWindow);
	float getX(void) const { return m_x; }
	void setX(float value) { m_x = value; }
	float getY(void) const { return m_y; }
	void setY(float value) { m_y = value; }
	float getZ(void) const { return m_z; }
	void setZ(float value) { m_z = value; }
protected:
	virtual ~CameraLocationDialog(void);
	virtual void dealloc(void);

	virtual BOOL doInitDialog(HWND /*hKbControl*/);
	virtual LRESULT doTextFieldChange(int controlId, HWND control);
	virtual LRESULT doCommand(int notifyCode, int commandId, HWND control);
	virtual void doOK(void);

	BOOL verifyField(UINT fieldID, float &value, float min, float max,
		bool checkMin = true, bool checkMax = true);

	void updateEnabled(void);

	float m_x;
	float m_y;
	float m_z;
};

#endif // __CAMERALOCATIONDIALOG_H__
