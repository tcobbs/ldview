#include "LDViewErrors.h"
#include "ErrorPanel.h"
#include "Preferences.h"

#include <LDLoader/LDLError.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/mystring.h>

#include <qlistview.h>
#include <qstring.h>
#include <qheader.h>
#include <qbutton.h>
#include <qcheckbox.h>

LDViewErrors::LDViewErrors(Preferences *preferences)
	:panel(new ErrorPanel),
	preferences(preferences),
	errors(new LDLErrorArray),
	listViewPopulated(false)
{
	panel->setErrors(this);
	panel->errorListView->setColumnWidthMode(0, QListView::Maximum);
	panel->errorListView->header()->hide();
	preferences->setButtonState(panel->parseErrorButton,
		preferences->getShowError(LDLEParse));
	preferences->setButtonState(panel->fileNotFoundButton,
		preferences->getShowError(LDLEFileNotFound));
//	preferences->setButtonState(panel->colorErrorButton,
//		preferences->getShowError(LDLEColor));
	preferences->setButtonState(panel->partDeterminantButton,
		preferences->getShowError(LDLEPartDeterminant));
	preferences->setButtonState(panel->concaveQuadButton,
		preferences->getShowError(LDLEConcaveQuad));
	preferences->setButtonState(panel->concaveQuadSplitButton,
		preferences->getShowError(LDLEConcaveQuadSplit));
	preferences->setButtonState(panel->colinearPointsButton,
		preferences->getShowError(LDLEColinear));
//	preferences->setButtonState(panel->openGLErrorButton,
//		preferences->getShowError(LDLEOpenGL));
}

LDViewErrors::~LDViewErrors(void)
{
	if (errors)
	{
		errors->release();
	}
	delete panel;
}

void LDViewErrors::show(void)
{
	panel->show();
	panel->raise();
	panel->setActiveWindow();
}

void LDViewErrors::clear(void)
{
	errors->removeAll();
	clearListView();
}

void LDViewErrors::clearListView(void)
{
	panel->errorListView->clear();
	listViewPopulated = false;
}

void LDViewErrors::addError(LDLError *error)
{
	errors->addObject(error);
}

int LDViewErrors::populateListView(void)
{
	int errorCount = 0;
	
	if (!listViewPopulated)
	{
		int i;
		int count = errors->getCount();

		for (i = 0; i < count; i++)
		{
			if (addErrorToListView((*errors)[i]))
			{
				errorCount++;
			}
		}
		listViewPopulated = true;
	}
	return errorCount;
}

bool LDViewErrors::addErrorToListView(LDLError *error)
{
	char *string;
	QListViewItem *parent;
	QString buf;

	if (!showsErrorType(error->getType()))
	{
		return false;
	}
	string = error->getMessage();
	parent = addErrorLine(NULL, string, error);
	if (parent)
	{
		TCStringArray *extraInfo;

		string = error->getFilename();
		if (string)
		{
			buf.sprintf("File: %s", string);
		}
		else
		{
			buf.sprintf("Unknown filename");
		}
		addErrorLine(parent, buf, error);
		string = error->getFileLine();
		if (string)
		{
			int lineNumber = error->getLineNumber();
			
			if (lineNumber > 0)
			{
				buf.sprintf("Line #%d", lineNumber);
			}
			else
			{
				buf.sprintf("Unknown Line #");
			}
			addErrorLine(parent, buf, error);
			stripCRLF(string);
			buf.sprintf("Line: %s", string);
		}
		else
		{
			buf.sprintf("Unknown Line");
		}
		addErrorLine(parent, buf, error);
		if ((extraInfo = error->getExtraInfo()) != NULL)
		{
			int i;
			int count = extraInfo->getCount();
			
			for (i = 0; i < count; i++)
			{
				addErrorLine(parent, extraInfo->stringAtIndex(i), error);
			}
		}
	}
	return true;
}

bool LDViewErrors::showsErrorType(LDLErrorType errorType)
{
	return preferences->getShowError(errorType);
}

QListViewItem *LDViewErrors::addErrorLine(QListViewItem *parent,
	const char *line, LDLError * /*error*/, int /*imageIndex*/)
{
	QListViewItem *item;

	if (parent)
	{
		item = new QListViewItem(parent, line);
	}
	else
	{
		item = new QListViewItem(panel->errorListView, line);
	}
	return item;
}

void LDViewErrors::doErrorClick(QButton *button, LDLErrorType errorType)
{
	preferences->setShowError(errorType, button->state());
	clearListView();
	populateListView();
}
