#ifndef __LDLMODEL_H__
#define __LDLMODEL_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedObjectArray.h>
#include <LDLoader/LDLFileLine.h>
#include <LDLoader/LDLError.h>
#include <stdio.h>
#include <stdarg.h>

typedef TCTypedObjectArray<LDLFileLine> LDLFileLineArray;

class TCDictionary;
class LDLMainModel;
class LDLCommentLine;
class LDLModelLine;

typedef enum
{
	// WARNING: Increasing this list past 4 members requires more bits in flags.
	BFCUnknownState,
	BFCOffState,
	BFCOnState
} BFCState;

class LDLModel : public TCObject
{
public:
	LDLModel(void);
	LDLModel(const LDLModel &other);
	virtual TCObject *copy(void);
	// Color numbers might become dynamic, so not static.
	virtual void getRGBA(int colorNumber, int& r, int& g, int& b, int& a);
	virtual LDLModel *subModelNamed(const char *subModelName,
		bool lowRes = false);
	virtual const char *getFilename(void) { return m_filename; }
	virtual void setFilename(const char *filename);
	virtual const char *getName(void) { return m_name; }
	virtual void setName(const char *name);
	virtual bool load(FILE *file);
	virtual void print(int indent = 0);
	virtual bool parse(void);
	virtual TCDictionary* getLoadedModels(void);
	virtual bool getLowResStuds(void) const;
	virtual LDLError *newError(LDLErrorType type, const LDLFileLine &fileLine,
		const char *format, va_list argPtr);
	virtual LDLError *newError(LDLErrorType type, const LDLFileLine &fileLine,
		const char *format, ...);

	// Flags
	bool isPart(void) { return m_flags.part; }

	// Transparency detection is fixed, so static.
	static bool colorNumberIsTransparent(int);
	static TCULong colorForRGBA(int r, int g, int b, int a);
	static const char *lDrawDir(void);
protected:
	virtual void dealloc(void);
	virtual FILE *openSubModelNamed(const char* subModelName,
		char* subModelPath);
	virtual bool initializeNewSubModel(LDLModel* subModel,
		const char  *dictName, FILE* subModelFile = NULL);
	virtual bool read(FILE *file);
	virtual int parseComment(int index, LDLCommentLine *commentLine);
	virtual void readComment(LDLCommentLine *commentLine);
	virtual void reportError(LDLError *error);
	virtual void reportError(LDLErrorType type, const LDLFileLine &fileLine,
		const char* format, ...);
	virtual void processModelLine(LDLModelLine *modelLine);

	char *m_filename;
	char *m_name;
	LDLFileLineArray *m_fileLines;
	LDLMainModel *m_mainModel;
	struct
	{
		// Private flags
		bool loadingPart:1;
		bool mainModelLoaded:1;
		bool mainModelParsed:1;
		bool started:1;
		bool bfcClip:1;
		bool bfcWindingCCW:1;
		bool bfcInvertNext:1;
		// Public flags
		bool part:1;
		BFCState bfcCertify:2;
	} m_flags;

	static char *sm_systemLDrawDir;
};

#endif // __LDLMODEL_H__
