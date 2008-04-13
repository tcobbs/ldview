#include "ToolbarStrip.h"
#include "Resource.h"
#include "LDViewWindow.h"
#include "ModelWindow.h"
#include "LDViewPreferences.h"
#include <TCFoundation/TCJpegOptions.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCAlertManager.h>

ToolbarStrip::ToolbarStrip(HINSTANCE hInstance):
CUIDialog(hInstance),
m_stdBitmapStartId(-1),
m_tbBitmapStartId(-1),
m_numSteps(0)
{
	ModelWindow::initCommonControls(ICC_BAR_CLASSES | ICC_WIN95_CLASSES);
	TCAlertManager::registerHandler(ModelWindow::alertClass(), this,
		(TCAlertCallback)&ToolbarStrip::modelAlertCallback);
}

ToolbarStrip::~ToolbarStrip(void)
{
}

void ToolbarStrip::dealloc(void)
{
	TCAlertManager::unregisterHandler(this);
	m_prefs->release();
	CUIDialog::dealloc();
}

void ToolbarStrip::modelAlertCallback(TCAlert *alert)
{
	if (alert->getSender() == m_ldviewWindow->getModelWindow())
	{
		if (ucstrcmp(alert->getMessageUC(), _UC("ModelLoaded")) == 0 ||
			ucstrcmp(alert->getMessageUC(), _UC("ModelLoadCanceled")) == 0)
		{
			updateStep();
			updateNumSteps();
		}
	}
}

void ToolbarStrip::create(LDViewWindow *parent)
{
	m_ldviewWindow = parent;
	m_prefs = m_ldviewWindow->getModelWindow()->getPrefs();
	m_prefs->retain();
	::CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_TOOLBAR),
		parent->getHWindow(), staticDialogProc, (LPARAM)this);
}

void ToolbarStrip::show(void)
{
	::ShowWindow(hWindow, SW_SHOW);
}

void ToolbarStrip::hide(void)
{
	::ShowWindow(hWindow, SW_HIDE);
}

void ToolbarStrip::initToolbar(
	HWND hToolbar,
	TbButtonInfoVector &infos,
	UINT bitmapId)
{
	TBADDBITMAP addBitmap;
	TBBUTTON *buttons;
	char buttonTitle[128];
	int i;
	int count;
	RECT rect;
	RECT buttonRect;
	int width;
	int height;

	SendMessage(hToolbar, TB_SETEXTENDEDSTYLE, 0,
		TBSTYLE_EX_DRAWDDARROWS | WS_EX_TRANSPARENT);
	memset(buttonTitle, 0, sizeof(buttonTitle));
	SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	// The toolbar behaves very strangely.  The following setting gives me
	// buttons that are the same size as the buttons in WordPad.  Any other
	// settings don't.  Additionally, increasing the second number to 23
	// increases the size of toolbar buttons dramatically.  So, I have to
	// tell it to use 23 as the minimum width and 22 as the maximum width in
	// order to get it to display like I want it to.  Go figure.
	SendMessage(hToolbar, TB_SETBUTTONWIDTH, 0, MAKELONG(23, 22));
	if (true/*newToolbar()*/)
	{
		HIMAGELIST imageList = ImageList_Create(16, 16, ILC_COLOR24 | ILC_MASK,
			10, 10);
		// Should the toolbar bitmap be language-specific?
		HBITMAP hBitmap = (HBITMAP)LoadImage(getLanguageModule(),
			MAKEINTRESOURCE(bitmapId), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		HBITMAP hMask = createMask(hBitmap, RGB(255, 0, 254));

		// ImageList_AddMask works fine in XP, and avoids the necessity of
		// creating the mask via the createMask function above, but according
		// to the documentation, it isn't supposed to work on bitmaps whose
		// color depth is greater than 8bpp.  Ours is 24bpp.
		ImageList_Add(imageList, hBitmap, hMask);
		DeleteObject(hBitmap);
		DeleteObject(hMask);
		SendMessage(hToolbar, TB_SETIMAGELIST, 0, (LPARAM)imageList);
		m_stdBitmapStartId = m_tbBitmapStartId = 0;
	}
	else
	{
		addBitmap.hInst = HINST_COMMCTRL;
		addBitmap.nID = IDB_STD_SMALL_COLOR;
		m_stdBitmapStartId = SendMessage(hToolbar, TB_ADDBITMAP, 0,
			(LPARAM)&addBitmap);
		// Should the toolbar bitmap be language-specific?
		addBitmap.hInst = getLanguageModule();
		// This doesn't actually work!!!
		addBitmap.nID = bitmapId;//256;
		// The 10 on the following line is the number of buttons in the bitmap.
		m_tbBitmapStartId = SendMessage(hToolbar, TB_ADDBITMAP, 10,
			(LPARAM)&addBitmap);
	}
	// Note: buttonTitle is an empty string.  No need for Unicode.
	SendMessage(hToolbar, TB_ADDSTRING, 0, (LPARAM)buttonTitle);
	count = (int)infos.size();
	buttons = new TBBUTTON[count];
	for (i = 0; i < count; i++)
	{
		TbButtonInfo &buttonInfo = infos[i];

		buttons[i].iBitmap = buttonInfo.getBmpId(m_stdBitmapStartId,
			m_tbBitmapStartId);
		buttons[i].idCommand = buttonInfo.getCommandId();
		buttons[i].fsState = buttonInfo.getState();
		buttons[i].fsStyle = buttonInfo.getStyle();
		buttons[i].dwData = (DWORD)this;
		buttons[i].iString = -1;
	}
	SendMessage(hToolbar, TB_ADDBUTTONS, count, (LPARAM)buttons);
	SendMessage(hToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(17, 16));
	GetWindowRect(hToolbar, &rect);
	screenToClient(hWindow, &rect);
	if (!SendMessage(hToolbar, TB_GETRECT, buttons[count - 1].idCommand,
		(LPARAM)&buttonRect))
	{
		buttonRect = rect;
		buttonRect.left -= rect.left;
		buttonRect.right -= rect.right;
	}
	SendMessage(hToolbar, TB_SETPARENT, (WPARAM)m_ldviewWindow->getHWindow(),
		0);
	delete[] buttons;
	width = buttonRect.right;
	height = buttonRect.bottom - buttonRect.top;
	MoveWindow(hToolbar, rect.left, rect.top, width, height, FALSE);
	SendMessage(hToolbar, TB_SETHOTITEM, (WPARAM)-1, 0);
	ShowWindow(hToolbar, SW_SHOW);
}

void ToolbarStrip::initMainToolbar(void)
{
	populateMainTbButtonInfos();
	initToolbar(m_hToolbar, m_mainButtonInfos, IDB_TOOLBAR);
}

void ToolbarStrip::initStepToolbar(void)
{
	populateStepTbButtonInfos();
	initToolbar(m_hStepToolbar, m_stepButtonInfos, IDB_STEP_TOOLBAR);
}

void ToolbarStrip::autoSize(void)
{
	RECT rect;
	RECT parentRect;

	GetWindowRect(hWindow, &rect);
	GetClientRect(m_ldviewWindow->getHWindow(), &parentRect);
	screenToClient(m_ldviewWindow->getHWindow(), &rect);
	MoveWindow(hWindow, rect.left, rect.top, parentRect.right - parentRect.left,
		m_stripHeight, TRUE);
}

void ToolbarStrip::initLayout(void)
{
	RECT tbRect;
	RECT rect;
	int delta;
	//int right;
	int maxHeight;
	size_t i;

	GetWindowRect(m_hToolbar, &tbRect);
	screenToClient(hWindow, &tbRect);
	GetWindowRect(m_hStepLabel, &rect);
	screenToClient(hWindow, &rect);
	delta = tbRect.right + 16 - rect.left;
	maxHeight = tbRect.bottom - tbRect.top;
	for (i = 1; i < m_controls.size(); i++)
	{
		int height;

		GetWindowRect(m_controls[i], &rect);
		screenToClient(hWindow, &rect);
		height = rect.bottom - rect.top;
		MoveWindow(m_controls[i], rect.left + delta, rect.top,
			rect.right - rect.left, height, FALSE);
		if (height > maxHeight)
		{
			maxHeight = height;
		}
	}
	m_stripHeight = maxHeight + 4;
	maxHeight += 4;
	for (i = 0; i < m_controls.size(); i++)
	{
		int height;

		GetWindowRect(m_controls[i], &rect);
		screenToClient(hWindow, &rect);
		height = rect.bottom - rect.top;
		MoveWindow(m_controls[i], rect.left, (maxHeight - height) / 2,
			rect.right - rect.left, height, TRUE);
	}
	autoSize();
}

void ToolbarStrip::stepChanged(void)
{
	ModelWindow *modelWindow = m_ldviewWindow->getModelWindow();
	LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

	if (modelViewer)
	{
		std::string text;
		int step;

		windowGetText(IDC_STEP, text);
		if (sscanf(text.c_str(), "%d", &step) == 1)
		{
			if (step < 1)
			{
				step = 1;
			}
			if (step > modelViewer->getNumSteps())
			{
				step = modelViewer->getNumSteps();
			}
			m_step = step;
			modelViewer->setStep(step);
			modelWindow->forceRedraw();
			updateStep();
		}
	}
	else
	{
		updateStep();
	}
}

void ToolbarStrip::updateStep(void)
{
	LDrawModelViewer *modelViewer =
		m_ldviewWindow->getModelWindow()->getModelViewer();

	if (modelViewer)
	{
		m_step = modelViewer->getStep();
	}
	else
	{
		m_step = 0;
	}
	if (m_step > 0)
	{
		UCCHAR buf[1024];

		sucprintf(buf, COUNT_OF(buf), _UC("%d"), m_step);
		windowSetText(IDC_STEP, buf);
		if (GetFocus() == m_hStepField)
		{
			SendMessage(m_hStepField, EM_SETSEL, 0, (LPARAM)-1);
		}
	}
	else
	{
		windowSetText(IDC_STEP, "");
	}
}

void ToolbarStrip::updateNumSteps(void)
{
	LDrawModelViewer *modelViewer =
		m_ldviewWindow->getModelWindow()->getModelViewer();

	if (modelViewer)
	{
		m_numSteps = modelViewer->getNumSteps();
	}
	else
	{
		m_numSteps = 0;
	}
	if (m_numSteps > 0)
	{
		UCCHAR buf[1024];

		sucprintf(buf, COUNT_OF(buf), m_numStepsFormat.c_str(), m_numSteps);
		windowSetText(IDC_NUM_STEPS, buf);
	}
	else
	{
		windowSetText(IDC_NUM_STEPS, "");
	}
}

BOOL ToolbarStrip::doInitDialog(HWND /*hKbControl*/)
{
	m_hToolbar = GetDlgItem(hWindow, IDC_TOOLBAR);
	m_hStepLabel = GetDlgItem(hWindow, IDC_STEP_LABEL);
	m_hStepField = GetDlgItem(hWindow, IDC_STEP);
	m_hNumStepsLabel = GetDlgItem(hWindow, IDC_NUM_STEPS);
	m_hStepToolbar = GetDlgItem(hWindow, IDC_STEP_TOOLBAR);
	m_controls.push_back(m_hToolbar);
	m_controls.push_back(m_hStepLabel);
	m_controls.push_back(m_hStepField);
	m_controls.push_back(m_hNumStepsLabel);
	m_controls.push_back(m_hStepToolbar);

	windowGetText(IDC_NUM_STEPS, m_numStepsFormat);
	updateStep();
	updateNumSteps();
	initMainToolbar();
	initStepToolbar();
	initLayout();
	return TRUE;
}

LRESULT ToolbarStrip::doTextFieldChange(int controlId, HWND control)
{
	return CUIDialog::doTextFieldChange(controlId, control);
}

LRESULT ToolbarStrip::doCommand(
	int notifyCode,
	int commandId,
	HWND control)
{
	if (commandId == IDOK || notifyCode == EN_KILLFOCUS)
	{
		stepChanged();
		return 0;
	}
	return CUIDialog::doCommand(notifyCode, commandId, control);
}

void ToolbarStrip::addTbButtonInfo(
	TbButtonInfoVector &infos,
	CUCSTR tooltipText,
	int commandId,
	int stdBmpId,
	int tbBmpId,
	BYTE style,
	BYTE state)
{
	infos.resize(infos.size() + 1);
	TbButtonInfo &buttonInfo = infos.back();

	buttonInfo.setTooltipText(tooltipText);
	buttonInfo.setCommandId(commandId);
	buttonInfo.setStdBmpId(stdBmpId);
	buttonInfo.setTbBmpId(tbBmpId);
	buttonInfo.setStyle(style);
	buttonInfo.setState(state);
}

void ToolbarStrip::addTbCheckButtonInfo(
	TbButtonInfoVector &infos,
	CUCSTR tooltipText,
	int commandId,
	int stdBmpId,
	int tbBmpId,
	bool checked,
	BYTE style,
	BYTE state)
{
	state &= ~TBSTATE_CHECKED;
	if (checked)
	{
		state |= TBSTATE_CHECKED;
	}
	addTbButtonInfo(infos, tooltipText, commandId, stdBmpId, tbBmpId, style,
		state);
}

void ToolbarStrip::addTbSeparatorInfo(TbButtonInfoVector &infos)
{
	infos.resize(infos.size() + 1);
	TbButtonInfo &buttonInfo = infos.back();

	buttonInfo.setStyle(TBSTYLE_SEP);
}

void ToolbarStrip::populateStepTbButtonInfos(void)
{
	if (m_stepButtonInfos.size() == 0)
	{
		addTbButtonInfo(m_stepButtonInfos, _UC("Previous Step"), ID_PREV_STEP,
			-1, 0);
		addTbButtonInfo(m_stepButtonInfos, _UC("Next Step"), ID_NEXT_STEP, -1,
			1);
	}
}

void ToolbarStrip::populateMainTbButtonInfos(void)
{
	if (m_mainButtonInfos.size() == 0)
	{
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("OpenFile")), ID_FILE_OPEN, -1,
			10);
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("SaveSnapshot")), ID_FILE_SAVE,
			-1, 5);
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("Reload")), ID_FILE_RELOAD, -1,
			0);
		addTbSeparatorInfo(m_mainButtonInfos);
		m_drawWireframe = m_prefs->getDrawWireframe();
		m_seams = m_prefs->getUseSeams() != 0;
		m_edges = m_prefs->getShowsHighlightLines();
		m_primitiveSubstitution = m_prefs->getAllowPrimitiveSubstitution();
		m_lighting = m_prefs->getUseLighting();
		m_bfc = m_prefs->getBfc();
		addTbCheckButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("Wireframe")),
			IDC_WIREFRAME, -1, 1, m_drawWireframe,
			TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbCheckButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("Seams")), IDC_SEAMS, -1,
			2, m_seams);
		addTbCheckButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("EdgeLines")),
			IDC_HIGHLIGHTS, -1, 3, m_edges, TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbCheckButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("PrimitiveSubstitution")),
			IDC_PRIMITIVE_SUBSTITUTION, -1, 4, m_primitiveSubstitution,
			TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbCheckButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("Lighting")), IDC_LIGHTING,
			-1, 7, m_lighting, TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbCheckButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("BFC")), IDC_BFC, -1, 9,
			m_bfc, TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("SelectView")),
			ID_VIEWANGLE, -1, 6, TBSTYLE_DROPDOWN | BTNS_WHOLEDROPDOWN);
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("Preferences")), 
			ID_EDIT_PREFERENCES, -1, 8);
	}
}

// Note: static function
HBITMAP ToolbarStrip::createMask(HBITMAP hBitmap, COLORREF maskColor)
{
	BITMAP bitmap;
	TCByte *data;
	int bytesPerLine;
	int maskSize;
	int x, y;
	HDC hBmDc = CreateCompatibleDC(NULL);

	::GetObject((HANDLE)hBitmap, sizeof(BITMAP), &bitmap);
	bytesPerLine = ModelWindow::roundUp((bitmap.bmWidth + 7) / 8, 2);
	maskSize = bytesPerLine * bitmap.bmHeight;
	data = new TCByte[maskSize];
	memset(data, 0, maskSize);
	SelectObject(hBmDc, hBitmap);
	for (y = 0; y < bitmap.bmHeight; y++)
	{
		int yOffset = bytesPerLine * y;

		for (x = 0; x < bitmap.bmWidth; x++)
		{
			COLORREF pixelColor = GetPixel(hBmDc, x, y);

			if (pixelColor == maskColor)
			{
				int byteOffset = yOffset + x / 8;
				int bitOffset = 7 - (x % 8);

				data[byteOffset] |= (1 << bitOffset);
			}
		}
	}
	DeleteDC(hBmDc);
	return CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, data);
}

LRESULT ToolbarStrip::doNotify(int controlId, LPNMHDR notification)
{
	//if (notification->code == TBN_DROPDOWN)
	//{
	//	debugPrintf("Hmm.\n");
	//}
	//if (notification->hwndFrom == m_hToolbar)
	//{
	//	return SendMessage(m_ldviewWindow->getHWindow(), WM_NOTIFY,
	//		(WPARAM)controlId, (LPARAM)notification);
	//}
	//else
	{
		return CUIDialog::doNotify(controlId, notification);
	}
}
