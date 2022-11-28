#ifndef __LDVIEWERRORS_H__
#define __LDVIEWERRORS_H__

#include <TCFoundation/TCTypedObjectArray.h>
#include <LDLoader/LDLError.h>
#include "ui_ErrorPanel.h"

class Preferences;

typedef TCTypedObjectArray<LDLError> LDLErrorArray;

class LDViewErrors : public QMainWindow , Ui::ErrorPanel
{
	Q_OBJECT
public:
	LDViewErrors(QWidget *parent,Preferences *preferences);
	~LDViewErrors(void);

	void clear(void);
	void addError(LDLError *error);
	int populateListView(void);
	void reflectSettings(void);
	void setValues(bool);
	void doErrorClick(QCheckBox *button, LDLErrorType errorNumber);

public slots:
	void generalError() {doErrorClick(generalErrorButton, LDLEGeneral);}
	void parseError()   {doErrorClick(parseErrorButton, LDLEParse);}
	void fileNotFound() {doErrorClick(fileNotFoundButton, LDLEFileNotFound);}
	void singularMatrix(){doErrorClick(singularMatrixButton, LDLEMatrix);}
	void partDeterminant(){doErrorClick(partDeterminantButton, LDLEPartDeterminant);}
	void concaveQuad()  {doErrorClick(concaveQuadButton, LDLEConcaveQuad);}
	void badVertexOrder(){doErrorClick(badVertexOrderButton, LDLEVertexOrder);}
	void colinearPoints(){doErrorClick(colinearPointsButton, LDLEColinear);}
	void identicalVertices(){doErrorClick(identicalVerticesButton,LDLEMatchingPoints);}
	void modelLoop()    {doErrorClick(modelLoopButton, LDLEModelLoop);}
	void metaCommand()  {doErrorClick(metaCommandProblemButton, LDLEMetaCommand);}
	void BFCWarning()   {doErrorClick(BFCWarningButton, LDLEBFCWarning);}
	void BFCError()     {doErrorClick(BFCErrorButton, LDLEBFCError);}
	void nonFlatQuad()  {doErrorClick(nonFlatQuadButton, LDLENonFlatQuad);}
	void MPDError()     {doErrorClick(MPDErrorButton, LDLEMPDError);}
	void whitespace()   {doErrorClick(whitespaceButton, LDLEWhitespace);}
	void partrenamed()  {doErrorClick(partrenamedButton, LDLEMovedTo);}
	void unofficialpart(){doErrorClick(unofficialPartButton, LDLEUnofficialPart);}
	
	void showWarnings(void);
	void showAllError(void);
	void showNoneError(void);

protected:
	void clearListView(void);
	bool addErrorToListView(LDLError *error);
	bool showsErrorType(LDLErrorType errorType);
	QTreeWidgetItem *addErrorLine(QTreeWidgetItem *parent, QString line,
	LDLError *error, int imageIndex = -1);

	Preferences *preferences;
	LDLErrorArray *errors;
	bool listViewPopulated;
	QLabel *messageText;
};

#endif // __LDVIEWERRORS_H__
