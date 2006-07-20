#ifndef __LDLMODEL_H__
#define __LDLMODEL_H__

#include <TCFoundation/TCObject.h>
#include <LDLoader/LDLFileLine.h>
#include <LDLoader/LDLError.h>
#include <stdio.h>
#include <stdarg.h>

class TCDictionary;
class LDLMainModel;
class LDLCommentLine;
class LDLModelLine;

typedef enum
{
	// WARNING: Increasing this list past 4 members requires more bits in flags.
	// Note that even now, it requires 3 bits, because the compiler treats it
	// as a signed integer.  When the value 2 is sign-extended, it becomes -2,
	// which doesn't match.
	BFCUnknownState,
	BFCOffState,
	BFCOnState,
	BFCForcedOnState
} BFCState;

typedef bool (*LDLFileCaseCallback)(char *filename);
struct LDrawIniS;

class LDLModel : public TCObject
{
public:
	LDLModel(void);
	LDLModel(const LDLModel &other);
	virtual TCObject *copy(void);
	// Color numbers might become dynamic, so not static.
	virtual void getRGBA(TCULong colorNumber, int& r, int& g, int& b, int& a);
	virtual TCULong getPackedRGBA(TCULong colorNumber);
	virtual TCULong getEdgeColorNumber(TCULong colorNumber);
	virtual bool hasSpecular(TCULong colorNumber);
	virtual bool hasShininess(TCULong colorNumber);
	virtual void getSpecular(TCULong colorNumber, float *specular);
	virtual void getShininess(TCULong colorNumber, float &shininess);
	virtual LDLModel *subModelNamed(const char *subModelName,
		bool lowRes = false, bool secondAttempt = false,
		const LDLModelLine *fileLine = NULL, bool knownPart = false);
	virtual const char *getFilename(void) { return m_filename; }
	virtual void setFilename(const char *filename);
	virtual const char *getName(void) { return m_name; }
	virtual void setName(const char *name);
	virtual bool load(FILE *file, bool trackProgress = true);
	virtual void print(int indent);
	virtual bool parse(void);
	virtual TCDictionary* getLoadedModels(void);
	virtual bool getLowResStuds(void) const;
	virtual LDLError *newError(LDLErrorType type, const LDLFileLine &fileLine,
		const char *format, va_list argPtr);
	virtual LDLError *newError(LDLErrorType type, const LDLFileLine &fileLine,
		const char *format, ...);
	virtual LDLError *newError(LDLErrorType type, const char *format,
		va_list argPtr);
	virtual LDLError *newError(LDLErrorType type, const char *format, ...);
	virtual LDLFileLineArray *getFileLines(void) { return m_fileLines; }
	virtual int getActiveLineCount(void) { return m_activeLineCount; }
	virtual bool colorNumberIsTransparent(TCULong colorNumber);
	virtual bool isMainModel(void) { return false; }

	// Flags
	// Note that bit flags can cause odd results; thus returning the != false,
	// instead of returning the flag value directly.
	bool isPart(void) { return m_flags.part != false; }
	bool isPrimitive(void) { return m_flags.primitive != false; }
	bool isMPD(void) { return m_flags.mpd != false; }
	bool getNoShrinkFlag(void) { return m_flags.noShrink != false; }

	BFCState getBFCState(void) { return m_flags.bfcCertify; }

	bool getBFCOn(void)
	{
		return m_flags.bfcCertify == BFCOnState ||
			m_flags.bfcCertify == BFCForcedOnState;
	}

	virtual void cancelLoad(void);
	virtual bool getLoadCanceled(void);
	LDLMainModel *getMainModel(void) { return m_mainModel; }

	static const char *lDrawDir(void);
	static void setLDrawDir(const char *value);
	static void setFileCaseCallback(LDLFileCaseCallback value);
	static LDLFileCaseCallback getFileCaseCallback(void)
	{
		return fileCaseCallback;
	}
	static FILE *openFile(const char *filename);
protected:
	virtual void dealloc(void);
	virtual FILE *openSubModelNamed(const char* subModelName,
		char* subModelPath, bool knownPart);
	virtual bool initializeNewSubModel(LDLModel* subModel,
		const char  *dictName, FILE* subModelFile = NULL);
	virtual bool read(FILE *file);
	virtual int parseComment(int index, LDLCommentLine *commentLine);
	virtual int parseMPDMeta(int index, const char *filename);
	virtual int parseBFCMeta(LDLCommentLine *commentLine);
	virtual void readComment(LDLCommentLine *commentLine);
	virtual void sendAlert(LDLError *alert);
	virtual void sendAlert(LDLErrorType type, LDLAlertLevel level,
		const char* format, va_list argPtr);
	virtual void sendAlert(LDLErrorType type, LDLAlertLevel level,
		const LDLFileLine &fileLine, const char* format, va_list argPtr);
	virtual void reportError(LDLErrorType type, const LDLFileLine &fileLine,
		const char* format, ...);
	virtual void reportWarning(LDLErrorType type, const LDLFileLine &fileLine,
		const char* format, ...);
	virtual void reportError(LDLErrorType type, const char* format, ...);
	virtual void reportWarning(LDLErrorType type, const char* format, ...);
	virtual void reportProgress(const char *message, float progress,
		bool mainOnly = true);
	virtual bool isSubPart(const char *subModelName);
	virtual bool isAbsolutePath(const char *path);
//	virtual void processModelLine(LDLModelLine *modelLine);
	virtual FILE *openModelFile(const char *filename, bool knownPart = false);

	static bool verifyLDrawDir(const char *value);

	char *m_filename;
	char *m_name;
	LDLFileLineArray *m_fileLines;
	LDLMainModel *m_mainModel;
	int m_activeLineCount;
	LDLModel *m_activeMPDModel;
	struct
	{
		// Private flags
		bool loadingPart:1;			// Temporal
		bool loadingSubPart:1;		// Temporal
		bool loadingPrimitive:1;	// Temporal
		bool mainModelLoaded:1;		// Temporal
		bool mainModelParsed:1;		// Temporal
		bool started:1;				// Temporal
		bool bfcClip:1;				// Temporal
		bool bfcWindingCCW:1;		// Temporal
		bool bfcInvertNext:1;		// Temporal
		// Public flags
		bool part:1;
		bool subPart:1;
		bool primitive:1;
		bool mpd:1;
		bool noShrink:1;
		BFCState bfcCertify:3;
	} m_flags;

	static char *sm_systemLDrawDir;
	static LDrawIniS *sm_lDrawIni;
	static int sm_modelCount;
	static LDLFileCaseCallback fileCaseCallback;
	static class LDLModelCleanup
	{
	public:
		~LDLModelCleanup(void);
	} sm_cleanup;
	friend class LDLModelCleanup;
};

#endif // __LDLMODEL_H__
