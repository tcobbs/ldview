#ifndef __LDVIEWERRORS_H__
#define __LDVIEWERRORS_H__

#include <TCFoundation/TCTypedObjectArray.h>
#include <LDLoader/LDLError.h>

class ErrorPanel;
class Preferences;
class QListViewItem;
class QButton;

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
	
	void doErrorClick(QButton *button, LDLErrorType errorNumber);

protected:
	void clearListView(void);
	bool addErrorToListView(LDLError *error);
	bool showsErrorType(LDLErrorType errorType);
	QListViewItem *addErrorLine(QListViewItem *parent, const char *line,
	LDLError *error, int imageIndex = -1);

	ErrorPanel *panel;
	Preferences *preferences;
	LDLErrorArray *errors;
	bool listViewPopulated;
};

#endif // __LDVIEWERRORS_H__
