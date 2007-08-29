#pragma once
#include <TCFoundation/TCObject.h>

class LDrawModelViewer;
class TCAlert;

class LDInputHandler : public TCObject
{
public:
	enum ModifierKey
	{
		MKShift = 0x0001,
		MKControl = 0x0002,
		MKAppleControl = 0x0004,
	};
	enum MouseButton
	{
		MBLeft,
		MBFirst = MBLeft,
		MBRight,
		MBMiddle,
		MBLast = MBMiddle
	};
	enum ViewMode
	{
		VMExamine,
		VMFlyThrough,
	};

	LDInputHandler(LDrawModelViewer *modelViewer);

	void setViewMode(ViewMode value) { m_viewMode = value; }
	bool mouseDown(TCULong modifierKeys, MouseButton button, int x, int y);
	bool mouseUp(TCULong modifierKeys, MouseButton button, int x, int y);
	bool mouseMove(TCULong modifierKeys, int x, int y);
	bool mouseWheel(TCULong modifierKeys, TCFloat amount);
	void setMouseUpPending(bool value);

	static TCULong convertModifiers(TCULong osModifiers);
	static const char *captureAlertClass(void) { return "LDCaptureMouse"; }
	static const char *releaseAlertClass(void) { return "LDReleaseMouse"; }
	static const char *peekMouseUpAlertClass(void) { return "LDPeekMouseUp"; }
protected:
	typedef enum
	{
		MMNone,
		MMNormal,
		MMZoom,
		MMPan,
		MMLight,
	} MouseMode;

	~LDInputHandler(void);
	void dealloc(void);
	bool lightMouseDown(int x, int y);
	bool updateSpinRateXY(int xPos, int yPos);
	bool updateHeadXY(TCULong modifierKeys, int xPos, int yPos);
	bool updatePanXY(int xPos, int yPos);
	bool updateZoom(int yPos);
	void frameDone(TCAlert *alert);
	//double getTimeRef(void);

	LDrawModelViewer *m_modelViewer;
	ViewMode m_viewMode;
	MouseMode m_mouseMode;
	bool m_buttonsDown[3];
	bool m_appleRightClick;
	int m_numButtons;
	int m_clickX;
	int m_clickY;
	int m_lastX;
	int m_lastY;
	int m_lastFrameX;
	int m_lastFrameY;
	TCFloat m_rotationSpeed;
	bool m_mouseUpPending;
	bool m_mouseUpHandled;
	//double m_lastMoveTime;
};
