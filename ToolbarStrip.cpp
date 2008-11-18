#include "ToolbarStrip.h"
#include "Resource.h"
#include "LDViewWindow.h"
#include "ModelWindow.h"
#include "LDViewPreferences.h"
#include <TCFoundation/TCJpegOptions.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCAlertManager.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

ToolbarStrip::ToolbarStrip(HINSTANCE hInstance):
CUIDialog(hInstance),
m_stdBitmapStartId(-1),
m_tbBitmapStartId(-1),
m_numSteps(0),
m_hDeactivatedTooltip(NULL)
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
	HImageListList::iterator it;

	for (it = m_imageLists.begin(); it != m_imageLists.end(); it++)
	{
		ImageList_Destroy(*it);
	}
	TCAlertManager::unregisterHandler(this);
	m_prefs->release();
	CUIDialog::dealloc();
}

void ToolbarStrip::modelAlertCallback(TCAlert *alert)
{
	if (alert->getSender() == m_ldviewWindow->getModelWindow())
	{
		if (ucstrcmp(alert->getMessageUC(), _UC("ModelLoaded")) == 0 ||
			ucstrcmp(alert->getMessageUC(), _UC("ModelLoadCanceled")) == 0 ||
			ucstrcmp(alert->getMessageUC(), _UC("ModelParsed")) == 0 ||
			ucstrcmp(alert->getMessageUC(), _UC("ModelParseCanceled")) == 0)
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

void ToolbarStrip::initImageList(HWND hToolbar, UINT bitmapId)
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
	m_imageLists.push_back(imageList);
}

void ToolbarStrip::initToolbar(
	HWND hToolbar,
	TbButtonInfoVector &infos,
	UINT bitmapId)
{
	TBBUTTON *buttons;
	char buttonTitle[128];
	int i;
	int count;
	RECT rect;
	RECT buttonRect;
	int width;
	int height;

#ifndef TC_NO_UNICODE
	SendMessage(hToolbar, TB_SETUNICODEFORMAT, (WPARAM)TRUE, 0);
#endif // !TC_NO_UNICODE
	SendMessage(hToolbar, TB_SETEXTENDEDSTYLE, 0,
		TBSTYLE_EX_DRAWDDARROWS | WS_EX_TRANSPARENT);
	memset(buttonTitle, 0, sizeof(buttonTitle));
	SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(hToolbar, TB_SETBUTTONWIDTH, 0, MAKELONG(22, 22));
	initImageList(hToolbar, bitmapId);
	m_stdBitmapStartId = m_tbBitmapStartId = 0;
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
	if (!CUIThemes::isThemeActive() ||
		((CUIThemes::getThemeAppProperties() & STAP_ALLOW_CONTROLS) == 0))
	{
		SendMessage(hToolbar, TB_SETBUTTONSIZE, 0, MAKELONG(22, 24));
	}
	GetWindowRect(hToolbar, &rect);
	screenToClient(hWindow, &rect);
	if (!SendMessage(hToolbar, TB_GETRECT, buttons[count - 1].idCommand,
		(LPARAM)&buttonRect))
	{
		buttonRect = rect;
		buttonRect.left -= rect.left;
		buttonRect.right -= rect.right;
	}
	delete[] buttons;
	width = buttonRect.right;
	height = buttonRect.bottom - buttonRect.top;
	MoveWindow(hToolbar, rect.left, rect.top, width, height, FALSE);
	SendMessage(hToolbar, TB_SETHOTITEM, (WPARAM)-1, 0);
	ShowWindow(hToolbar, SW_SHOW);
}

void ToolbarStrip::loadMainToolbarMenus(void)
{
	m_hMainToolbarMenu = LoadMenu(getLanguageModule(),
		MAKEINTRESOURCE(IDR_TOOLBAR_MENU));
	m_hWireframeMenu = GetSubMenu(m_hMainToolbarMenu, 0);
	m_hEdgesMenu = GetSubMenu(m_hMainToolbarMenu, 1);
	m_hPrimitivesMenu = GetSubMenu(m_hMainToolbarMenu, 2);
	m_hLightingMenu = GetSubMenu(m_hMainToolbarMenu, 3);
	m_hBFCMenu = GetSubMenu(m_hMainToolbarMenu, 4);
}

void ToolbarStrip::initMainToolbar(void)
{
	loadMainToolbarMenus();
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
			m_step = m_ldviewWindow->setStep(step);
			updateStep();
		}
	}
	else
	{
		updateStep();
	}
}

void ToolbarStrip::enableToolbarButton(
	HWND hToolbar,
	UINT buttonId,
	bool enable)
{
	BYTE state = (BYTE)SendMessage(hToolbar, TB_GETSTATE, (WPARAM)buttonId, 0);

	if (state == -1)
	{
		state = 0;
	}
	if (enable)
	{
		state |= TBSTATE_ENABLED;
	}
	else
	{
		state &= ~TBSTATE_ENABLED;
	}
	SendMessage(hToolbar, TB_SETSTATE, (WPARAM)buttonId, MAKELONG(state, 0));
}

void ToolbarStrip::updateStep(void)
{
	LDrawModelViewer *modelViewer =
		m_ldviewWindow->getModelWindow()->getModelViewer();
	bool prevEnabled = false;
	bool nextEnabled = false;

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
		prevEnabled = m_step > 1;
		nextEnabled = m_step < modelViewer->getNumSteps();
	}
	else
	{
		windowSetText(IDC_STEP, "--");
	}
	enableToolbarButton(m_hStepToolbar, ID_FIRST_STEP, prevEnabled);
	enableToolbarButton(m_hStepToolbar, ID_PREV_STEP, prevEnabled);
	enableToolbarButton(m_hStepToolbar, ID_NEXT_STEP, nextEnabled);
	enableToolbarButton(m_hStepToolbar, ID_LAST_STEP, nextEnabled);
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
		//updateStep();
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
	initMainToolbar();
	initStepToolbar();
	initLayout();
	updateStep();
	updateNumSteps();
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
	switch (commandId)
	{
	case IDOK:
	case EN_KILLFOCUS:
		stepChanged();
		return 0;
	case ID_EDIT_PREFERENCES:
	case ID_FILE_OPEN:
	case ID_FILE_SAVE:
	case ID_FILE_RELOAD:
	case ID_VIEW_RESET:
	case ID_VIEW_DEFAULT:
	case ID_VIEW_FRONT:
	case ID_VIEW_BACK:
	case ID_VIEW_LEFT:
	case ID_VIEW_RIGHT:
	case ID_VIEW_TOP:
	case ID_VIEW_BOTTOM:
	case ID_VIEW_SPECIFYLATLON:
	case ID_VIEW_ISO:
	case ID_VIEW_SAVE_DEFAULT:
	case ID_NEXT_STEP:
	case ID_PREV_STEP:
	case ID_FIRST_STEP:
	case ID_LAST_STEP:
		// Forward all these messages to LDViewWindow.
		return SendMessage(m_ldviewWindow->getHWindow(), WM_COMMAND,
			MAKEWPARAM(commandId, notifyCode), (LPARAM)control);
	case IDC_WIREFRAME:
		doWireframe();
		break;
	case IDC_SEAMS:
		doSeams();
		break;
	case IDC_HIGHLIGHTS:
		doEdges();
		break;
	case IDC_PRIMITIVE_SUBSTITUTION:
		doPrimitiveSubstitution();
		break;
	case IDC_LIGHTING:
		doLighting();
		break;
	case IDC_BFC:
		doBfc();
		break;
	case IDC_SHOW_AXES:
		doShowAxes();
		break;
	case ID_WIREFRAME_FOG:
		doFog();
		break;
	case ID_WIREFRAME_REMOVEHIDDENLINES:
		doRemoveHiddenLines();
		break;
	case ID_EDGES_SHOWEDGESONLY:
		doShowEdgesOnly();
		break;
	case ID_EDGES_CONDITIONALLINES:
		doConditionalLines();
		break;
	case ID_EDGES_HIGHQUALITY:
		doHighQualityEdges();
		break;
	case ID_EDGES_ALWAYSBLACK:
		doAlwaysBlack();
		break;
	case ID_PRIMITIVES_TEXTURESTUDS:
		doTextureStuds();
		break;
	case ID_LIGHTING_HIGHQUALITY:
		doQualityLighting();
		break;
	case ID_LIGHTING_SUBDUED:
		doSubduedLighting();
		break;
	case ID_LIGHTING_SPECULARHIGHLIGHT:
		doSpecularHighlight();
		break;
	case ID_LIGHTING_ALTERNATESETUP:
		doAlternateLighting();
		break;
	case ID_LIGHTING_DRAWDATS:
		doDrawLightDats();
		break;
	case ID_LIGHTING_OPTIONALMAIN:
		doOptionalStandardLight();
		break;
	case ID_BFC_REDBACKFACES:
		doRedBackFaces();
		break;
	case ID_BFC_GREENFRONTFACES:
		doGreenFrontFaces();
		break;
	default:
		return CUIDialog::doCommand(notifyCode, commandId, control);
	}
	return 0;
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
		addTbButtonInfo(m_stepButtonInfos,
			TCLocalStrings::get(_UC("FirstStep")), ID_FIRST_STEP, -1, 2);
		addTbButtonInfo(m_stepButtonInfos,
			TCLocalStrings::get(_UC("PrevStep")), ID_PREV_STEP,
			-1, 0);
		addTbButtonInfo(m_stepButtonInfos,
			TCLocalStrings::get(_UC("NextStep")), ID_NEXT_STEP, -1,
			1);
		addTbButtonInfo(m_stepButtonInfos,
			TCLocalStrings::get(_UC("LastStep")), ID_LAST_STEP, -1,
			3);
	}
}

void ToolbarStrip::populateMainTbButtonInfos(void)
{
	if (m_mainButtonInfos.size() == 0)
	{
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("OpenFile")),
			ID_FILE_OPEN, -1, 10);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("SaveSnapshot")), ID_FILE_SAVE, -1, 5);
		addTbButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("Reload")),
			ID_FILE_RELOAD, -1, 0);
		addTbSeparatorInfo(m_mainButtonInfos);
		m_drawWireframe = m_prefs->getDrawWireframe();
		m_seams = m_prefs->getUseSeams() != 0;
		m_edges = m_prefs->getShowsHighlightLines();
		m_primitiveSubstitution = m_prefs->getAllowPrimitiveSubstitution();
		m_lighting = m_prefs->getUseLighting();
		m_bfc = m_prefs->getBfc();
		m_showAxes = m_prefs->getShowAxes();
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("Wireframe")), IDC_WIREFRAME, -1, 1,
			m_drawWireframe, TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("Seams")), IDC_SEAMS, -1, 2, m_seams);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("EdgeLines")), IDC_HIGHLIGHTS, -1, 3,
			m_edges, TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("PrimitiveSubstitution")),
			IDC_PRIMITIVE_SUBSTITUTION, -1, 4, m_primitiveSubstitution,
			TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("Lighting")), IDC_LIGHTING, -1, 7,
			m_lighting, TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbCheckButtonInfo(m_mainButtonInfos, TCLocalStrings::get(_UC("BFC")),
			IDC_BFC, -1, 9, m_bfc, TBSTYLE_CHECK | TBSTYLE_DROPDOWN);
		addTbCheckButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("ShowAxes")), IDC_SHOW_AXES, -1, 11,
			m_showAxes);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("SelectView")), ID_VIEWANGLE, -1, 6,
			TBSTYLE_DROPDOWN | BTNS_WHOLEDROPDOWN);
		addTbButtonInfo(m_mainButtonInfos,
			TCLocalStrings::get(_UC("Preferences")), ID_EDIT_PREFERENCES, -1,
			8);
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
	HBITMAP hOldBitmap;
	HBITMAP hNewBitmap;

	::GetObject((HANDLE)hBitmap, sizeof(BITMAP), &bitmap);
	bytesPerLine = ModelWindow::roundUp((bitmap.bmWidth + 7) / 8, 2);
	maskSize = bytesPerLine * bitmap.bmHeight;
	data = new TCByte[maskSize];
	memset(data, 0, maskSize);
	hOldBitmap = (HBITMAP)SelectObject(hBmDc, hBitmap);
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
	SelectObject(hBmDc, hOldBitmap);
	DeleteDC(hBmDc);
	hNewBitmap = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, data);
	delete data;
	return hNewBitmap;
}

//static const std::string notificationName(UINT code)
//{
//	static char buf[128];
//
//	switch (code)
//	{
//	case TBN_GETBUTTONINFOA:
//		return "TBN_GETBUTTONINFOA";
//	case TBN_BEGINDRAG:
//		return "TBN_BEGINDRAG";
//	case TBN_ENDDRAG:
//		return "TBN_ENDDRAG";
//	case TBN_BEGINADJUST:
//		return "TBN_BEGINADJUST";
//	case TBN_ENDADJUST:
//		return "TBN_ENDADJUST";
//	case TBN_RESET:
//		return "TBN_RESET";
//	case TBN_QUERYINSERT:
//		return "TBN_QUERYINSERT";
//	case TBN_QUERYDELETE:
//		return "TBN_QUERYDELETE";
//	case TBN_TOOLBARCHANGE:
//		return "TBN_TOOLBARCHANGE";
//	case TBN_CUSTHELP:
//		return "TBN_CUSTHELP";
//	case TBN_DROPDOWN:
//		return "TBN_DROPDOWN";
//	case TBN_GETOBJECT:
//		return "TBN_GETOBJECT";
//	case TBN_HOTITEMCHANGE:
//		return "TBN_HOTITEMCHANGE";
//	case TBN_DRAGOUT:
//		return "TBN_DRAGOUT";
//	case TBN_DELETINGBUTTON:
//		return "TBN_DELETINGBUTTON";
//	case TBN_GETDISPINFOA:
//		return "TBN_GETDISPINFOA";
//	case TBN_GETDISPINFOW:
//		return "TBN_GETDISPINFOW";
//	case TBN_GETINFOTIPA:
//		return "TBN_GETINFOTIPA";
//	case TBN_GETINFOTIPW:
//		return "TBN_GETINFOTIPW";
//	case TBN_GETBUTTONINFOW:
//		return "TBN_GETBUTTONINFOW";
//	case TBN_RESTORE:
//		return "TBN_RESTORE";
//	case TBN_SAVE:
//		return "TBN_SAVE";
//	case TBN_INITCUSTOMIZE:
//		return "TBN_INITCUSTOMIZE";
//
//	case TTN_GETDISPINFOA:
//		return "TTN_GETDISPINFOA";
//	case TTN_GETDISPINFOW:
//		return "TTN_GETDISPINFOW";
//	case TTN_SHOW:
//		return "TTN_SHOW";
//	case TTN_POP:
//		return "TTN_POP";
//	case TTN_LINKCLICK:
//		return "TTN_LINKCLICK";
//
//	case NM_TOOLTIPSCREATED:
//		return "NM_TOOLTIPSCREATED";
//	default:
//		sprintf(buf, "0x%08X", code);
//		return buf;
//	}
//}

LRESULT ToolbarStrip::doToolbarGetInfotip(
	TbButtonInfoVector &infos,
	LPNMTBGETINFOTIPUC dispInfo)
{
	int i;
	int count = (int)infos.size();

	for (i = 0; i < count; i++)
	{
		TbButtonInfo &buttonInfo = infos[i];

		if (buttonInfo.getCommandId() == dispInfo->iItem)
		{
			if (CUIThemes::isThemeLibLoaded())
			{
				//HWND hTooltip = (HWND)SendMessage(dispInfo->hdr.hwndFrom,
				//	TB_GETTOOLTIPS, 0, 0);
				//// Turning off theme support in the tooltip makes it
				//// work properly.  With theme support on, it gets erased
				//// by the OpenGL window immediately after being drawn.
				//// Haven't the foggiest why this happens, but turning
				//// off theme support solves the problem.  This has to
				//// be done every time the tooltip is about to pop up.
				//// Not sure why that is either, but it is.
				//CUIThemes::setWindowTheme(hTooltip, NULL, L"");
			}
			if (m_ldviewWindow->getLoading())
			{
				// Don't allow tooltips to pop up while loading; they
				// prevent the loading from continuing until they go
				// away.
				dispInfo->pszText = NULL;
			}
			else
			{
				ucstrncpy(dispInfo->pszText, buttonInfo.getTooltipText(),
					dispInfo->cchTextMax);
				dispInfo->pszText[dispInfo->cchTextMax - 1] = 0;
			}
			break;
		}
	}
	return 0;
}

LRESULT ToolbarStrip::doMainToolbarNotify(int controlId, LPNMHDR notification)
{
	//debugPrintf("LDViewWindow::doMainToolbarNotify: 0x%04X, %s\n", controlId,
	//	notificationName(notification->code).c_str());
	switch (notification->code)
	{
	case TBN_GETINFOTIPUC:
		return doToolbarGetInfotip(m_mainButtonInfos,
			(LPNMTBGETINFOTIPUC)notification);
	case TBN_DROPDOWN:
		if (notification->idFrom == ID_TOOLBAR ||
			notification->idFrom == IDC_TOOLBAR)
		{
			doDropDown((LPNMTOOLBAR)notification);
		}
		break;
	default:
		return CUIWindow::doNotify(controlId, notification);
	}
	return 0;
}

LRESULT ToolbarStrip::doStepToolbarNotify(int controlId, LPNMHDR notification)
{
	//debugPrintf("LDViewWindow::doStepToolbarNotify: 0x%04X, %s\n", controlId,
	//	notificationName(notification->code).c_str());
	switch (notification->code)
	{
	case TBN_GETINFOTIPUC:
		return doToolbarGetInfotip(m_stepButtonInfos,
			(LPNMTBGETINFOTIPUC)notification);
	default:
		return CUIWindow::doNotify(controlId, notification);
	}
	return 0;
}

LRESULT ToolbarStrip::doNotify(int controlId, LPNMHDR notification)
{
	if (notification->hwndFrom == m_hToolbar)
	{
		return doMainToolbarNotify(controlId, notification);
	}
	else if (notification->hwndFrom == m_hStepToolbar)
	{
		return doStepToolbarNotify(controlId, notification);
	}
	else
	{
		return CUIDialog::doNotify(controlId, notification);
	}
}

void ToolbarStrip::enableMainToolbarButton(UINT buttonId, bool enable)
{
	enableToolbarButton(m_hToolbar, buttonId, enable);
}

void ToolbarStrip::updateWireframeMenu(void)
{
	setMenuCheck(m_hWireframeMenu, ID_WIREFRAME_FOG,
		m_prefs->getUseWireframeFog());
	setMenuCheck(m_hWireframeMenu, ID_WIREFRAME_REMOVEHIDDENLINES,
		m_prefs->getRemoveHiddenLines());
	setMenuItemsEnabled(m_hWireframeMenu, m_drawWireframe);
}

void ToolbarStrip::updateEdgesMenu(void)
{
	setMenuCheck(m_hEdgesMenu, ID_EDGES_SHOWEDGESONLY,
		m_prefs->getEdgesOnly());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_CONDITIONALLINES,
		m_prefs->getDrawConditionalHighlights());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_HIGHQUALITY,
		m_prefs->getUsePolygonOffset());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_ALWAYSBLACK,
		m_prefs->getBlackHighlights());
	setMenuItemsEnabled(m_hEdgesMenu, m_edges);
}

void ToolbarStrip::updatePrimitivesMenu(void)
{
	setMenuCheck(m_hPrimitivesMenu, ID_PRIMITIVES_TEXTURESTUDS,
		m_prefs->getTextureStuds());
	setMenuItemsEnabled(m_hPrimitivesMenu, m_primitiveSubstitution);
}

void ToolbarStrip::updateLightingMenu(void)
{
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_HIGHQUALITY,
		m_prefs->getQualityLighting());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_SUBDUED,
		m_prefs->getSubduedLighting());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_SPECULARHIGHLIGHT,
		m_prefs->getUsesSpecular());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_ALTERNATESETUP,
		m_prefs->getOneLight());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_DRAWDATS,
		m_prefs->getDrawLightDats());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_OPTIONALMAIN,
		m_prefs->getOptionalStandardLight());
	setMenuItemsEnabled(m_hLightingMenu, m_lighting);
	if (!m_prefs->getDrawLightDats())
	{
		setMenuEnabled(m_hLightingMenu, ID_LIGHTING_OPTIONALMAIN, false,
			FALSE);
		setMenuCheck(m_hLightingMenu, ID_LIGHTING_OPTIONALMAIN, false);
	}
}

void ToolbarStrip::updateBFCMenu(void)
{
	setMenuCheck(m_hBFCMenu, ID_BFC_REDBACKFACES,
		m_prefs->getRedBackFaces());
	setMenuCheck(m_hBFCMenu, ID_BFC_GREENFRONTFACES,
		m_prefs->getGreenFrontFaces());
	setMenuItemsEnabled(m_hBFCMenu, m_bfc);
}

void ToolbarStrip::doDropDown(LPNMTOOLBAR toolbarNot)
{
	RECT rect;
	TPMPARAMS tpm;
	HMENU hMenu = NULL;

    SendMessage(toolbarNot->hdr.hwndFrom, TB_GETRECT, (WPARAM)toolbarNot->iItem,
		(LPARAM)&rect);
	MapWindowPoints(toolbarNot->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rect, 2);
	tpm.cbSize = sizeof(TPMPARAMS);
	tpm.rcExclude.top    = rect.top;
	tpm.rcExclude.left   = rect.left;
	tpm.rcExclude.bottom = rect.bottom;
	tpm.rcExclude.right  = rect.right;
	switch (toolbarNot->iItem)
	{
	case IDC_WIREFRAME:
		hMenu = m_hWireframeMenu;
		break;
	case IDC_HIGHLIGHTS:
		hMenu = m_hEdgesMenu;
		break;
	case IDC_PRIMITIVE_SUBSTITUTION:
		hMenu = m_hPrimitivesMenu;
		break;
	case IDC_LIGHTING:
		hMenu = m_hLightingMenu;
		break;
	case IDC_BFC:
		hMenu = m_hBFCMenu;
		break;
	case ID_VIEWANGLE:
		hMenu = m_ldviewWindow->getViewAngleMenu();
		break;
	}
	if (hMenu)
	{
		BOOL fade;

		// Note: selection fade causes a display glitch.  This is an obscenely
		// ugly hack to get rid of that, but it does work.  The problem still
		// exists when the menu item is selected from the main menu, though.
		// Did I mention yet today that Microsoft sucks?
		SystemParametersInfo(SPI_GETSELECTIONFADE, 0, &fade, 0);
		if (fade)
		{
			// We better pray it doesn't crash between here and where we put
			// things back below.
			SystemParametersInfo(SPI_SETSELECTIONFADE, FALSE, NULL, 0);
		}
		TrackPopupMenuEx(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL,
			rect.left, rect.bottom, hWindow, &tpm);
		if (fade)
		{
			SystemParametersInfo(SPI_SETSELECTIONFADE, 0, &fade, 0);
		}
	}
}

void ToolbarStrip::forceRedraw(void)
{
	ModelWindow *modelWindow = m_ldviewWindow->getModelWindow();

	if (modelWindow)
	{
		modelWindow->forceRedraw();
	}
}

void ToolbarStrip::doWireframe(void)
{
	if (doCheck(m_drawWireframe, IDC_WIREFRAME))
	{
		m_prefs->setDrawWireframe(m_drawWireframe);
		forceRedraw();
	}
}

void ToolbarStrip::doSeams(void)
{
	if (doCheck(m_seams, IDC_SEAMS))
	{
		m_prefs->setUseSeams(m_seams);
		forceRedraw();
	}
}

void ToolbarStrip::doEdges(void)
{
	if (doCheck(m_edges, IDC_HIGHLIGHTS))
	{
		m_prefs->setShowsHighlightLines(m_edges);
		forceRedraw();
	}
}

void ToolbarStrip::doPrimitiveSubstitution(void)
{
	if (doCheck(m_primitiveSubstitution, IDC_PRIMITIVE_SUBSTITUTION))
	{
		m_prefs->setAllowPrimitiveSubstitution(m_primitiveSubstitution);
		forceRedraw();
	}
}

void ToolbarStrip::doLighting(void)
{
	if (doCheck(m_lighting, IDC_LIGHTING))
	{
		m_prefs->setUseLighting(m_lighting);
		forceRedraw();
	}
}

void ToolbarStrip::doFog(void)
{
	m_prefs->setUseWireframeFog(!m_prefs->getUseWireframeFog());
	setMenuCheck(m_hWireframeMenu, ID_WIREFRAME_FOG,
		m_prefs->getUseWireframeFog());
}

void ToolbarStrip::doRemoveHiddenLines(void)
{
	m_prefs->setRemoveHiddenLines(!m_prefs->getRemoveHiddenLines());
	setMenuCheck(m_hWireframeMenu, ID_WIREFRAME_REMOVEHIDDENLINES,
		m_prefs->getRemoveHiddenLines());
}

void ToolbarStrip::doShowEdgesOnly(void)
{
	m_prefs->setEdgesOnly(!m_prefs->getEdgesOnly());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_SHOWEDGESONLY,
		m_prefs->getEdgesOnly());
}

void ToolbarStrip::doConditionalLines(void)
{
	m_prefs->setDrawConditionalHighlights(!m_prefs->getDrawConditionalHighlights());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_CONDITIONALLINES,
		m_prefs->getDrawConditionalHighlights());
}

void ToolbarStrip::doHighQualityEdges(void)
{
	m_prefs->setUsePolygonOffset(!m_prefs->getUsePolygonOffset());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_HIGHQUALITY,
		m_prefs->getUsePolygonOffset());
}

void ToolbarStrip::doAlwaysBlack(void)
{
	m_prefs->setBlackHighlights(!m_prefs->getBlackHighlights());
	setMenuCheck(m_hEdgesMenu, ID_EDGES_ALWAYSBLACK,
		m_prefs->getBlackHighlights());
}

void ToolbarStrip::doTextureStuds(void)
{
	m_prefs->setTextureStuds(!m_prefs->getTextureStuds());
	setMenuCheck(m_hPrimitivesMenu, ID_PRIMITIVES_TEXTURESTUDS,
		m_prefs->getTextureStuds());
}

void ToolbarStrip::doQualityLighting(void)
{
	m_prefs->setQualityLighting(!m_prefs->getQualityLighting());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_HIGHQUALITY,
		m_prefs->getQualityLighting());
}

void ToolbarStrip::doSubduedLighting(void)
{
	m_prefs->setSubduedLighting(!m_prefs->getSubduedLighting());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_SUBDUED,
		m_prefs->getSubduedLighting());
}

void ToolbarStrip::doSpecularHighlight(void)
{
	m_prefs->setUsesSpecular(!m_prefs->getUsesSpecular());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_SPECULARHIGHLIGHT,
		m_prefs->getUsesSpecular());
}

void ToolbarStrip::doAlternateLighting(void)
{
	m_prefs->setOneLight(!m_prefs->getOneLight());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_ALTERNATESETUP,
		m_prefs->getOneLight());
}

void ToolbarStrip::doDrawLightDats(void)
{
	m_prefs->setDrawLightDats(!m_prefs->getDrawLightDats());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_DRAWDATS,
		m_prefs->getDrawLightDats());
}

void ToolbarStrip::doOptionalStandardLight(void)
{
	m_prefs->setOptionalStandardLight(!m_prefs->getOptionalStandardLight());
	setMenuCheck(m_hLightingMenu, ID_LIGHTING_OPTIONALMAIN,
		m_prefs->getOptionalStandardLight());
}

void ToolbarStrip::doRedBackFaces(void)
{
	m_prefs->setRedBackFaces(!m_prefs->getRedBackFaces());
	setMenuCheck(m_hBFCMenu, ID_BFC_REDBACKFACES,
		m_prefs->getRedBackFaces());
}

void ToolbarStrip::doGreenFrontFaces(void)
{
	m_prefs->setGreenFrontFaces(!m_prefs->getGreenFrontFaces());
	setMenuCheck(m_hBFCMenu, ID_BFC_GREENFRONTFACES,
		m_prefs->getGreenFrontFaces());
}

void ToolbarStrip::doBfc(void)
{
	if (doCheck(m_bfc, IDC_BFC))
	{
		m_prefs->setBfc(m_bfc);
		forceRedraw();
	}
}

void ToolbarStrip::doShowAxes(void)
{
	if (doCheck(m_showAxes, IDC_SHOW_AXES))
	{
		m_prefs->setShowAxes(m_showAxes);
		forceRedraw();
	}
}

bool ToolbarStrip::doCheck(bool &value, LPARAM commandId)
{
	BYTE state = (BYTE)SendMessage(m_hToolbar, TB_GETSTATE, commandId, 0);
	bool newValue = false;

	if (state & TBSTATE_CHECKED)
	{
		newValue = true;
	}
	if (newValue != value)
	{
		if (m_hDeactivatedTooltip)
		{
			SendMessage(m_hDeactivatedTooltip, TTM_ACTIVATE, 1, 0);
		}
		m_hDeactivatedTooltip = (HWND)SendMessage(m_hToolbar, TB_GETTOOLTIPS, 0,
			0);
		SendMessage(m_hDeactivatedTooltip, TTM_POP, 0, 0);
		SendMessage(m_hDeactivatedTooltip, TTM_ACTIVATE, 0, 0);
		value = newValue;
		return true;
	}
	else
	{
		return false;
	}
}

void ToolbarStrip::activateDeactivatedTooltip(void)
{
	if (m_hDeactivatedTooltip)
	{
		SendMessage(m_hDeactivatedTooltip, TTM_ACTIVATE, 1, 0);
	}
}

LRESULT ToolbarStrip::doInitMenuPopup(
	HMENU hPopupMenu,
	UINT /*uPos*/,
	BOOL /*fSystemMenu*/)
{
	if (hPopupMenu == m_hWireframeMenu)
	{
		updateWireframeMenu();
	}
	else if (hPopupMenu == m_hEdgesMenu)
	{
		updateEdgesMenu();
	}
	else if (hPopupMenu == m_hPrimitivesMenu)
	{
		updatePrimitivesMenu();
	}
	else if (hPopupMenu == m_hLightingMenu)
	{
		updateLightingMenu();
	}
	else if (hPopupMenu == m_hBFCMenu)
	{
		updateBFCMenu();
	}
	return 1;
}

void ToolbarStrip::checkReflect(bool &value, bool prefsValue, LPARAM commandID)
{
	if (value != prefsValue)
	{
		BYTE state = (BYTE)SendMessage(m_hToolbar, TB_GETSTATE, commandID, 0);

		value = prefsValue;
		state &= ~TBSTATE_CHECKED;
		if (value)
		{
			state |= TBSTATE_CHECKED;
		}
		SendMessage(m_hToolbar, TB_SETSTATE, commandID, MAKELONG(state, 0));
	}
}

void ToolbarStrip::checksReflect(void)
{
	checkReflect(m_drawWireframe, m_prefs->getDrawWireframe(), IDC_WIREFRAME);
	checkReflect(m_seams, m_prefs->getUseSeams() != 0, IDC_SEAMS);
	checkReflect(m_edges, m_prefs->getShowsHighlightLines(), IDC_HIGHLIGHTS);
	checkReflect(m_primitiveSubstitution,
		m_prefs->getAllowPrimitiveSubstitution(), IDC_PRIMITIVE_SUBSTITUTION);
	checkReflect(m_lighting, m_prefs->getUseLighting(), IDC_LIGHTING);
	checkReflect(m_bfc, m_prefs->getBfc(), IDC_BFC);
	checkReflect(m_showAxes, m_prefs->getShowAxes(), IDC_SHOW_AXES);
}

LRESULT ToolbarStrip::doEnterMenuLoop(bool /*isTrackPopupMenu*/)
{
	ModelWindow *modelWindow = m_ldviewWindow->getModelWindow();

	if (m_ldviewWindow->getLoading())
	{
		return 0;
	}
	if (modelWindow)
	{
		LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			modelViewer->pause();
		}
	}
	return 1;
}

LRESULT ToolbarStrip::doExitMenuLoop(bool /*isTrackPopupMenu*/)
{
	ModelWindow *modelWindow = m_ldviewWindow->getModelWindow();

	if (modelWindow)
	{
		LDrawModelViewer *modelViewer = modelWindow->getModelViewer();

		if (modelViewer)
		{
			modelViewer->unpause();
		}
		modelWindow->forceRedraw();
	}
	return 1;
}
