#include "LDInputHandler.h"
#include "LDrawModelViewer.h"
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCAlert.h>
#include <TCFoundation/TCMacros.h>
//#ifdef __APPLE__
//enum {
//	NSAlphaShiftKeyMask =		1 << 16,
//	NSShiftKeyMask =		1 << 17,
//	NSControlKeyMask =		1 << 18,
//	NSAlternateKeyMask =		1 << 19,
//	NSCommandKeyMask =		1 << 20,
//	NSNumericPadKeyMask =		1 << 21,
//	NSHelpKeyMask =			1 << 22,
//	NSFunctionKeyMask =		1 << 23,
//#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
//	NSDeviceIndependentModifierFlagsMask = 0xffff0000U
//#endif
//};
//#endif // __APPLE__
//#ifdef _QT
//#include <qnamespace.h>
//#endif // _QT

LDInputHandler::LDInputHandler(LDrawModelViewer *modelViewer):
	m_modelViewer(modelViewer),
	m_viewMode(VMExamine),
	m_mouseMode(MMNone),
	m_appleRightClick(false),
	m_numButtons(sizeof(m_buttonsDown) / sizeof(m_buttonsDown[0]))
{
	memset(&m_buttonsDown, 0, sizeof(m_buttonsDown));
	TCAlertManager::registerHandler(LDrawModelViewer::frameDoneAlertClass(),
		this, (TCAlertCallback)&LDInputHandler::frameDone);
}

LDInputHandler::~LDInputHandler(void)
{
}

void LDInputHandler::dealloc(void)
{
	TCAlertManager::unregisterHandler(this);
	TCObject::dealloc();
}

//// Note: static method.
//TCULong LDInputHandler::convertModifiers(TCULong osModifiers)
//{
//	TCULong retValue = 0;
//#ifdef WIN32
//	if (osModifiers & MK_SHIFT)
//	{
//		retValue |= MKShift;
//	}
//	if (osModifiers & MK_CONTROL)
//	{
//		retValue |= MKControl;
//	}
//#else // WIN32
//#ifdef _QT
//	if (osModifiers & Qt::ShiftButton)
//	{
//		retValue |= MKShift;
//	}
//	if (osModifiers & Qt::ControlButton)
//	{
//		retValue |= MKControl;
//	}
//#else // _QT
//#ifdef __APPLE__
//	if (osModifiers & NSShiftKeyMask)
//	{
//		retValue |= MKShift;
//	}
//	if (osModifiers & NSCommandKeyMask)
//	{
//		retValue |= MKControl;
//	}
//	if (osModifiers & NSControlKeyMask)
//	{
//		retValue |= MKAppleControl;
//	}
//#else // __APPLE__
//	// If you get here, this isn't going to work.  The below line will generate
//	// an error.
//	Unknown GUI framework.
//#endif // __APPLE__
//#endif // _QT
//#endif // WIN32
//	return retValue;
//}

bool LDInputHandler::updateSpinRateXY(int xPos, int yPos)
{
	int deltaX = xPos - m_lastX;
	int deltaY = yPos - m_lastY;
	TCFloat magnitude = (TCFloat)sqrt((TCFloat)(deltaX * deltaX + deltaY * deltaY));

	m_lastX = xPos;
	m_lastY = yPos;
	m_rotationSpeed = magnitude / 10.0f;
	if (fEq(m_rotationSpeed, 0.0f))
	{
		m_rotationSpeed = 0.0f;
		m_modelViewer->setXRotate(1.0f);
		m_modelViewer->setYRotate(1.0f);
	}
	else
	{
		m_modelViewer->setXRotate((TCFloat)deltaY);
		m_modelViewer->setYRotate((TCFloat)deltaX);
	}
	m_modelViewer->setRotationSpeed(m_rotationSpeed);
	m_modelViewer->requestRedraw();
	return true;
}

bool LDInputHandler::updateHeadXY(TCULong modifierKeys, int xPos, int yPos)
{
	TCFloat magnitude = (TCFloat)(xPos - m_lastX);
	TCFloat denom = 5000.0f;
	TCFloat fov = m_modelViewer->getFov();

	denom /= (TCFloat)tan(deg2rad(fov));
	//if (GetKeyState(VK_SHIFT) & 0x8000)
	if (modifierKeys & MKShift)
	{
		debugPrintf("Faster.\n");
		denom /= 2.0f;
	}
	m_modelViewer->setCameraXRotate(magnitude / denom);
	magnitude = (TCFloat)(yPos - m_lastY);
	m_modelViewer->setCameraYRotate(magnitude / -denom);
	m_modelViewer->requestRedraw();
	return true;
}

bool LDInputHandler::updatePanXY(int xPos, int yPos)
{
	int deltaX = xPos - m_lastX;
	int deltaY = yPos - m_lastY;

	m_lastX = xPos;
	m_lastY = yPos;
	m_modelViewer->panXY(deltaX, deltaY);
	m_modelViewer->requestRedraw();
	return true;
}

bool LDInputHandler::updateZoom(int yPos)
{
	TCFloat magnitude = (TCFloat)(yPos - m_clickY);

	m_modelViewer->setZoomSpeed(magnitude / 2.0f);
	m_modelViewer->requestRedraw();
	return true;
}

bool LDInputHandler::mouseDown(
	TCULong modifierKeys,
	MouseButton button,
	int x,
	int y)
{
	if (button < MBFirst || button > MBLast)
	{
		return false;
	}
	if (modifierKeys & MKAppleControl && button == MBLeft)
	{
		m_appleRightClick = true;
		button = MBRight;
	}
	// Don't process chorded mouse clicks.
	for (int i = 0; i < m_numButtons; i++)
	{
		if (i != button && m_buttonsDown[i])
		{
			return false;
		}
	}
	m_buttonsDown[button] = true;
	m_lastX = m_lastFrameX = m_clickX = x;
	m_lastY = m_lastFrameY = m_clickY = y;
	switch (button)
	{
	case MBLeft:
		if (modifierKeys & MKShift)
		{
			m_mouseMode = MMLight;
			m_modelViewer->mouseDown(LDVMouseLight, x, y);
		}
		else if (modifierKeys & MKControl)
		{
			m_mouseMode = MMPan;
		}
		else
		{
			m_mouseMode = MMNormal;
			updateSpinRateXY(x, y);
		}
		break;
	case MBRight:
		m_modelViewer->setClipZoom((modifierKeys & MKControl) != 0);
		m_mouseMode = MMZoom;
		break;
	case MBMiddle:
		m_mouseMode = MMPan;
		break;
	}
	TCAlertManager::sendAlert(captureAlertClass(), this);
	return true;
}

bool LDInputHandler::mouseUp(
	TCULong modifierKeys,
	MouseButton button,
	int x,
	int y)
{
	if (button < MBFirst || button > MBLast)
	{
		return false;
	}
	if (button == MBLeft && m_appleRightClick)
	{
		m_appleRightClick = false;
		button = MBRight;
	}
	if (!m_buttonsDown[button])
	{
		return false;
	}
	if (x != m_lastX || y != m_lastY)
	{
		mouseMove(modifierKeys, x, y);
	}
	switch (button)
	{
	case MBLeft:
		if (m_viewMode == VMFlyThrough)
		{
			m_modelViewer->setCameraXRotate(0.0f);
			m_modelViewer->setCameraYRotate(0.0f);
		}
		if (m_mouseMode == MMLight)
		{
			m_modelViewer->mouseUp(x, y);
		}
		break;
	case MBRight:
		m_modelViewer->setZoomSpeed(0.0f);
	default:
		break;
	}
	TCAlertManager::sendAlert(releaseAlertClass(), this);
	m_modelViewer->requestRedraw();
	m_buttonsDown[button] = false;
	m_mouseMode = MMNone;
	return true;
}

//double LDInputHandler::getTimeRef(void)
//{
//#ifdef WIN32
//	return (double)GetTickCount() / 1000.0;
//#else // WIN32
//#ifdef _QT
//	return -1.0;
//#else // _QT
//#ifdef __APPLE__
//	return -1.0;
//#else // __APPLE__
//	// If you get here, this isn't going to work.  The below line will generate
//	// an error.
//	Unknown GUI framework.
//#endif // __APPLE__
//#endif // _QT
//#endif // WIN32
//}

bool LDInputHandler::mouseMove(TCULong modifierKeys, int x, int y)
{
	switch (m_mouseMode)
	{
	case MMNone:
		return false;
	case MMNormal:
		if (m_viewMode == VMExamine)
		{
			//m_lastMoveTime = getTimeRef();
			return updateSpinRateXY(x, y);
		}
		else
		{
			return updateHeadXY(modifierKeys, x, y);
		}
	case MMZoom:
		return updateZoom(y);
	case MMPan:
		if (m_viewMode == VMExamine)
		{
			return updatePanXY(x, y);
		}
	case MMLight:
		if (m_mouseMode == MMLight)
		{
			m_modelViewer->mouseMove(x, y);
		}
		break;
	}
	return false;
}

bool LDInputHandler::mouseWheel(TCULong modifierKeys, TCFloat amount)
{
	// Don't process while while any mouse buttons are pressed.
	for (int i = 0; i < m_numButtons; i++)
	{
		if (m_buttonsDown[i])
		{
			return false;
		}
	}
	m_modelViewer->setClipZoom((modifierKeys & MKControl) != 0);
	m_modelViewer->zoom((TCFloat)amount * -0.5f);
	m_modelViewer->requestRedraw();
	return true;
}

void LDInputHandler::setMouseUpPending(bool value)
{
	m_mouseUpPending = value;
	m_mouseUpHandled = true;
}

void LDInputHandler::frameDone(TCAlert *alert)
{
	if (m_mouseMode == MMNormal && m_viewMode == VMExamine &&
		alert->getSender() == m_modelViewer)
	{
		// We're spinning, and a frame just completed drawing, so
		// check to see if we should stop the spinning.
		m_mouseUpHandled = false;
		TCAlertManager::sendAlert(peekMouseUpAlertClass(), this);
		if (!m_mouseUpHandled ||
			(m_mouseUpHandled && m_mouseUpPending))
		{
			updateSpinRateXY(m_lastX, m_lastY);
		}
		m_lastFrameX = m_lastX;
		m_lastFrameY = m_lastY;
	}
}
