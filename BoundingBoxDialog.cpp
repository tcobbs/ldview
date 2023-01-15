#include "BoundingBoxDialog.h"
#include "ModelWindow.h"
#include "Resource.h"
#include <TCFoundation/TCAlertManager.h>
#include <LDLoader/LDLMainModel.h>
#include <CUI/CUIWindowResizer.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

BoundingBoxDialog::BoundingBoxDialog(HINSTANCE hInstance):
CUIDialog(hInstance, NULL),
m_modelWindow(NULL),
m_model(NULL)
{
	TCAlertManager::registerHandler(ModelWindow::alertClass(), this,
		(TCAlertCallback)&BoundingBoxDialog::modelAlertCallback);
}

BoundingBoxDialog::~BoundingBoxDialog(void)
{
}

void BoundingBoxDialog::dealloc(void)
{
	TCAlertManager::unregisterHandler(this);
	TCObject::release(m_modelWindow);
	TCObject::release(m_model);
	if (hWindow)
	{
		DestroyWindow(hWindow);
	}
	CUIDialog::dealloc();
}

void BoundingBoxDialog::setModel(LDLMainModel *model)
{
	if (model != m_model)
	{
		TCObject::release(m_model);
		m_model = model;
		TCObject::retain(m_model);
	}
}

void BoundingBoxDialog::modelAlertCallback(TCAlert *alert)
{
	if (alert->getSender() == m_modelWindow)
	{
		if (ucstrcmp(alert->getMessageUC(), _UC("ModelLoaded")) == 0)
		{
			setModel(getModelViewer()->getMainModel());
			updateData();
		}
		else if (ucstrcmp(alert->getMessageUC(), _UC("ModelLoadCanceled")) == 0)
		{
			setModel(NULL);
			updateData();
		}
	}
}

void BoundingBoxDialog::setModelWindow(ModelWindow *modelWindow)
{
	if (modelWindow != m_modelWindow)
	{
		if (m_modelWindow)
		{
			m_modelWindow->release();
		}
		m_modelWindow = modelWindow;
		TCObject::retain(m_modelWindow);
	}
	setModel(getModelViewer()->getMainModel());
}

void BoundingBoxDialog::showBoundingBox(bool value)
{
	LDrawModelViewer *modelViewer = getModelViewer();

	if (modelViewer)
	{
		modelViewer->setShowBoundingBox(value);
	}
	m_modelWindow->boundingBoxToggled();
}

bool BoundingBoxDialog::isVisible(void)
{
	return IsWindowVisible(hWindow) ? true : false;
}

void BoundingBoxDialog::toggle(ModelWindow *modelWindow)
{
	setModelWindow(modelWindow);
	if (hWindow == NULL)
	{
		HWND hParentWnd = GetParent(modelWindow->getHWindow());

		createDialog(IDD_BOUNDING_BOX, hParentWnd);
	}
	else if (IsWindowVisible(hWindow))
	{
		ShowWindow(hWindow, SW_HIDE);
		showBoundingBox(false);
	}
	else
	{
		ShowWindow(hWindow, SW_SHOW);
		showBoundingBox(true);
	}
}

LDrawModelViewer *BoundingBoxDialog::getModelViewer(void)
{
	if (m_modelWindow)
	{
		return m_modelWindow->getModelViewer();
	}
	else
	{
		return NULL;
	}
}

void BoundingBoxDialog::updateData(void)
{
	LDrawModelViewer *modelViewer = getModelViewer();
	if (m_model != NULL && modelViewer != NULL)
	{
		UCCHAR buf1[1024];
		UCCHAR buf2[1024];

		modelViewer->getBoundingMin().print(buf1, COUNT_OF(buf1));
		sucprintf(buf2, COUNT_OF(buf2), _UC("<%s>"), buf1);
		windowSetText(IDC_MIN_POINT, buf2);
		modelViewer->getBoundingMax().print(buf1, COUNT_OF(buf1));
		sucprintf(buf2, COUNT_OF(buf2), _UC("<%s>"), buf1);
		windowSetText(IDC_MAX_POINT, buf2);
	}
	else
	{
		windowSetText(IDC_MIN_POINT, _UC(""));
		windowSetText(IDC_MAX_POINT, _UC(""));
	}
}

BOOL BoundingBoxDialog::doInitDialog(HWND /*hKbControl*/)
{
	updateData();
	setAutosaveName("BoundingBoxDialog");
	return TRUE;
}

LRESULT BoundingBoxDialog::doClose(void)
{
	showWindow(SW_HIDE);
	showBoundingBox(false);
	return 0;
}
