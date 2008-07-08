#ifndef __LDLMODEL_H__
#define __LDLMODEL_H__

#include <TCFoundation/TCAlertSender.h>
#include <TCFoundation/TCVector.h>
#include <TCFoundation/TCStlIncludes.h>
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

typedef std::list<int> IntList;
typedef std::list<std::string> StringList;

typedef bool (*LDLFileCaseCallback)(char *filename);
typedef void (TCObject::*LDLScanPointCallback)(const TCVector &point,
	const LDLFileLine *pFileLine);
struct LDrawIniS;

class LDLModel : public TCAlertSender
{
public:
	LDLModel(void);
	LDLModel(const LDLModel &other);
	virtual TCObject *copy(void) const;
	// Color numbers might become dynamic, so not static.
	virtual void getRGBA(int colorNumber, int& r, int& g, int& b, int& a);
	virtual TCULong getPackedRGBA(int colorNumber);
	virtual int getEdgeColorNumber(int colorNumber);
	virtual bool hasSpecular(int colorNumber);
	virtual bool hasShininess(int colorNumber);
	virtual void getSpecular(int colorNumber, float *specular);
	virtual void getShininess(int colorNumber, float &shininess);
	virtual LDLModel *subModelNamed(const char *subModelName,
		bool lowRes = false, bool secondAttempt = false,
		const LDLModelLine *fileLine = NULL, bool knownPart = false);
	virtual const char *getFilename(void) const { return m_filename; }
	virtual void setFilename(const char *filename);
	virtual const char *getName(void) const { return m_name; }
	virtual const char *getDescription(void) const { return m_description; }
	virtual const char *getAuthor(void) const { return m_author; }
	virtual void setName(const char *name);
	virtual bool load(FILE *file, bool trackProgress = true);
	virtual void print(int indent);
	virtual bool parse(void);
	virtual TCDictionary* getLoadedModels(void);
	virtual bool getLowResStuds(void) const;
	virtual LDLError *newError(LDLErrorType type, const LDLFileLine &fileLine,
		CUCSTR format, va_list argPtr);
	virtual LDLError *newError(LDLErrorType type, const LDLFileLine &fileLine,
		CUCSTR format, ...);
	virtual LDLError *newError(LDLErrorType type, CUCSTR format,
		va_list argPtr);
	virtual LDLError *newError(LDLErrorType type, CUCSTR format, ...);
	virtual LDLFileLineArray *getFileLines(void) { return m_fileLines; }
	virtual const IntList &getStepIndices(void) const { return m_stepIndices; }
	virtual int getActiveLineCount(void) const { return m_activeLineCount; }
	virtual bool colorNumberIsTransparent(int colorNumber);
	virtual bool isMainModel(void) const { return false; }
	virtual void scanPoints(TCObject *scanner,
		LDLScanPointCallback scanPointCallback, const TCFloat *matrix,
		int step = -1) const;
	virtual void getBoundingBox(TCVector &min, TCVector &max);
	virtual TCFloat getMaxRadius(const TCVector &center);

	// Flags
	// Note that bit flags can cause odd results; thus returning the != false,
	// instead of returning the flag value directly.
	bool isPart(void) const { return m_flags.part != false; }
	bool isPrimitive(void) const { return m_flags.primitive != false; }
	bool isSubPart(void) const { return m_flags.subPart != false; }
	bool isMPD(void) const { return m_flags.mpd != false; }
	bool getNoShrinkFlag(void) const { return m_flags.noShrink != false; }
	bool isOfficial(void) const { return m_flags.official != false; }
	bool hasStuds(void) const { return m_flags.hasStuds != false; }
	bool hasBoundingBox(void) const;
	//bool hasBoundingBox(void) const { return m_flags.haveBoundingBox != false; }

	BFCState getBFCState(void) { return m_flags.bfcCertify; }

	bool getBFCOn(void)
	{
		return m_flags.bfcCertify == BFCOnState ||
			m_flags.bfcCertify == BFCForcedOnState;
	}

	virtual void cancelLoad(void);
	virtual bool getLoadCanceled(void);
	LDLMainModel *getMainModel(void) { return m_mainModel; }
	virtual TCObject *getAlertSender(void);

	static const char *lDrawDir(bool defaultValue = false);
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
		const char *dictName, FILE* subModelFile = NULL);
	virtual bool read(FILE *file);
	virtual int parseComment(int index, LDLCommentLine *commentLine);
	virtual int parseMPDMeta(int index, const char *filename);
	virtual int parseBFCMeta(LDLCommentLine *commentLine);
	virtual void readComment(LDLCommentLine *commentLine);
	virtual void sendAlert(LDLError *alert);
	virtual void sendAlert(LDLErrorType type, LDLAlertLevel level,
		CUCSTR format, va_list argPtr);
	virtual void sendAlert(LDLErrorType type, LDLAlertLevel level,
		const LDLFileLine &fileLine, CUCSTR format, va_list argPtr);
	virtual void reportError(LDLErrorType type, const LDLFileLine &fileLine,
		CUCSTR format, ...);
	virtual void reportWarning(LDLErrorType type, const LDLFileLine &fileLine,
		CUCSTR format, ...);
	virtual void reportError(LDLErrorType type, CUCSTR format, ...);
	virtual void reportWarning(LDLErrorType type, CUCSTR format, ...);
	virtual void reportProgress(const char *message, float progress,
		bool mainOnly = true);
	virtual void reportProgress(const wchar_t *message, float progress,
		bool mainOnly = true);
	virtual bool isSubPart(const char *subModelName);
	virtual bool isAbsolutePath(const char *path);
//	virtual void processModelLine(LDLModelLine *modelLine);
	virtual FILE *openModelFile(const char *filename, bool knownPart = false);
	virtual void calcBoundingBox(void);
	virtual void calcMaxRadius(const TCVector &center);
	void scanBoundingBoxPoint(const TCVector &point, LDLFileLine *pFileLine);
	void scanRadiusPoint(const TCVector &point, LDLFileLine *pFileLine);

	static bool verifyLDrawDir(const char *value);
	static void initCheckDirs();

	char *m_filename;
	char *m_name;
	char *m_author;
	char *m_description;
	LDLFileLineArray *m_fileLines;
	LDLMainModel *m_mainModel;
	IntList m_stepIndices;
	int m_activeLineCount;
	LDLModel *m_activeMPDModel;

	TCVector m_boundingMin;
	TCVector m_boundingMax;
	TCVector m_center;
	TCFloat m_maxRadius;
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
		bool haveBoundingBox:1;		// Temporal
		bool haveMaxRadius:1;		// Temporal
		// Public flags
		bool part:1;
		bool subPart:1;
		bool primitive:1;
		bool mpd:1;
		bool noShrink:1;
		bool official:1;
		bool hasStuds:1;
		BFCState bfcCertify:3;
	} m_flags;

	static StringList sm_checkDirs;
	static char *sm_systemLDrawDir;
	static char *sm_defaultLDrawDir;
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
