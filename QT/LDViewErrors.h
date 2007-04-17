#ifndef __LDVIEWERRORS_H__
#define __LDVIEWERRORS_H__

#include <TCFoundation/TCTypedObjectArray.h>
#include <LDLoader/LDLError.h>
#include "ErrorPanel.h"

class ErrorPanel;
class Preferences;
class QListViewItem;
class QButton;
class QStatusBar;
class QLabel;
class QCheckBox;

typedef TCTypedObjectArray<LDLError> LDLErrorArray;

class LDViewErrors
{
public:
	LDViewErrors(Preferences *preferences);
	~LDViewErrors(void);

	void show(void);
	void clear(void);
	void addError(LDLError *error);
	int populateListView(void);
	void doShowWarnings(void);
	void doShowAllError(void);
	void doShowNoneError(void);
	void reflectSettings(void);
	void setValues(bool);
	void doErrorClick(QCheckBox *button, LDLErrorType errorNumber);

protected:
	void clearListView(void);
	bool addErrorToListView(LDLError *error);
	bool showsErrorType(LDLErrorType errorType);
	QListViewItem *addErrorLine(QListViewItem *parent, QString line,
	LDLError *error, int imageIndex = -1);

	ErrorPanel *panel;
	Preferences *preferences;
	LDLErrorArray *errors;
	bool listViewPopulated;
	QStatusBar *statusBar;
	QLabel *messageText;
};

#endif // __LDVIEWERRORS_H__
