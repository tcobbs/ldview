#ifndef __LDVIEWERRORS_H__
#define __LDVIEWERRORS_H__

#include <TCFoundation/TCTypedObjectArray.h>
#include <LDLib/LDMError.h>

class ErrorPanel;
class Preferences;
class QListViewItem;
class QButton;

typedef TCTypedObjectArray<LDMError> LDMErrorArray;

class LDViewErrors
{
public:
	LDViewErrors(Preferences *preferences);
	~LDViewErrors(void);

	void show(void);
	void clear(void);
	void addError(LDMError *error);
	int populateListView(void);
	
	void doErrorClick(QButton *button, LDMErrorType errorNumber);

protected:
	void clearListView(void);
	bool addErrorToListView(LDMError *error);
	bool showsErrorType(LDMErrorType errorType);
	QListViewItem *addErrorLine(QListViewItem *parent, const char *line,
	LDMError *error, int imageIndex = -1);

	ErrorPanel *panel;
	Preferences *preferences;
	LDMErrorArray *errors;
	bool listViewPopulated;
};

#endif // __LDVIEWERRORS_H__
