#include "LDLModel.h"
#include "LDLMainModel.h"
#include "LDLCommentLine.h"
#include "LDLModelLine.h"
#include <TCFoundation/TCDictionary.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCAlertManager.h>
#include <TCFoundation/TCProgressAlert.h>

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#else // WIN32
#include <unistd.h>
#endif // WIN32

#define LDL_LOWRES_PREFIX "LDL-LOWRES:"
#define LOAD_MESSAGE "Loading..."
#define MAIN_READ_FRACTION 0.1f

char *LDLModel::sm_systemLDrawDir = NULL;
int LDLModel::sm_modelCount = 0;
LDLFileCaseCallback LDLModel::fileCaseCallback = NULL;
LDLModel::LDLModelCleanup LDLModel::sm_cleanup;

LDLModel::LDLModelCleanup::~LDLModelCleanup(void)
{
	delete LDLModel::sm_systemLDrawDir;
	LDLModel::sm_systemLDrawDir = NULL;
}


LDLModel::LDLModel(void)
	:m_filename(NULL),
	m_name(NULL),
	m_fileLines(NULL),
	m_mainModel(NULL),
	m_activeLineCount(0),
	m_activeMPDModel(NULL)
{
	// Initialize Private flags
	m_flags.loadingPart = false;
	m_flags.mainModelLoaded = false;
	m_flags.mainModelParsed = false;
	m_flags.started = false;
	m_flags.bfcClip = false;
	m_flags.bfcWindingCCW = true;
	m_flags.bfcInvertNext = false;
	// Initialize Public flags
	m_flags.part = false;
	m_flags.mpd = false;
	m_flags.bfcCertify = BFCUnknownState;
	sm_modelCount++;
}

LDLModel::LDLModel(const LDLModel &other)
	:m_filename(copyString(other.m_filename)),
	m_name(copyString(other.m_name)),
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

LDLModel *LDLModel::subModelNamed(const char *subModelName, bool lowRes)
{
	TCDictionary* subModelDict = getLoadedModels();
	LDLModel* subModel;
	char *dictName = NULL;
	char *adjustedName;

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

		if ((subModelFile = openSubModelNamed(adjustedName, subModelPath))
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
		}
	}
	delete adjustedName;
	delete dictName;
	return subModel;
}

FILE *LDLModel::openModelFile(const char *filename)
{
	if (fileCaseCallback)
	{
		char *newFilename = copyString(filename);
		FILE *modelFile;

		convertStringToLower(newFilename);
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
		delete newFilename;
		return modelFile;
	}
	else
	{
		char *newFilename = copyString(filename);
		FILE *modelFile;

		convertStringToLower(newFilename);
		modelFile = fopen(newFilename, "rb");
		delete newFilename;
		return modelFile;
	}
}

FILE* LDLModel::openSubModelNamed(const char* subModelName, char* subModelPath)
{
	FILE* subModelFile;
	TCStringArray *extraSearchDirs = m_mainModel->getExtraSearchDirs();

	strcpy(subModelPath, subModelName);
	// Use binary mode to work around problem with fseek on a non-binary file.
	// file.  The file parsing code will still work fine and strip out the extra
	// data.
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
		m_flags.loadingPart = true;
		return subModelFile;
	}
	sprintf(subModelPath, "%s/MODELS/%s", lDrawDir(), subModelName);
	if ((subModelFile = openModelFile(subModelPath)) != NULL)
	{
		return subModelFile;
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
//		subModel->m_flags.bfcCertify = BFCForcedOnState;
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
void LDLModel::setLDrawDir(const char *value)
{
	if (value != sm_systemLDrawDir)
	{
		delete sm_systemLDrawDir;
		sm_systemLDrawDir = copyString(value);
	}
}

// NOTE: static function.
const char* LDLModel::lDrawDir(void)
{
	if (!sm_systemLDrawDir)
	{
		sm_systemLDrawDir = copyString(getenv("LDRAWDIR"));
		if (!verifyLDrawDir(sm_systemLDrawDir))
		{
			delete sm_systemLDrawDir;
			sm_systemLDrawDir = NULL;
		}
		if (!sm_systemLDrawDir)
		{
#ifdef WIN32
			char buf[1024];

			if (GetPrivateProfileString("LDraw", "BaseDirectory",
				"", buf, 1024, "ldraw.ini"))
			{
				buf[1023] = 0;
				if (verifyLDrawDir(buf))
				{
					sm_systemLDrawDir = copyString(buf);
				}
			}
			if (!sm_systemLDrawDir)
			{
				sm_systemLDrawDir = copyString("C:\\LDRAW");
			}
#else // WIN32
			sm_systemLDrawDir = copyString("/usr/local/ldraw");
#endif // WIN32
		}
		if (!verifyLDrawDir(sm_systemLDrawDir))
		{
			delete sm_systemLDrawDir;
			sm_systemLDrawDir = copyString("");
		}
	}
	return sm_systemLDrawDir;
}

void LDLModel::readComment(LDLCommentLine *commentLine)
{
	char filename[1024];

	if (commentLine->getMPDFilename(filename, sizeof(filename)))
	{
		if (m_flags.mainModelLoaded)
		{
			if (m_activeLineCount == 0)
			{
				m_activeLineCount = commentLine->getLineNumber() - 1;
			}
			if (!getLoadedModels()->objectForKey(filename))
			{
				LDLModel *subModel = new LDLModel;

				subModel->setFilename(m_filename);
				initializeNewSubModel(subModel, filename);
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
		// No matter what the comment says, remember that a primitive cannot
		// be a part, so if we found the file in the P directory, then by
		// definition it isn't a part.
		if (m_flags.mainModelLoaded)
		{
			if (m_activeMPDModel && !m_activeMPDModel->isPrimitive())
			{
				m_activeMPDModel->m_flags.part = true;
			}
		}
		else if (!isPrimitive())
		{
			m_flags.part = true;
			// This is now a part, so if we've already hit a BFC CERTIFY line
			// switch it to forced certify.
			if (m_flags.bfcCertify == BFCOnState)
			{
				m_flags.bfcCertify = BFCForcedOnState;
			}
		}
	}
}

bool LDLModel::read(FILE *file)
{
	char buf[2048];
	int lineNumber = 1;
	bool done = false;
	bool retValue = true;

	m_fileLines = new LDLFileLineArray;
	while (!done && !getLoadCanceled())
	{
		if (fgets(buf, 2048, file))
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

		TCProgressAlert::send("LDLModel", message, progress, &loadCanceled);
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
					"MPD sub-file already loaded: ignoring");
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
			"First action following BFC INVERTNEXT isn't linetype 1.\n"
			"Ignoring BFC INVERTNEXT command.");
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
					"NOCERTIFY command isn't the first action in file.");
			}
		}
		else
		{
			if (m_flags.started)
			{
				m_flags.bfcCertify = BFCOffState;
				reportError(LDLEBFCError, *commentLine,
					"First BFC command isn't the first action in file; "
					"changing to NOCERTIFY.");
			}
			else
			{
				// Unless a NOCERTIFY is present, CERTIFY gets turned on by
				// default for any BFC command.
				if (m_flags.part)
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
					"CERTIFY command after other BFC commands.");
			}
			else
			{
				reportError(LDLEBFCError, *commentLine,
					"CERTIFY command after NOCERTIFY command.");
			}
		}
		// Not else if below, because each BFC command could potentially
		// have both certify AND nocertify.
		if (commentLine->containsBFCCommand("NOCERTIFY"))
		{
			if (getBFCOn())
			{
				reportError(LDLEBFCError, *commentLine,
					"NOCERTIFY command after CERTIFY command.");
			}
			else
			{
				reportWarning(LDLEBFCWarning, *commentLine,
					"Repeat NOCERTIFY command.");
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
					"CLIP and NOCLIP both specified in one BFC command.");
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
					"CW and CCW both specified in one BFC command.");
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
				"BFC command after NOCERTIFY command.");
		}
	}
	return 0;
}

int LDLModel::parseComment(int index, LDLCommentLine *commentLine)
{
	char filename[1024];

	if (commentLine->getMPDFilename(filename, sizeof(filename)))
	{
		return parseMPDMeta(index, filename);
	}
	else if (commentLine->isBFCMeta())
	{
		return parseBFCMeta(commentLine);
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
				m_flags.bfcInvertNext = false;
				break;
			default:
				break;
			}
			if (checkInvertNext && m_flags.bfcInvertNext)
			{
				reportError(LDLEBFCError, *fileLine,
					"First action following BFC INVERTNEXT isn't linetype "
					"1.\nIgnoring BFC INVERTNEXT command.");
				m_flags.bfcInvertNext = false;
			}
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
		TCAlertManager::sendAlert(alert);
		if (alert->getLoadCanceled())
		{
			cancelLoad();
		}
	}
}

void LDLModel::sendAlert(LDLErrorType type, LDLAlertLevel level,
						 const char* format, va_list argPtr)
{
	LDLError *alert;

	alert = newError(type, format, argPtr);
	alert->setLevel(level);
	sendAlert(alert);
	alert->release();
}

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

void LDLModel::reportError(LDLErrorType type, const LDLFileLine &fileLine,
						   const char* format, ...)
{
	va_list argPtr;

	va_start(argPtr, format);
	sendAlert(type, LDLAError, fileLine, format, argPtr);
	va_end(argPtr);
}

void LDLModel::reportWarning(LDLErrorType type, const LDLFileLine &fileLine,
							 const char* format, ...)
{
	va_list argPtr;

	va_start(argPtr, format);
	sendAlert(type, LDLAWarning, fileLine, format, argPtr);
	va_end(argPtr);
}

void LDLModel::reportError(LDLErrorType type, const char* format, ...)
{
	va_list argPtr;

	va_start(argPtr, format);
	sendAlert(type, LDLAError, format, argPtr);
	va_end(argPtr);
}

void LDLModel::reportWarning(LDLErrorType type, const char* format, ...)
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
							 const char* format, va_list argPtr)
{
	char message[1024];
	char** components;
	int componentCount;
	LDLError *error = NULL;

	vsprintf(message, format, argPtr);
	stripCRLF(message);
	components = componentsSeparatedByString(message, "\n", componentCount);
	if (componentCount > 1)
	{
		int i;
		TCStringArray *extraInfo = new TCStringArray(componentCount - 1);

		*strchr(message, '\n') = 0;
		for (i = 1; i < componentCount; i++)
		{
			extraInfo->addString(components[i]);
		}
		error = new LDLError(type, message, m_filename, fileLine,
			fileLine.getLineNumber(), extraInfo);
		extraInfo->release();
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
							 const char* format, ...)
{
	va_list argPtr;
	LDLError *retValue;

	va_start(argPtr, format);
	retValue = newError(type, fileLine, format, argPtr);
	va_end(argPtr);
	return retValue;
}

LDLError *LDLModel::newError(LDLErrorType type, const char* format,
							 va_list argPtr)
{
	char message[1024];
	char** components;
	int componentCount;
	LDLError *error = NULL;

	vsprintf(message, format, argPtr);
	stripCRLF(message);
	components = componentsSeparatedByString(message, "\n", componentCount);
	if (componentCount > 1)
	{
		int i;
		TCStringArray *extraInfo = new TCStringArray(componentCount - 1);

		*strchr(message, '\n') = 0;
		for (i = 1; i < componentCount; i++)
		{
			extraInfo->addString(components[i]);
		}
		error = new LDLError(type, message, m_filename, NULL, -1, extraInfo);
		extraInfo->release();
	}
	else
	{
		error = new LDLError(type, message, m_filename, NULL, -1);
	}
	deleteStringArray(components, componentCount);
	return error;
}

LDLError *LDLModel::newError(LDLErrorType type, const char* format, ...)
{
	va_list argPtr;
	LDLError *retValue;

	va_start(argPtr, format);
	retValue = newError(type, format, argPtr);
	va_end(argPtr);
	return retValue;
}
