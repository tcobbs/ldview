#ifndef __UNMIRRORSTUDS_H__
#define __UNMIRRORSTUDS_H__

#include <LDLoader/LDLModel.h>
#include <TCFoundation/TCObject.h>

class LDLMainModel;
class LDLModelLine;
class LDLError;

class UnMirrorStuds: public TCObject
{
public:
	UnMirrorStuds(void);
	int run(int argc, char *argv[]);
protected:
	virtual ~UnMirrorStuds(void);
	virtual void dealloc(void);

	bool processModel(LDLModel *model, float *matrix);
	bool modelIsStud(LDLModel *model);
	bool checkModel(LDLModel *model, float *matrix,
		LDLFileLineArray *mirroredLines);
	void mirrorModel(LDLModel *model, float *matrix,
		LDLFileLineArray *mirroredLines);
	void mirrorStud(FILE *file, LDLModelLine *modelLine);
	void mirrorFilename(char *newFilename, const char *oldFilename,
		float *matrix);
	bool chDirFromFilename(const char* filename, char* outFilename,
		unsigned maxLength);
	bool checkOption(const char *option);
	void ldlErrorCallback(LDLError *error);
	void printUsage(void);
	void cleanFilename(char *filename);

	// flags
	bool m_delete;
	bool m_overwrite;
	bool m_verbose;
	// other stuff
	bool m_abort;
	bool m_optionsDone;
	bool m_fileProcessed;
	int m_deleteCount;
	int m_newFileCount;
};

#endif // __UNMIRRORSTUDS_H__
