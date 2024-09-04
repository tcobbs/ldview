#include <QtGlobal>
#include <QRadioButton>
#include "Preferences.h"

#include "LDViewErrors.h"

#include <LDLoader/LDLError.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCUserDefaults.h>
#include <LDLib/LDUserDefaultsKeys.h>

#include <QLabel>
#include <QStatusBar>
#if QT_VERSION < 0x60000
#include <QRegExp>
#endif
#include <QTreeWidgetItem>
#include <QHeaderView>
#include "misc.h"

LDViewErrors::LDViewErrors(QWidget *parent, Preferences *preferences)
	:QMainWindow(parent),ErrorPanel(),
	preferences(preferences),
	errors(new LDLErrorArray),
	listViewPopulated(false)
{
	setupUi(this);
	connect( generalErrorButton, SIGNAL( clicked() ), this, SLOT( generalError() ) );
	connect( parseErrorButton, SIGNAL( clicked() ), this, SLOT( parseError() ) );
	connect( fileNotFoundButton, SIGNAL( clicked() ), this, SLOT( fileNotFound() ) );
	connect( singularMatrixButton, SIGNAL( clicked() ), this, SLOT( singularMatrix() ) );
	connect( partDeterminantButton, SIGNAL( clicked() ), this, SLOT( partDeterminant() ) );
	connect( concaveQuadButton, SIGNAL( clicked() ), this, SLOT( concaveQuad() ) );
	connect( badVertexOrderButton, SIGNAL( clicked() ), this, SLOT( badVertexOrder() ) );
	connect( colinearPointsButton, SIGNAL( clicked() ), this, SLOT( colinearPoints() ) );
	connect( MPDErrorButton, SIGNAL( clicked() ), this, SLOT( MPDError() ) );
	connect( whitespaceButton, SIGNAL( clicked() ), this, SLOT( whitespace() ) );
	connect( partrenamedButton, SIGNAL( clicked() ), this, SLOT( partrenamed() ) );
	connect( unofficialPartButton, SIGNAL( clicked() ), this, SLOT( unofficialpart() ) );
	connect( modelLoopButton, SIGNAL( clicked() ), this, SLOT( modelLoop() ) );
	connect( metaCommandProblemButton, SIGNAL( clicked() ), this, SLOT( metaCommand() ) );
	connect( tooManyRequestsButton, SIGNAL( clicked() ), this, SLOT( toomanyrequests() ) );
	connect( showWarningsButton, SIGNAL( clicked() ), this, SLOT( showWarnings() ) );
	connect( identicalVerticesButton, SIGNAL( clicked() ), this, SLOT( identicalVertices() ) );
	connect( BFCErrorButton, SIGNAL( clicked() ), this, SLOT( BFCError() ) );
	connect( BFCWarningButton, SIGNAL( clicked() ), this, SLOT( BFCWarning() ) );
	connect( showAllButton, SIGNAL( clicked() ), this, SLOT( showAllError() ) );
	connect( showNoneButton, SIGNAL( clicked() ), this, SLOT( showNoneError() ) );
	connect( nonFlatQuadButton, SIGNAL( clicked() ), this, SLOT( nonFlatQuad() ) );

//	errorListView->setColumnWidthMode(0, QListView::Maximum);
	errorListView->header()->hide();
//	errorListView->setSorting(-1);
	reflectSettings();
	messageText = new QLabel(statusBar());
	statusBar()->addWidget(messageText,1);
}

void LDViewErrors::reflectSettings(void)
{
	preferences->setButtonState(generalErrorButton,
		preferences->getShowError(LDLEGeneral));
		preferences->setButtonState(parseErrorButton,
		preferences->getShowError(LDLEParse));
	preferences->setButtonState(fileNotFoundButton,
		preferences->getShowError(LDLEFileNotFound));
//	preferences->setButtonState(colorErrorButton,
//		preferences->getShowError(LDLEColor));
	preferences->setButtonState(singularMatrixButton,
		preferences->getShowError(LDLEMatrix));
	preferences->setButtonState(partDeterminantButton,
		preferences->getShowError(LDLEPartDeterminant));
	preferences->setButtonState(concaveQuadButton,
		preferences->getShowError(LDLEConcaveQuad));
	preferences->setButtonState(badVertexOrderButton,
		preferences->getShowError(LDLEVertexOrder));
//	preferences->setButtonState(concaveQuadSplitButton,
//		preferences->getShowError(LDLEConcaveQuadSplit));
	preferences->setButtonState(colinearPointsButton,
		preferences->getShowError(LDLEColinear));
//	preferences->setButtonState(openGLErrorButton,
//		preferences->getShowError(LDLEOpenGL));
	preferences->setButtonState(BFCWarningButton,
		preferences->getShowError(LDLEBFCWarning));
	preferences->setButtonState(BFCErrorButton,
		preferences->getShowError(LDLEBFCError));
	preferences->setButtonState(MPDErrorButton,
		preferences->getShowError(LDLEMPDError));
	preferences->setButtonState(whitespaceButton,
		preferences->getShowError(LDLEWhitespace));
	preferences->setButtonState(partrenamedButton,
		preferences->getShowError(LDLEMovedTo));
	preferences->setButtonState(unofficialPartButton,
		preferences->getShowError(LDLEUnofficialPart));
	preferences->setButtonState(modelLoopButton,
		preferences->getShowError(LDLEModelLoop));
	preferences->setButtonState(metaCommandProblemButton,
		preferences->getShowError(LDLEMetaCommand));
	preferences->setButtonState(tooManyRequestsButton,
		preferences->getShowError(LDLETooManyRequests));
	preferences->setButtonState(showWarningsButton,
		TCUserDefaults::longForKey(SHOW_WARNINGS_KEY, 0) ? 1 : 0);
	preferences->setButtonState(identicalVerticesButton,
		preferences->getShowError(LDLEMatchingPoints));
	preferences->setButtonState(nonFlatQuadButton,
		preferences->getShowError(LDLENonFlatQuad));
}

void LDViewErrors::setValues(bool value)
{
	for(int b=LDLEFirstError;b<=LDLELastError;b++)
	{
		preferences->setShowError(b,value);
	}
}

void LDViewErrors::showWarnings(void)
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
}

void LDViewErrors::clear(void)
{
	errors->removeAll();
	clearListView();
}

void LDViewErrors::clearListView(void)
{
	errorListView->clear();
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
				buf = QString::fromWCharArray(TCLocalStrings::get(L"ErrorTreeOneError"));
			}
			else
			{
				buf = QString::fromWCharArray(TCLocalStrings::get(L"ErrorTreeNErrors"));
#if QT_VERSION >= 0x60000
				buf.replace(QRegularExpression("%."), QString::number(errorCount));
#else
				buf.replace(QRegExp("%."), QString::number(errorCount));
#endif
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
				buf += QString::fromWCharArray(TCLocalStrings::get(L"ErrorTreeOneWarning"));
			}
			else
			{
				buf += QString::fromWCharArray(TCLocalStrings::get(L"ErrorTreeNWarnings"));
#if QT_VERSION >= 0x60000
				buf.replace(QRegularExpression("%."), QString::number(warningCount));
#else
				buf.replace(QRegExp("%."), QString::number(warningCount));
#endif
			}
		}
		messageText->setText(buf);
	}
	return errorCount;
}

bool LDViewErrors::addErrorToListView(LDLError *error)
{
	const char *string;
	QTreeWidgetItem *parent;
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
			buf = QString::fromWCharArray(TCLocalStrings::get(L"ErrorTreeUnknownFile"));
		}
		addErrorLine(parent, buf, error);
		string = error->getFormattedFileLine();
		if (string)
		{
			int lineNumber = error->getLineNumber();
			
			if (lineNumber > 0)
			{
				buf = QString::fromWCharArray(TCLocalStrings::get(L"ErrorTreeLine#"));
#if QT_VERSION >= 0x60000
				buf.replace(QRegularExpression("%d"), QString::number(lineNumber));
#else
				buf.replace(QRegExp("%d"), QString::number(lineNumber));
#endif
			}
			else
			{
				buf = QString::fromWCharArray(TCLocalStrings::get(L"ErrorTreeUnknownLine#"));
			}
			addErrorLine(parent, buf, error);
			char *tempString = copyString(string);
			stripCRLF(tempString);
			buf = QString::fromWCharArray(TCLocalStrings::get(L"ErrorTreeLine"));
#if QT_VERSION >= 0x60000
			buf.replace(QRegularExpression("%s"),QString(tempString));
#else
			buf.replace(QRegExp("%s"),QString(tempString));
#endif
			delete tempString;
		}
		else
		{
			buf = QString::fromWCharArray(TCLocalStrings::get(L"ErrorTreeUnknownLine"));
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

QTreeWidgetItem *LDViewErrors::addErrorLine(QTreeWidgetItem *parent,
	QString line, LDLError * error, int /*imageIndex*/)
{
	QTreeWidgetItem *item;

	if (parent)
	{
		if (parent->childCount() > 0)
		{
			QTreeWidgetItem *lastChild = parent->child(parent->childCount()-1);
			item = new QTreeWidgetItem(parent, lastChild);
		}
		else
		{
			item = new QTreeWidgetItem(parent);
		}
		item->setText(0, line);
		item->setIcon(0, QIcon( ":/images/Icons/error_info.png" ));
	}
	else
	{
		item = new QTreeWidgetItem(errorListView);
		item->setText(0, line);
		switch (error->getType())
		{
			case LDLETooManyRequests:
			case LDLEGeneral:
			case LDLEBFCError:
			case LDLEBFCWarning:
			case LDLEMPDError:
			case LDLEWhitespace:
			case LDLEMetaCommand:
			case LDLEParse:
					item->setIcon(0,
						QIcon( ":/images/Icons/error_parse.png"));
					break;
			case LDLEMatrix:
					item->setIcon(0,
						QIcon( ":/images/Icons/error_matrix.png"));
					break;
			case LDLEFileNotFound:
					item->setIcon(0,
						QIcon( ":/images/Icons/error_fnf.png"));
					break;
			case LDLEMatchingPoints:
					item->setIcon(0,
						QIcon( ":/images/Icons/error_matching_points.png"));
					break;
			case LDLEConcaveQuad:
					item->setIcon(0,
						QIcon( ":/images/Icons/error_concave_quad.png"));
					break;
			case LDLEColinear:
					item->setIcon(0,
						QIcon( ":/images/Icons/error_colinear.png"));
					break;
			case LDLEVertexOrder:
					item->setIcon(0,
						QIcon( ":/images/Icons/error_vertex_order.png"));
					break;
			case LDLENonFlatQuad:
					item->setIcon(0,
						QIcon( ":/images/Icons/error_non_flat_quad.png"));
					break;
			case LDLEPartDeterminant:
					item->setIcon(0,
						QIcon( ":/images/Icons/error_determinant.png"));
					break;
			case LDLEMovedTo:
			case LDLEUnofficialPart:
					item->setIcon(0,
						QIcon( ":/images/Icons/error_info.png"));
					break;
			case LDLEModelLoop:
					item->setIcon(0,
						QIcon( ":/images/Icons/error_loop.png"));
					break;
		}
	}
	return item;
}

void LDViewErrors::doErrorClick(QCheckBox *button, LDLErrorType errorType)
{
	preferences->setShowError(errorType, button->checkState());
	clearListView();
	populateListView();
}

void LDViewErrors::showAllError()
{
	setValues(true);
	reflectSettings();
	clearListView();
	populateListView();
}

void LDViewErrors::showNoneError()
{
	setValues(false);
	reflectSettings();
	clearListView();
	populateListView();
}

