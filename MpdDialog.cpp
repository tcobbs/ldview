#include "MpdDialog.h"
#include "ModelWindow.h"
#include "Resource.h"
#include <TCFoundation/TCAlertManager.h>
#include <LDLoader/LDLMainModel.h>
#include <CUI/CUIWindowResizer.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

MpdDialog::MpdDialog(HINSTANCE hInstance):
CUIDialog(hInstance, NULL),
m_modelWindow(NULL),
m_model(NULL),
m_resizer(NULL),
m_okPressed(false)
{
	TCAlertManager::registerHandler(ModelWindow::alertClass(), this,
		(TCAlertCallback)&MpdDialog::modelAlertCallback);
}

MpdDialog::~MpdDialog(void)
{
}

void MpdDialog::dealloc(void)
{
	TCAlertManager::unregisterHandler(this);
	TCObject::release(m_modelWindow);
	TCObject::release(m_model);
	TCObject::release(m_resizer);
	if (hWindow)
	{
		DestroyWindow(hWindow);
	}
	CUIDialog::dealloc();
}

void MpdDialog::setModel(LDLMainModel *model)
{
	if (model != m_model)
	{
		TCObject::release(m_model);
		m_model = model;
		TCObject::retain(m_model);
	}
}

void MpdDialog::modelAlertCallback(TCAlert *alert)
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

void MpdDialog::setModelWindow(ModelWindow *modelWindow)
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

void MpdDialog::showMpdModel(int index)
{
	LDrawModelViewer *modelViewer = getModelViewer();

	if (modelViewer)
	{
		modelViewer->setMpdChildIndex(index);
		listBoxSelectItem(IDC_MPD_LIST, index);
	}
}

void MpdDialog::show(ModelWindow *modelWindow)
{
	setModelWindow(modelWindow);
	if (hWindow == NULL)
	{
		HWND hParentWnd = GetParent(modelWindow->getHWindow());

		createDialog(IDD_MPD, hParentWnd);
	}
	ShowWindow(hWindow, SW_SHOW);
}

LDrawModelViewer *MpdDialog::getModelViewer(void)
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

void MpdDialog::updateData(void)
{
	LDrawModelViewer *modelViewer = getModelViewer();
	listBoxResetContent(IDC_MPD_LIST);
	if (m_model != NULL && modelViewer != NULL)
	{
		LDLModelVector &mpdModels = m_model->getMpdModels();

		if (mpdModels.size() > 0)
		{
			for (size_t i = 0; i < mpdModels.size(); i++)
			{
				LDLModel *mpdModel = mpdModels[i];
				ucstring ucName;
				utf8toucstring(ucName, mpdModel->getName());

				listBoxAddString(IDC_MPD_LIST, ucName);
			}
			listBoxSelectItem(IDC_MPD_LIST, modelViewer->getMpdChildIndex());
		}
	}
}

BOOL MpdDialog::doInitDialog(HWND hKbControl)
{
	RECT rect;

	GetClientRect(hWindow, &rect);
	setMinSize(rect.right, rect.bottom, true);
	setIcon(IDI_APP_ICON);
	m_resizer = new CUIWindowResizer;
	m_resizer->setHWindow(hWindow);
	m_resizer->addSubWindow(IDC_MPD_LIST, CUISizeHorizontal | CUISizeVertical);
	m_resizer->addSubWindow(IDOK, CUIFloatLeft | CUIFloatTop);
	m_resizer->addSubWindow(IDCANCEL, CUIFloatLeft | CUIFloatTop);
	m_resizer->addResizeGrip();
	updateData();
	setAutosaveName("MpdDialog");
	return CUIDialog::doInitDialog(hKbControl);
}

LRESULT MpdDialog::doClose(void)
{
	showWindow(SW_HIDE);
	if (!m_okPressed)
	{
		showMpdModel(0);
	}
	m_okPressed = false;
	return 0;
}

LRESULT MpdDialog::doSize(WPARAM sizeType, int newWidth, int newHeight)
{
	if (sizeType != SIZE_MINIMIZED)
	{
		m_resizer->resize(newWidth, newHeight);
	}
	return CUIDialog::doSize(sizeType, newWidth, newHeight);
}

LRESULT MpdDialog::doCommand(
	int notifyCode,
	int commandId,
	HWND control)
{
	if (commandId == IDC_MPD_LIST && notifyCode == LBN_SELCHANGE)
	{
		showMpdModel(listBoxGetSelectedItem(IDC_MPD_LIST));
	}
	return CUIDialog::doCommand(notifyCode, commandId, control);
}

void MpdDialog::doCancel(void)
{
	doClose();
}

void MpdDialog::doOK(void)
{
	m_okPressed = true;
	doClose();
}
