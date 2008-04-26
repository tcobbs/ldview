#include "LDLModel.h"
#include "LDLMainModel.h"
#include "LDLCommentLine.h"
#include "LDLModelLine.h"
#include "LDLFindFileAlert.h"
#include "LDrawIni.h"
#include <TCFoundation/TCDictionary.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCProgressAlert.h>
#include <TCFoundation/TCLocalStrings.h>
#include <TCFoundation/TCUserDefaults.h>

#ifdef WIN32
#include <direct.h>

#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG

#else // WIN32
#include <unistd.h>
#endif // WIN32

#define LDL_LOWRES_PREFIX "LDL-LOWRES:"
#define LOAD_MESSAGE TCLocalStrings::get(_UC("LDLModelLoading"))
#define MAIN_READ_FRACTION 0.1f

char *LDLModel::sm_systemLDrawDir = NULL;
char *LDLModel::sm_defaultLDrawDir = NULL;
LDrawIniS *LDLModel::sm_lDrawIni = NULL;
int LDLModel::sm_modelCount = 0;
LDLFileCaseCallback LDLModel::fileCaseCallback = NULL;
LDLModel::LDLModelCleanup LDLModel::sm_cleanup;
StringList LDLModel::sm_checkDirs;

LDLModel::LDLModelCleanup::~LDLModelCleanup(void)
{
	delete LDLModel::sm_systemLDrawDir;
	delete LDLModel::sm_defaultLDrawDir;
	LDLModel::sm_systemLDrawDir = NULL;
	if (LDLModel::sm_lDrawIni)
	{
		LDrawIniFree(LDLModel::sm_lDrawIni);
	}
}


LDLModel::LDLModel(void)
	:m_filename(NULL),
	m_name(NULL),
	m_author(NULL),
	m_description(NULL),
	m_fileLines(NULL),
	m_mainModel(NULL),
	m_activeLineCount(0),
	m_activeMPDModel(NULL)
{
	// Initialize Private flags
	m_flags.loadingPart = false;
	m_flags.loadingSubPart = false;
	m_flags.loadingPrimitive = false;
	m_flags.mainModelLoaded = false;
	m_flags.mainModelParsed = false;
	m_flags.started = false;
	m_flags.bfcClip = false;
	m_flags.bfcWindingCCW = true;
	m_flags.bfcInvertNext = false;
	m_flags.haveBoundingBox = false;
	m_flags.haveMaxRadius = false;
	// Initialize Public flags
	m_flags.part = false;
	m_flags.subPart = false;
	m_flags.primitive = false;
	m_flags.mpd = false;
	m_flags.official = false;
	m_flags.bfcCertify = BFCUnknownState;
	m_flags.noShrink = false;
	sm_modelCount++;
}

LDLModel::LDLModel(const LDLModel &other)
	:m_filename(copyString(other.m_filename)),
	m_name(copyString(other.m_name)),
	m_description(copyString(other.m_description)),
	m_fileLines(NULL),
	m_mainModel(other.m_mainModel),
	m_activeLineCount(other.m_activeLineCount),
	m_activeMPDModel(NULL),
	m_flags(other.m_flags)
{
	if (other.m_fileLines)
	{
		m_fileLines = (LDLFileLineArray *)other.m_fileLines->copy();
	}
}

void LDLModel::dealloc(void)
{
	delete m_filename;
	delete m_name;
	delete m_author;
	delete m_description;
	TCObject::release(m_fileLines);
	sm_modelCount--;
	TCObject::dealloc();
}

TCObject *LDLModel::copy(void)
{
	return new LDLModel(*this);
}

void LDLModel::setFilename(const char *filename)
{
	delete m_filename;
	m_filename = copyString(filename);
}

void LDLModel::setName(const char *name)
{
	delete m_name;
	m_name = copyString(name);
}

TCULong LDLModel::getPackedRGBA(TCULong colorNumber)
{
	int r, g, b, a;

	getRGBA(colorNumber, r, g, b, a);
	return r << 24 | g << 16 | b << 8 | a;
}

bool LDLModel::hasSpecular(TCULong colorNumber)
{
	return m_mainModel->hasSpecular(colorNumber);
}

bool LDLModel::hasShininess(TCULong colorNumber)
{
	return m_mainModel->hasShininess(colorNumber);
}

void LDLModel::getSpecular(TCULong colorNumber, float *specular)
{
	m_mainModel->getSpecular(colorNumber, specular);
}

void LDLModel::getShininess(TCULong colorNumber, float &shininess)
{
	m_mainModel->getShininess(colorNumber, shininess);
}

TCULong LDLModel::getEdgeColorNumber(TCULong colorNumber)
{
	return m_mainModel->getEdgeColorNumber(colorNumber);
}

void LDLModel::getRGBA(TCULong colorNumber, int& r, int& g, int& b, int& a)
{
	m_mainModel->getRGBA(colorNumber, r, g, b, a);
}

bool LDLModel::colorNumberIsTransparent(TCULong colorNumber)
{
	return m_mainModel->colorNumberIsTransparent(colorNumber);
}

LDLModel *LDLModel::subModelNamed(const char *subModelName, bool lowRes,
								  bool secondAttempt,
								  const LDLModelLine *fileLine, bool knownPart)
{
	TCDictionary* subModelDict = getLoadedModels();
	LDLModel* subModel;
	char *dictName = NULL;
	char *adjustedName;
	bool &ancestorCheck = m_mainModel->ancestorCheck(subModelName);

	if (ancestorCheck)
	{
		// Recursion: the model named subModelName is an ancestor of the current
		// model.  Loading it will result in an infinite loop and crash after
		// the stack overflows.
		return NULL;
	}
	else
	{
		ancestorCheck = true;
	}
	adjustedName = copyString(subModelName);
	replaceStringCharacter(adjustedName, '\\', '/');
	if (lowRes)
	{
		if (stringHasCaseInsensitivePrefix(adjustedName, "stud"))
		{
			adjustedName[3] = '2';
		}
		else
		{
			delete adjustedName;
			ancestorCheck = false;
			return NULL;
		}
		dictName = new char[strlen(LDL_LOWRES_PREFIX) + strlen(adjustedName) +
			1];
		sprintf(dictName, "%s%s", LDL_LOWRES_PREFIX, adjustedName);
	}
	else
	{
		dictName = copyString(adjustedName);
	}
	subModel = (LDLModel*)(subModelDict->objectForKey(dictName));
	if (!subModel)
	{
		FILE* subModelFile;
		char subModelPath[1024];

		if ((subModelFile = openSubModelNamed(adjustedName, subModelPath,
			knownPart))
			!= NULL)
		{
			replaceStringCharacter(subModelPath, '\\', '/');
			subModel = new LDLModel;
			subModel->setFilename(subModelPath);
			if (!initializeNewSubModel(subModel, dictName, subModelFile))
			{
				subModel = NULL;
			}
			m_flags.loadingPart = false;
			m_flags.loadingSubPart = false;
		}
	}
	delete adjustedName;
	delete dictName;
	if (!subModel && !secondAttempt)
	{
		LDLFindFileAlert *alert = new LDLFindFileAlert(subModelName);

		TCAlertManager::sendAlert(alert, this);
		if (alert->getFileFound())
		{
			subModel = subModelNamed(alert->getFilename(), lowRes, true,
				fileLine, alert->getPartFlag());
			if (subModel)
			{
				if (!isPart() && subModel->isPart())
				{
					UCCHAR szWarning[1024];
					UCSTR ucSubModelName = mbstoucstring(subModelName);

					sucprintf(szWarning, COUNT_OF(szWarning),
						TCLocalStrings::get(_UC("LDLModelUnofficialPart")),
						ucSubModelName);
					delete ucSubModelName;
					reportWarning(LDLEUnofficialPart, *fileLine, szWarning);
				}
			}
		}
		alert->release();
	}
	ancestorCheck = false;
	return subModel;
}

// NOTE: static function
FILE *LDLModel::openFile(const char *filename)
{
	FILE *modelFile = NULL;
	char *newFilename = copyString(filename);

	convertStringToLower(newFilename);
	if (fileCaseCallback)
	{
		// Use binary mode to work around problem with fseek on a non-binary
		// file.  The file parsing code will still work fine and strip out the
		// extra data.
		if ((modelFile = fopen(newFilename, "rb")) == NULL)
		{
			convertStringToUpper(newFilename);
			if ((modelFile = fopen(newFilename, "rb")) == NULL)
			{
				strcpy(newFilename, filename);
				if ((modelFile = fopen(filename, "rb")) == NULL)
				{
					if (fileCaseCallback(newFilename))
					{
						modelFile = fopen(newFilename, "rb");
					}
				}
			}
		}
	}
	else
	{
		modelFile = fopen(newFilename, "rb");
	}
	delete newFilename;
	return modelFile;
}

FILE *LDLModel::openModelFile(const char *filename, bool knownPart)
{
	FILE *modelFile = openFile(filename);
	if (modelFile && knownPart)
	{
		m_flags.loadingPart = true;
	}
	return modelFile;
}

bool LDLModel::isSubPart(const char *subModelName)
{
	return stringHasCaseInsensitivePrefix(subModelName, "s/") ||
		stringHasCaseInsensitivePrefix(subModelName, "s\\");
}

bool LDLModel::isAbsolutePath(const char *path)
{
#ifdef WIN32
	return strlen(path) >= 2 && path[1] == ':' || path[0] == '/';
#else
	return path[0] == '/';
#endif
}

FILE* LDLModel::openSubModelNamed(const char* subModelName, char* subModelPath,
								  bool knownPart)
{
	FILE* subModelFile;
	TCStringArray *extraSearchDirs = m_mainModel->getExtraSearchDirs();

	strcpy(subModelPath, subModelName);
	if (isAbsolutePath(subModelPath))
	{
		return openModelFile(subModelPath, knownPart);
	}
	else if (sm_lDrawIni && sm_lDrawIni->nSearchDirs > 0)
	{
		int i;

		for (i = 0; i < sm_lDrawIni->nSearchDirs; i++)
		{
			LDrawSearchDirS *searchDir = &sm_lDrawIni->SearchDirs[i];

			if ((searchDir->Flags & LDSDF_SKIP) == 0)
			{
				sprintf(subModelPath, "%s/%s", searchDir->Dir, subModelName);
				if ((subModelFile = openModelFile(subModelPath)) != NULL)
				{
					if (searchDir->Flags & LDSDF_DEFPRIM)
					{
						m_flags.loadingPrimitive = true;
					}
					else if (searchDir->Flags & LDSDF_DEFPART)
					{
						if (isSubPart(subModelName))
						{
							m_flags.loadingSubPart = true;
						}
						else
						{
							m_flags.loadingPart = true;
						}
					}
					return subModelFile;
				}
			}
		}
	}
	else
	{
		if ((subModelFile = openModelFile(subModelPath)) != NULL)
		{
			return subModelFile;
		}
		sprintf(subModelPath, "%s/P/%s", lDrawDir(), subModelName);
		if ((subModelFile = openModelFile(subModelPath)) != NULL)
		{
			m_flags.loadingPrimitive = true;
			return subModelFile;
		}
		sprintf(subModelPath, "%s/PARTS/%s", lDrawDir(), subModelName);
		if ((subModelFile = openModelFile(subModelPath)) != NULL)
		{
			if (isSubPart(subModelName))
			{
				m_flags.loadingSubPart = true;
			}
			else
			{
				m_flags.loadingPart = true;
			}
			return subModelFile;
		}
		sprintf(subModelPath, "%s/MODELS/%s", lDrawDir(), subModelName);
		if ((subModelFile = openModelFile(subModelPath)) != NULL)
		{
			return subModelFile;
		}
	}
	if (extraSearchDirs)
	{
		int i;
		int count = extraSearchDirs->getCount();

		for (i = 0; i < count; i++)
		{
			sprintf(subModelPath, "%s/%s", (*extraSearchDirs)[i], subModelName);
			if ((subModelFile = openModelFile(subModelPath)) != NULL)
			{
				return subModelFile;
			}
		}
	}
	return NULL;
}

bool LDLModel::initializeNewSubModel(LDLModel *subModel, const char *dictName,
									 FILE *subModelFile)
{
	TCDictionary* subModelDict = getLoadedModels();

	subModelDict->setObjectForKey(subModel, dictName);
	subModel->release();
	subModel->m_mainModel = m_mainModel;
	if (m_flags.loadingPart)
	{
		subModel->m_flags.part = true;
		subModel->m_flags.subPart = false;
//		subModel->m_flags.bfcCertify = BFCForcedOnState;
	}
	if (m_flags.loadingSubPart)
	{
		subModel->m_flags.part = false;
		subModel->m_flags.subPart = true;
	}
	if (m_flags.loadingPrimitive)
	{
		subModel->m_flags.primitive = true;
	}
	if (subModelFile && !subModel->load(subModelFile))
	{
		subModelDict->removeObjectForKey(dictName);
		return false;
	}
	subModel->setName(dictName);
	return true;
}

// NOTE: static function.
bool LDLModel::verifyLDrawDir(const char *value)
{
	char currentDir[1024];
	bool retValue = false;

	if (value && getcwd(currentDir, sizeof(currentDir)))
	{
		if (chdir(value) == 0)
		{
			if (chdir("parts") == 0 && chdir("..") == 0 && chdir("p") == 0)
			{
				retValue = true;
			}
		}
		chdir(currentDir);
	}
	return retValue;
}

// NOTE: static function.
void LDLModel::setFileCaseCallback(LDLFileCaseCallback value)
{
	fileCaseCallback = value;
	// If bool isn't 1 byte, then we can't support the case callback, so don't
	// even try.  This will trigger an error in LDLMainModel if the P and/or
	// PARTS directory inside the LDraw directory aren't capitalized.
	if (sizeof(bool) == sizeof(char))
	{
		if (sm_lDrawIni)
		{
			LDrawIniSetFileCaseCallback(sm_lDrawIni, fileCaseCallback);
		}
	}
}

// NOTE: static function.
void LDLModel::setLDrawDir(const char *value)
{
	if (value != sm_systemLDrawDir || !value)
	{
		delete sm_systemLDrawDir;
		if (value)
		{
			sm_systemLDrawDir = cleanedUpPath(value);
		}
		else
		{
			sm_systemLDrawDir = NULL;
		}
		if (sm_lDrawIni)
		{
			LDrawIniFree(sm_lDrawIni);
		}
		sm_lDrawIni = LDrawIniGet(sm_systemLDrawDir, NULL);
		if (sm_lDrawIni)
		{
			if (fileCaseCallback)
			{
				LDrawIniSetFileCaseCallback(sm_lDrawIni, fileCaseCallback);
			}
			if (!sm_systemLDrawDir)
			{
				sm_systemLDrawDir = copyString(sm_lDrawIni->LDrawDir);
			}
			if (sm_systemLDrawDir)
			{
				stripTrailingPathSeparators(sm_systemLDrawDir);
				LDrawIniComputeRealDirs(sm_lDrawIni, 1, 0, NULL);
			}
		}
	}
}

void LDLModel::initCheckDirs()
{
	const char *value = getenv("LDRAWDIR");

	if (value)
	{
		sm_checkDirs.push_back(value);
	}
#ifdef WIN32
	char buf[1024];
	
	if (GetPrivateProfileString("LDraw", "BaseDirectory", "", buf, 1024,
		"ldraw.ini"))
	{
		buf[1023] = 0;
	}
	if (buf[0])
	{
		sm_checkDirs.push_back(buf);
	}
	sm_checkDirs.push_back("C:\\ldraw");
#else // WIN32
#ifdef __APPLE__
	const char *libDir = "/Library/ldraw";
	const char *homeDir = getenv("HOME");

	if (homeDir != NULL)
	{
		char *homeLib = copyString(homeDir, strlen(libDir));
		
		stripTrailingPathSeparators(homeLib);
		strcat(homeLib, libDir);
		sm_checkDirs.push_back(homeLib);
		delete homeLib;
	}
	sm_checkDirs.push_back(libDir);
	sm_checkDirs.push_back("/Applications/Bricksmith/LDraw");
#else // __APPLE__
	sm_checkDirs.push_back("/usr/local/ldraw");
#endif // __APPLE__
#endif // WIN32
	char *ldviewDir = copyString(TCUserDefaults::getAppPath());
	stripTrailingPathSeparators(ldviewDir);
	char *ldviewLDrawDir = copyString(ldviewDir, 10);

	// LDView Dir/ldraw
	strcat(ldviewLDrawDir, "/ldraw");
	sm_checkDirs.push_back(ldviewLDrawDir);
#ifndef COCOA
	delete ldviewLDrawDir;
	char *ldviewParentDir = directoryFromPath(ldviewDir);
	stripTrailingPathSeparators(ldviewParentDir);
	ldviewLDrawDir = copyString(ldviewParentDir, 10);
	delete ldviewParentDir;
	// LDView Dir/../ldraw
	strcat(ldviewLDrawDir, "/ldraw");
	sm_checkDirs.push_back(ldviewLDrawDir);
#endif // COCOA
	delete ldviewDir;
	delete ldviewLDrawDir;
}

// NOTE: static function.
const char* LDLModel::lDrawDir(bool defaultValue /*= false*/)
{
	char *origValue = NULL;

	if (defaultValue)
	{
		if (sm_defaultLDrawDir)
		{
			return sm_defaultLDrawDir;
		}
		origValue = copyString(sm_systemLDrawDir);
		if (sm_systemLDrawDir)
		{
			setLDrawDir(NULL);
		}
	}
	if (!sm_systemLDrawDir)
	{
		bool found = false;

		if (sm_checkDirs.size() == 0)
		{
			initCheckDirs();
		}
		for (StringList::const_iterator it = sm_checkDirs.begin(); !found &&
			 it != sm_checkDirs.end(); it++)
		{
			const char *dir = it->c_str();

			if (verifyLDrawDir(dir))
			{
				setLDrawDir(dir);
				found = true;
			}
		}
		if (!found)
		{
			sm_systemLDrawDir = copyString("");
		}
	}
	if (defaultValue)
	{
		sm_defaultLDrawDir = copyString(sm_systemLDrawDir);
		setLDrawDir(origValue);
		delete origValue;
		return sm_defaultLDrawDir;
	}
	else
	{
		return sm_systemLDrawDir;
	}
}

void LDLModel::readComment(LDLCommentLine *commentLine)
{
	char buf[1024];

	if (commentLine->getMPDFilename(buf, sizeof(buf)))
	{
		replaceStringCharacter(buf, '\\', '/');
		if (m_flags.mainModelLoaded)
		{
			if (m_activeLineCount == 0)
			{
				m_activeLineCount = commentLine->getLineNumber() - 1;
			}
			if (!getLoadedModels()->objectForKey(buf))
			{
				LDLModel *subModel = new LDLModel;

				subModel->setFilename(m_filename);
				initializeNewSubModel(subModel, buf);
				m_activeMPDModel = subModel;
			}
		}
		else
		{
			m_flags.mainModelLoaded = true;
			m_flags.mpd = true;
		}
	}
	else if (commentLine->isPartMeta())
	{
		if (commentLine->isOfficialPartMeta(true))
		{
			// Note that even if we decide it's a primitive, it can still be an
			// official file.
			m_flags.official = true;
		}
		// No matter what the comment says, remember that a primitive cannot
		// be a part, so if we found the file in the P directory, then by
		// definition it isn't a part.
		if (m_flags.mainModelLoaded)
		{
			if (m_activeMPDModel && !m_activeMPDModel->isPrimitive())
			{
				if (m_flags.loadingSubPart)
				{
					m_activeMPDModel->m_flags.subPart = true;
				}
				else if (!m_activeMPDModel->m_flags.subPart)
				{
					m_activeMPDModel->m_flags.part = true;
				}
			}
		}
		else if (!isPrimitive())
		{
			if (m_flags.loadingSubPart)
			{
				m_flags.subPart = true;
			}
			else if (!m_flags.subPart)
			{
				m_flags.part = true;
			}
			// This is now a part, so if we've already hit a BFC CERTIFY line
			// switch it to forced certify.
			if (m_flags.bfcCertify == BFCOnState)
			{
				m_flags.bfcCertify = BFCForcedOnState;
			}
		}
	}
	else if (commentLine->isPrimitiveMeta())
	{
		if (m_flags.mainModelLoaded)
		{
			m_activeMPDModel->m_flags.part = false;
			m_activeMPDModel->m_flags.subPart = false;
			m_activeMPDModel->m_flags.primitive = true;
		}
		else
		{
			m_flags.part = false;
			m_flags.subPart = false;
			m_flags.primitive = true;
		}
	}
	else if (commentLine->isNoShrinkMeta())
	{
		if (m_flags.mainModelLoaded)
		{
			if (m_activeMPDModel)
			{
				m_activeMPDModel->m_flags.noShrink = true;
			}
		}
		else
		{
			m_flags.noShrink = true;
		}
	}
	else if (commentLine->getAuthor(buf, sizeof(buf)))
	{
		if (!m_author)
		{
			m_author = copyString(buf);
		}
	}
}

/*
static char *myFgets(char *buf, int bufSize, FILE *file)
{
	int i;

	for (i = 0; i < bufSize - 1; i++)
	{
		int char1 = fgetc(file);

		if (feof(file))
		{
			buf[i] = 0;
			if (i > 0)
			{
				return buf;
			}
			else
			{
				return NULL;
			}
		}
		if (char1 == '\r' || char1 == '\n')
		{
			int char2 = fgetc(file);

			buf[i] = '\n';
			buf[i + 1] = 0;
			if (!feof(file))
			{
				if (!(char1 == '\r' && char2 == '\n' ||
					char1 == '\n' && char2 == '\r'))
				{
					ungetc(char2, file);
				}
			}
			return buf;
		}
		buf[i] = (char)char1;
	}
	buf[bufSize - 1] = 0;
	return buf;
}
*/

bool LDLModel::read(FILE *file)
{
	char buf[2048];
	int lineNumber = 1;
	bool done = false;
	bool retValue = true;

	m_fileLines = new LDLFileLineArray;
	while (!done && !getLoadCanceled())
	{
		if (fgets(buf, sizeof(buf), file))
		{
			LDLFileLine *fileLine;

			stripCRLF(buf);
			fileLine = LDLFileLine::initFileLine(this, buf, lineNumber);
			lineNumber++;
			m_fileLines->addObject(fileLine);
			fileLine->release();
			if (fileLine->getLineType() == LDLLineTypeComment)
			{
				// To a certain extent, this will actually parse the comment, but
				// we really need to do some parsing prior to parsing the rest
				// of the file.
				readComment((LDLCommentLine *)fileLine);
			}
		}
		else
		{
			if (m_activeLineCount == 0)
			{
				m_activeLineCount = m_fileLines->getCount();
			}
			done = true;
		}
	}
	fclose(file);
	m_activeMPDModel = NULL;
	return retValue && !getLoadCanceled();
}

void LDLModel::reportProgress(const char *message, float progress,
							  bool mainOnly)
{
	if (!mainOnly || this == m_mainModel)
	{
		bool loadCanceled;

		TCProgressAlert::send("LDLModel", message, progress, &loadCanceled,
			this);
		if (loadCanceled)
		{
			cancelLoad();
		}
	}
}

void LDLModel::reportProgress(const wchar_t *message, float progress,
							  bool mainOnly)
{
	if (!mainOnly || this == m_mainModel)
	{
		bool loadCanceled;

		TCProgressAlert::send("LDLModel", message, progress, &loadCanceled,
			this);
		if (loadCanceled)
		{
			cancelLoad();
		}
	}
}

bool LDLModel::load(FILE *file, bool trackProgress)
{
	bool retValue;

	if (trackProgress)
	{
		reportProgress(LOAD_MESSAGE, 0.0f);
	}
	if (!read(file))
	{
		if (trackProgress)
		{
			reportProgress(LOAD_MESSAGE, 1.0f);
		}
		return false;
	}
	if (trackProgress)
	{
		reportProgress(LOAD_MESSAGE, MAIN_READ_FRACTION);
	}
	retValue = parse();
	if (trackProgress)
	{
		reportProgress(LOAD_MESSAGE, 1.0f);
	}
	return retValue;
}

int LDLModel::parseMPDMeta(int index, const char *filename)
{
	int i = index + 1;
	int count = m_fileLines->getCount();

	if (m_flags.mainModelParsed)
	{
		LDLModel *subModel;

		for (i = index + 1; i < count; i++)
		{
			LDLFileLine *fileLine = (*m_fileLines)[i];

			if (fileLine->getLineType() == LDLLineTypeComment &&
				((LDLCommentLine *)fileLine)->getMPDFilename(NULL, 0))
			{
				break;
			}
		}
		count = i;
		subModel = (LDLModel *)getLoadedModels()->objectForKey(filename);
		if (subModel)
		{
			if (!subModel->m_fileLines)
			{
				subModel->m_fileLines = new LDLFileLineArray(count - index - 1);
				for (i = index + 1; i < count; i++)
				{
					LDLFileLine *fileLine = (*m_fileLines)[i];

					subModel->m_fileLines->addObject(fileLine);
				}
				subModel->m_activeLineCount = subModel->m_fileLines->getCount();
				if (!subModel->parse())
				{
					return -1;
				}
			}
			else
			{
				reportError(LDLEMPDError, *(*m_fileLines)[index],
					TCLocalStrings::get(_UC("LDLModelMpdAlreadyLoaded")));
			}
		}
	}
	else
	{
		m_flags.mainModelParsed = true;
	}
	return i - index - 1;
}

int LDLModel::parseBFCMeta(LDLCommentLine *commentLine)
{
	if (m_flags.bfcInvertNext)
	{
		reportError(LDLEBFCError, *commentLine,
			TCLocalStrings::get(_UC("LDLModelBfcInvert")));
		m_flags.bfcInvertNext = false;
	}
	if (m_flags.bfcCertify == BFCUnknownState)
	{
		if (commentLine->containsBFCCommand("NOCERTIFY"))
		{
			m_flags.bfcCertify = BFCOffState;
			if (m_flags.started)
			{
				reportError(LDLEBFCError, *commentLine,
					TCLocalStrings::get(_UC("LDLModelBfcNoCertFirst")));
			}
		}
		else
		{
			if (m_flags.started)
			{
				m_flags.bfcCertify = BFCOffState;
				reportError(LDLEBFCError, *commentLine,
					TCLocalStrings::get(_UC("LDLModelBfcFirst")));
			}
			else
			{
				// Unless a NOCERTIFY is present, CERTIFY gets turned on by
				// default for any BFC command.
				if (m_flags.part || m_flags.subPart)
				{
					// BFC certified parts force BFC to be on, even if their
					// parent models don't have BFC certification.
					m_flags.bfcCertify = BFCForcedOnState;
				}
				else
				{
					m_flags.bfcCertify = BFCOnState;
				}
				m_flags.bfcClip = true;
			}
		}
	}
	else
	{
		if (commentLine->containsBFCCommand("CERTIFY"))
		{
			if (getBFCOn())
			{
				reportWarning(LDLEBFCWarning, *commentLine,
					TCLocalStrings::get(_UC("LDLModelBfcCertNotFirst")));
			}
			else
			{
				reportError(LDLEBFCError, *commentLine,
					TCLocalStrings::get(_UC("LDLModelBfcCertNoCert")));
			}
		}
		// Not else if below, because each BFC command could potentially
		// have both certify AND nocertify.
		if (commentLine->containsBFCCommand("NOCERTIFY"))
		{
			if (getBFCOn())
			{
				reportError(LDLEBFCError, *commentLine,
					TCLocalStrings::get(_UC("LDLModelBfcNoCertCert")));
			}
			else
			{
				reportWarning(LDLEBFCWarning, *commentLine,
					TCLocalStrings::get(_UC("LDLModelBfcNoCertMulti")));
			}
		}
	}
	if (getBFCOn())
	{
		if (commentLine->containsBFCCommand("CLIP"))
		{
			if (commentLine->containsBFCCommand("NOCLIP"))
			{
				reportError(LDLEBFCError, *commentLine,
					TCLocalStrings::get(_UC("LDLModelBfcClipNoClip")));
			}
			else
			{
				m_flags.bfcClip = true;
			}
		}
		else if (commentLine->containsBFCCommand("NOCLIP"))
		{
			m_flags.bfcClip = false;
		}
		if (commentLine->containsBFCCommand("CCW"))
		{
			if (commentLine->containsBFCCommand("CW"))
			{
				reportError(LDLEBFCError, *commentLine,
					TCLocalStrings::get(_UC("LDLModelBfcCwCcw")));
			}
			else
			{
				m_flags.bfcWindingCCW = true;
			}
		}
		else if (commentLine->containsBFCCommand("CW"))
		{
			m_flags.bfcWindingCCW = false;
		}
		if (commentLine->containsBFCCommand("INVERTNEXT"))
		{
			m_flags.bfcInvertNext = true;
		}
	}
	else
	{
		if (commentLine->containsBFCCommand("CLIP") ||
			commentLine->containsBFCCommand("NOCLIP") ||
			commentLine->containsBFCCommand("CW") ||
			commentLine->containsBFCCommand("CCW") ||
			commentLine->containsBFCCommand("INVERTNEXT"))
		{
			reportError(LDLEBFCError, *commentLine,
				TCLocalStrings::get(_UC("LDLModelBfcAfterNoCert")));
		}
	}
	return 0;
}

int LDLModel::parseComment(int index, LDLCommentLine *commentLine)
{
	char filename[1024];

	if (commentLine->getMPDFilename(filename, sizeof(filename)))
	{
		replaceStringCharacter(filename, '\\', '/');
		return parseMPDMeta(index, filename);
	}
	else if (commentLine->isBFCMeta())
	{
		return parseBFCMeta(commentLine);
	}
	else if (commentLine->isPartMeta())
	{
		m_stepIndices.push_back(index);
		return 0;
	}
	else if (index == 0)
	{
		delete m_description;
		m_description = copyString(&commentLine->getLine()[1]);
		stripLeadingWhitespace(m_description);
		stripTrailingWhitespace(m_description);
	}
	return 0;
}

/*
void LDLModel::processModelLine(LDLModelLine *modelLine)
{
	m_flags.started = true;
}
*/

bool LDLModel::parse(void)
{
	if (m_fileLines)
	{
		int i;
		int count = m_fileLines->getCount();

		// ********************************************************************
		// NOTE: This for loop does a number of things that aren't normally
		// done (at least by me).  In one place (when a line needs to be
		// replaced by new ones), it inserts new items in the array just after
		// the current spot, changes count, and uses continue.  In another place
		// (when it sees an MPD secondary file), it increases i to skip over all
		// the lines in that secondary file (they get parsed separately).
		// ********************************************************************
		for (i = 0; i < count && !getLoadCanceled(); i++)
		{
			LDLFileLine *fileLine = (*m_fileLines)[i];
			bool checkInvertNext = true;

			if (fileLine->isActionLine())
			{
				m_flags.started = true;
				((LDLActionLine *)fileLine)->setBFCSettings(m_flags.bfcCertify,
					m_flags.bfcClip, m_flags.bfcWindingCCW,
					m_flags.bfcInvertNext);
			}
			else
			{
				checkInvertNext = false;
			}
			if (fileLine->parse())
			{
				if (fileLine->isValid())
				{
					if (fileLine->getError())
					{
						sendAlert(fileLine->getError());
					}
				}
				else
				{
					LDLFileLineArray *replacementLines =
						fileLine->getReplacementLines();

					if (replacementLines)
					{
						int replacementCount = replacementLines->getCount();
						int j;

						for (j = 0; j < replacementCount; j++)
						{
							m_fileLines->insertObject((*replacementLines)[j],
								i + 1);
						}
						// Note that if we do get here, we haven't gotten past
						// m_activeLineCount, because that parsing gets done in
						// the secondary files themselves.
						m_activeLineCount += replacementCount;
						count += replacementCount;
						if (fileLine->getError())
						{
							sendAlert(fileLine->getError());
						}
						replacementLines->release();
						// ****************************************************
						// Note the use of continue below.  I really shy away
						// from using it, but I'm goint to do so here.
						// ****************************************************
						continue;
						// ****************************************************
						// ****************************************************
					}
					else
					{
						sendAlert(fileLine->getError());
					}
				}
			}
			else
			{
				sendAlert(fileLine->getError());
			}
			switch (fileLine->getLineType())
			{
			case LDLLineTypeComment:
				{
					int skippedLines = parseComment(i,
						(LDLCommentLine *)fileLine);

					checkInvertNext = false;
					if (skippedLines >= 0)
					{
						// ****************************************************
						// Note that I increment i below to skip over the lines
						// in the MPD secondary file I just encountered.  (The
						// parseComment function will only return a number
						// greater than 0 if it found an MPD secondary file.
						// ****************************************************
						i += skippedLines;
						// ****************************************************
						// ****************************************************
					}
					else
					{
						return false;
					}
				}
				break;
			case LDLLineTypeModel:
				{
					LDLModel *model = ((LDLModelLine *)fileLine)->getModel();

					if (model)
					{
						model->calcBoundingBox();
					}
					m_flags.bfcInvertNext = false;
				}
				break;
			default:
				break;
			}
			if (checkInvertNext && m_flags.bfcInvertNext)
			{
				reportError(LDLEBFCError, *fileLine,
					TCLocalStrings::get(_UC("LDLModelBfcInvert")));
				m_flags.bfcInvertNext = false;
			}
			fileLine->setStepIndex(m_stepIndices.size());
			reportProgress(LOAD_MESSAGE, (float)i / (float)m_activeLineCount *
				(1.0f - MAIN_READ_FRACTION) + MAIN_READ_FRACTION);
		}
		return !getLoadCanceled();
	}
	else
	{
		// This is in an MPD, and has not yet been fully initialized.
		return true;
	}
}

void LDLModel::cancelLoad(void)
{
	m_mainModel->cancelLoad();
}

bool LDLModel::getLoadCanceled(void)
{
	return m_mainModel->getLoadCanceled();
}

void LDLModel::sendAlert(LDLError *alert)
{
	if (alert)
	{
		TCAlertManager::sendAlert(alert, this);
		if (alert->getLoadCanceled())
		{
			cancelLoad();
		}
	}
}

/*
void LDLModel::sendAlert(LDLErrorType type, LDLAlertLevel level,
						 const char* format, va_list argPtr)
{
	LDLError *alert;

	alert = newError(type, format, argPtr);
	alert->setLevel(level);
	sendAlert(alert);
	alert->release();
}
*/

void LDLModel::sendAlert(LDLErrorType type, LDLAlertLevel level, CUCSTR format,
						 va_list argPtr)
{
	LDLError *alert;

	alert = newError(type, format, argPtr);
	alert->setLevel(level);
	sendAlert(alert);
	alert->release();
}

/*
void LDLModel::sendAlert(LDLErrorType type, LDLAlertLevel level,
						 const LDLFileLine &fileLine, const char* format,
						 va_list argPtr)
{
	LDLError *alert;

	alert = newError(type, fileLine, format, argPtr);
	alert->setLevel(level);
	sendAlert(alert);
	alert->release();
}
*/

void LDLModel::sendAlert(LDLErrorType type, LDLAlertLevel level,
						 const LDLFileLine &fileLine, CUCSTR format,
						 va_list argPtr)
{
	LDLError *alert;

	alert = newError(type, fileLine, format, argPtr);
	alert->setLevel(level);
	sendAlert(alert);
	alert->release();
}

/*
void LDLModel::reportError(LDLErrorType type, const LDLFileLine &fileLine,
						   const char* format, ...)
{
	va_list argPtr;

	va_start(argPtr, format);
	sendAlert(type, LDLAError, fileLine, format, argPtr);
	va_end(argPtr);
}
*/

void LDLModel::reportError(LDLErrorType type, const LDLFileLine &fileLine,
						   CUCSTR format, ...)
{
	va_list argPtr;

	va_start(argPtr, format);
	sendAlert(type, LDLAError, fileLine, format, argPtr);
	va_end(argPtr);
}

/*
void LDLModel::reportWarning(LDLErrorType type, const LDLFileLine &fileLine,
							 const char* format, ...)
{
	va_list argPtr;

	va_start(argPtr, format);
	sendAlert(type, LDLAWarning, fileLine, format, argPtr);
	va_end(argPtr);
}
*/

void LDLModel::reportWarning(LDLErrorType type, const LDLFileLine &fileLine,
							 CUCSTR format, ...)
{
	va_list argPtr;

	va_start(argPtr, format);
	sendAlert(type, LDLAWarning, fileLine, format, argPtr);
	va_end(argPtr);
}

/*
void LDLModel::reportError(LDLErrorType type, const char* format, ...)
{
	va_list argPtr;

	va_start(argPtr, format);
	sendAlert(type, LDLAError, format, argPtr);
	va_end(argPtr);
}
*/

void LDLModel::reportError(LDLErrorType type, CUCSTR format, ...)
{
	va_list argPtr;

	va_start(argPtr, format);
	sendAlert(type, LDLAError, format, argPtr);
	va_end(argPtr);
}

/*
void LDLModel::reportWarning(LDLErrorType type, const char* format, ...)
{
	va_list argPtr;

	va_start(argPtr, format);
	sendAlert(type, LDLAWarning, format, argPtr);
	va_end(argPtr);
}
*/

void LDLModel::reportWarning(LDLErrorType type, CUCSTR format, ...)
{
	va_list argPtr;

	va_start(argPtr, format);
	sendAlert(type, LDLAWarning, format, argPtr);
	va_end(argPtr);
}

TCDictionary *LDLModel::getLoadedModels(void)
{
	return (m_mainModel->getLoadedModels());
}

void LDLModel::print(int indent)
{
	indentPrintf(indent, "LDLModel");
	if (m_flags.part)
	{
		printf(" (Part)");
	}
	if (m_flags.subPart)
	{
		printf(" (SubPart)");
	}
	switch (m_flags.bfcCertify)
	{
	case BFCOffState:
		printf(" (BFC Off)");
		break;
	case BFCOnState:
		printf(" (BFC On)");
		break;
	case BFCForcedOnState:
		printf(" (BFC Forced On)");
		break;
	default:
		break;
	}
	printf(": ");
	if (m_filename)
	{
		printf("%s", m_filename);
	}
	else
	{
		printf("<Unknown>");
	}
	printf("\n");
	if (m_fileLines)
	{
		int i;
		int count = m_fileLines->getCount();

		for (i = 0; i < count; i++)
		{
			(*m_fileLines)[i]->print(indent + 1);
		}
	}
}

bool LDLModel::getLowResStuds(void) const
{
	return m_mainModel->getLowResStuds();
}

LDLError *LDLModel::newError(LDLErrorType type, const LDLFileLine &fileLine,
							 CUCSTR format, va_list argPtr)
{
	UCCHAR message[1024];
	UCCHAR** components;
	int componentCount;
	LDLError *error = NULL;

	vsucprintf(message, COUNT_OF(message), format, argPtr);
	stripCRLF(message);
	components = componentsSeparatedByString(message, _UC("\n"),
		componentCount);
	if (componentCount > 1)
	{
		int i;
#ifdef TC_NO_UNICODE
		TCStringArray *extraInfo = new TCStringArray(componentCount - 1);

		*strchr(message, '\n') = 0;
		for (i = 1; i < componentCount; i++)
		{
			extraInfo->addString(components[i]);
		}
		error = new LDLError(type, message, m_filename, fileLine,
			fileLine.getLineNumber(), extraInfo);
		extraInfo->release();
#else // TC_NO_UNICODE
		ucstringVector extraInfo;
		*wcschr(message, '\n') = 0;
		extraInfo.reserve(componentCount - 1);
		for (i = 1; i < componentCount; i++)
		{
			extraInfo.push_back(components[i]);
		}
		error = new LDLError(type, message, m_filename, fileLine,
			fileLine.getLineNumber(), extraInfo);
#endif // TC_NO_UNICODE
	}
	else
	{
		error = new LDLError(type, message, m_filename, fileLine,
			fileLine.getLineNumber());
	}
	deleteStringArray(components, componentCount);
	return error;
}

LDLError *LDLModel::newError(LDLErrorType type, const LDLFileLine &fileLine,
							 CUCSTR format, ...)
{
	va_list argPtr;
	LDLError *retValue;

	va_start(argPtr, format);
	retValue = newError(type, fileLine, format, argPtr);
	va_end(argPtr);
	return retValue;
}

LDLError *LDLModel::newError(LDLErrorType type, CUCSTR format, va_list argPtr)
{
	UCCHAR message[1024];
	UCCHAR** components;
	int componentCount;
	LDLError *error = NULL;

	vsucprintf(message, COUNT_OF(message), format, argPtr);
	stripCRLF(message);
	components = componentsSeparatedByString(message, _UC("\n"),
		componentCount);
	if (componentCount > 1)
	{
		int i;
#ifdef TC_NO_UNICODE
		TCStringArray *extraInfo = new TCStringArray(componentCount - 1);

		*strchr(message, '\n') = 0;
		for (i = 1; i < componentCount; i++)
		{
			extraInfo->addString(components[i]);
		}
		error = new LDLError(type, message, m_filename, NULL, -1, extraInfo);
		extraInfo->release();
#else // TC_NO_UNICODE
		ucstringVector extraInfo;
		*wcschr(message, '\n') = 0;
		extraInfo.reserve(componentCount - 1);
		for (i = 1; i < componentCount; i++)
		{
			extraInfo.push_back(components[i]);
		}
		error = new LDLError(type, message, m_filename, NULL, -1, extraInfo);
#endif // TC_NO_UNICODE
	}
	else
	{
		error = new LDLError(type, message, m_filename, NULL, -1);
	}
	deleteStringArray(components, componentCount);
	return error;
}

LDLError *LDLModel::newError(LDLErrorType type, CUCSTR format, ...)
{
	va_list argPtr;
	LDLError *retValue;

	va_start(argPtr, format);
	retValue = newError(type, format, argPtr);
	va_end(argPtr);
	return retValue;
}

void LDLModel::scanPoints(
	TCObject *scanner,
	LDLScanPointCallback scanPointCallback,
	const TCFloat *matrix,
	LDLModel::ScanPointType types,
	int step) const
{
	if (this != m_mainModel && isPart() && m_mainModel->getBoundingBoxesOnly()
		&& m_flags.haveBoundingBox)
	{
		TCVector boxPoints[8];
		TCVector point;
		int i;

		boxPoints[0] = m_boundingMin;
		boxPoints[4] = m_boundingMax;
		// Bottom square
		boxPoints[1] = boxPoints[0];
		boxPoints[1][0] = boxPoints[4][0];
		boxPoints[2] = boxPoints[0];
		boxPoints[2][1] = boxPoints[4][1];
		boxPoints[3] = boxPoints[4];
		boxPoints[3][2] = boxPoints[0][2];
		// Top square
		boxPoints[5] = boxPoints[4];
		boxPoints[5][0] = boxPoints[0][0];
		boxPoints[6] = boxPoints[4];
		boxPoints[6][1] = boxPoints[0][1];
		boxPoints[7] = boxPoints[0];
		boxPoints[7][2] = boxPoints[4][2];
		for (i = 0; i < 8; i++)
		{
			boxPoints[i].transformPoint(matrix, point);
			((*scanner).*scanPointCallback)(point, NULL);
		}
	}
	else
	{
		int curStep = 0;

		for (int i = 0; i < m_activeLineCount; i++)
		{
			LDLFileLine *fileLine = (*m_fileLines)[i];
			if (step >= 0 && fileLine->getLineType() == LDLLineTypeComment)
			{
				LDLCommentLine *commentLine = (LDLCommentLine *)fileLine;
				
				if (commentLine->isStepMeta())
				{
					if (++curStep > step)
					{
						break;
					}
				}
			}
			if (fileLine->isActionLine())
			{
				((LDLActionLine *)fileLine)->scanPoints(scanner, scanPointCallback,
					matrix, types);
			}
		}
	}
}

void LDLModel::getBoundingBox(TCVector &min, TCVector &max)
{
	calcBoundingBox();
	min = m_boundingMin;
	max = m_boundingMax;
}

TCFloat LDLModel::getMaxRadius(const TCVector &center)
{
	calcMaxRadius(center);
	return m_maxRadius;
}

void LDLModel::scanBoundingBoxPoint(
	const TCVector &point,
	LDLFileLine * /*pFileLine*/)
{
	if (m_flags.haveBoundingBox)
	{
		for (int i = 0; i < 3; i++)
		{
			if (point[i] < m_boundingMin[i])
			{
				m_boundingMin[i] = point[i];
			}
			else if (point[i] > m_boundingMax[i])
			{
				m_boundingMax[i] = point[i];
			}
		}
	}
	else
	{
		m_boundingMin = m_boundingMax = point;
		m_flags.haveBoundingBox = true;
	}
}

void LDLModel::calcBoundingBox(void)
{
	if (!m_flags.haveBoundingBox)
	{
		TCFloat matrix[16];

		if (this == m_mainModel && m_mainModel->getBoundingBoxesOnly())
		{
			int i;

			for (i = 0; i < m_fileLines->getCount(); i++)
			{
				LDLFileLine *fileLine = (*m_fileLines)[i];

				if (fileLine->getLineType() == LDLLineTypeModel)
				{
					LDLModelLine *modelLine = (LDLModelLine *)fileLine;

					modelLine->getModel()->calcBoundingBox();
				}
			}
		}
		TCVector::initIdentityMatrix(matrix);
		// NOTE: we cannot compute bounding boxes heirarchically due to
		// rotation of child models.  With their rotation, their bounding
		// boxes can easily stick out of the really minimum bounding box of
		// their parent.
		scanPoints(this,
			(LDLScanPointCallback)&LDLModel::scanBoundingBoxPoint, matrix);
	}
}

void LDLModel::scanRadiusPoint(const TCVector &point, LDLFileLine * /*pFileLine*/)
{
	TCFloat radius = (m_center - point).length();

	if (!m_flags.haveMaxRadius || radius > m_maxRadius)
	{
		m_flags.haveMaxRadius = true;
		m_maxRadius = radius;
	}
}

void LDLModel::calcMaxRadius(const TCVector &center)
{
	if (!m_flags.haveMaxRadius)
	{
		TCFloat matrix[16];

		TCVector::initIdentityMatrix(matrix);
		m_center = center;
		scanPoints(this, (LDLScanPointCallback)&LDLModel::scanRadiusPoint,
			matrix);
	}
}

TCObject *LDLModel::getAlertSender(void)
{
	return m_mainModel->getAlertSender();
}

bool LDLModel::hasBoundingBox(void) const
{
	return m_flags.haveBoundingBox != false;
}
