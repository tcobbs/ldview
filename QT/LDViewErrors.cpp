#include "qt4wrapper.h"
#include <qradiobutton.h>
#include "Preferences.h"

#include "LDViewErrors.h"

#include <LDLoader/LDLError.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDUserDefaultsKeys.h>

#include <qstring.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qstatusbar.h>
#include <qregexp.h>

LDViewErrors::LDViewErrors(Preferences *preferences)
	:panel(new ErrorPanel),
	preferences(preferences),
	errors(new LDLErrorArray),
	listViewPopulated(false)
{
	panel->setErrors(this);
	panel->errorListView->setColumnWidthMode(0, QListView::Maximum);
	panel->errorListView->header()->hide();
	panel->errorListView->setSorting(-1);
	reflectSettings();
	statusBar = panel->statusBar();
	messageText = new QLabel(statusBar);
	statusBar->addWidget(messageText,1);
}

void LDViewErrors::reflectSettings(void)
{ 
	preferences->setButtonState(panel->generalErrorButton,
		preferences->getShowError(LDLEGeneral));
        preferences->setButtonState(panel->parseErrorButton,
		preferences->getShowError(LDLEParse));
	preferences->setButtonState(panel->fileNotFoundButton,
		preferences->getShowError(LDLEFileNotFound));
//	preferences->setButtonState(panel->colorErrorButton,
//		preferences->getShowError(LDLEColor));
	preferences->setButtonState(panel->singularMatrixButton,
		preferences->getShowError(LDLEMatrix));
	preferences->setButtonState(panel->partDeterminantButton,
		preferences->getShowError(LDLEPartDeterminant));
	preferences->setButtonState(panel->concaveQuadButton,
		preferences->getShowError(LDLEConcaveQuad));
	preferences->setButtonState(panel->badVertexOrderButton,
		preferences->getShowError(LDLEVertexOrder));
//	preferences->setButtonState(panel->concaveQuadSplitButton,
//		preferences->getShowError(LDLEConcaveQuadSplit));
	preferences->setButtonState(panel->colinearPointsButton,
		preferences->getShowError(LDLEColinear));
//	preferences->setButtonState(panel->openGLErrorButton,
//		preferences->getShowError(LDLEOpenGL));
	preferences->setButtonState(panel->BFCWarningButton,
		preferences->getShowError(LDLEBFCWarning));
	preferences->setButtonState(panel->BFCErrorButton,
		preferences->getShowError(LDLEBFCError));
	preferences->setButtonState(panel->MPDErrorButton,
		preferences->getShowError(LDLEMPDError));
	preferences->setButtonState(panel->whitespaceButton,
		preferences->getShowError(LDLEWhitespace));
	preferences->setButtonState(panel->partrenamedButton,
		preferences->getShowError(LDLEMovedTo));
	preferences->setButtonState(panel->unofficialPartButton,
		preferences->getShowError(LDLEUnofficialPart));
	preferences->setButtonState(panel->showWarningsButton,
		TCUserDefaults::longForKey(SHOW_WARNINGS_KEY, 0) ? 1 : 0);
	preferences->setButtonState(panel->identicalVerticesButton,
		preferences->getShowError(LDLEMatchingPoints));
	preferences->setButtonState(panel->nonFlatQuadButton,
		preferences->getShowError(LDLENonFlatQuad));
}

void LDViewErrors::setValues(bool value)
{
	for(int b=LDLEFirstError;b<=LDLELastError;b++)
	{
		preferences->setShowError(b,value);
	}
}

void LDViewErrors::doShowWarnings(void)
{
	 TCUserDefaults::setLongForKey(TCUserDefaults::longForKey(SHOW_WARNINGS_KEY, 0) ? 0 :1 ,
		SHOW_WARNINGS_KEY);
        clearListView();
        populateListView();
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
	int warningCount = 0;
	QString buf;
	
	if (!listViewPopulated)
	{
		int i;
		int count = errors->getCount();

		for (i = count - 1; i >= 0; i--)
		{
			LDLError *error = (*errors)[i];
			if (addErrorToListView((*errors)[i]))
			{
				if (error->getLevel() == LDLAWarning)
				{
					warningCount++;
				}
				else
				{
					errorCount++;
				}
			}
		}
		listViewPopulated = true;
    	if (errorCount > 0)
    	{
        	if (errorCount == 1)
        	{
            	buf = TCLocalStrings::get("ErrorTreeOneError");
        	}
        	else
        	{
            	buf = TCLocalStrings::get("ErrorTreeNErrors");
				buf.replace(QRegExp("%."), QString::number(errorCount));
        	}
        	if (warningCount > 0)
        	{
            	buf += ", ";
        	}
    	}
    	if (warningCount > 0)
    	{
        	if (warningCount == 1)
        	{
            	buf += TCLocalStrings::get("ErrorTreeOneWarning");
        	}
        	else
        	{
               	buf += TCLocalStrings::get("ErrorTreeNWarnings");
				buf.replace(QRegExp("%."), QString::number(warningCount));
        	}
    	}
		messageText->setText(buf);
	}
	return errorCount+warningCount;
}

bool LDViewErrors::addErrorToListView(LDLError *error)
{
	const char *string;
	QListViewItem *parent;
	QString buf, qstring;
	if (!TCUserDefaults::longForKey(SHOW_WARNINGS_KEY, 0) && 
		(error->getLevel() == LDLAWarning))
	{
		return false;
	}
	if (!showsErrorType(error->getType()))
	{
		return false;
	}
	wcstoqstring(qstring, error->getWMessage());
	parent = addErrorLine(NULL, qstring, error);
	if (parent)
	{
		string = error->getFilename();
		if (string)
		{
			buf = QString("File: ")+string;
		}
		else
		{
			buf = TCLocalStrings::get("ErrorTreeUnknownFile");
		}
		addErrorLine(parent, buf, error);
		string = error->getFileLine();
		if (string)
		{
			int lineNumber = error->getLineNumber();
			
			if (lineNumber > 0)
			{
				buf = TCLocalStrings::get("ErrorTreeLine#");
				buf.replace(QRegExp("%d"), QString::number(lineNumber));
			}
			else
			{
				buf = TCLocalStrings::get("ErrorTreeUnknownLine#");
			}
			addErrorLine(parent, buf, error);
			char *tempString = copyString(string);
			stripCRLF(tempString);
			buf = TCLocalStrings::get("ErrorTreeLine");
			buf.replace(QRegExp("%s"),QString(tempString));
			delete tempString;
		}
		else
		{
			buf = TCLocalStrings::get("ErrorTreeUnknownLine");
		}
		addErrorLine(parent, buf, error);
		ucstringVector extraInfo = error->getUCExtraInfo();
		if (extraInfo.size() > 0)
		{
			for (ucstringVector::iterator it = extraInfo.begin();
				 it != extraInfo.end(); it++)
			{
				QString tmp;
				ucstringtoqstring(tmp, *it);
				addErrorLine(parent, tmp, error);
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
	QString line, LDLError * error, int /*imageIndex*/)
{
	QListViewItem *item;

	if (parent)
	{
		if (parent->childCount() > 0)
		{
			QListViewItem *lastChild = parent->firstChild();

			while (lastChild->nextSibling() != NULL)
			{
				lastChild = lastChild->nextSibling();
			}
			item = new QListViewItem(parent, lastChild, line);
		}
		else
		{
			item = new QListViewItem(parent, line);
		}
		item->setPixmap(0,getimage( "error_info.png" ));
	}
	else
	{
		item = new QListViewItem(panel->errorListView, line);
        switch (error->getType())
        {
            case LDLEGeneral:
            case LDLEBFCError:
			case LDLEBFCWarning:
            case LDLEMPDError:
		    case LDLEWhitespace:
            case LDLEParse:
                    item->setPixmap(0,
                        getimage( "error_parse.png"));
                    break;
            case LDLEMatrix:
                    item->setPixmap(0,
                        getimage( "error_matrix.png"));
                    break;
            case LDLEFileNotFound:
                    item->setPixmap(0,
                        getimage( "error_fnf.png"));
                    break;
            case LDLEMatchingPoints:
                    item->setPixmap(0,
                        getimage( "error_matching_points.png"));
                    break;
            case LDLEConcaveQuad:
                    item->setPixmap(0,
                        getimage( "error_concave_quad.png"));
                    break;
            case LDLEColinear:
                    item->setPixmap(0,
                        getimage( "error_colinear.png"));
                    break;
            case LDLEVertexOrder:
                    item->setPixmap(0,
                        getimage( "error_vertex_order.png"));
                    break;
            case LDLENonFlatQuad:
                    item->setPixmap(0,
                        getimage( "error_non_flat_quad.png"));
                    break;
            case LDLEPartDeterminant:
                    item->setPixmap(0,
                        getimage( "error_determinant.png"));
                    break;
			case LDLEMovedTo:
			case LDLEUnofficialPart:
					item->setPixmap(0,
                        getimage( "error_info.png"));
					break;
        }
	}
	return item;
}

void LDViewErrors::doErrorClick(QCheckBox *button, LDLErrorType errorType)
{
	preferences->setShowError(errorType, button->state());
	clearListView();
	populateListView();
}

void LDViewErrors::doShowAllError()
{
	setValues(true);
	reflectSettings();
	clearListView();
	populateListView();
}

void LDViewErrors::doShowNoneError()
{
	setValues(false);
	reflectSettings();
	clearListView();
	populateListView();
}

