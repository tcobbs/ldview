#include "LDLModel.h"
#include "LDLMainModel.h"
#include "LDLCommentLine.h"
#include "LDLModelLine.h"
#include <TCFoundation/TCDictionary.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStringArray.h>

char *LDLModel::sm_systemLDrawDir = NULL;

#define LDL_LOWRES_PREFIX "LDL-LOWRES:"

LDLModel::LDLModel(void)
	:m_filename(NULL),
	m_name(NULL),
	m_fileLines(NULL),
	m_mainModel(NULL)
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
	m_flags.bfcCertify = BFCUnknownState;
}

LDLModel::LDLModel(const LDLModel &other)
	:m_filename(copyString(other.m_filename)),
	m_name(copyString(other.m_name)),
	m_flags(other.m_flags),
	m_fileLines(NULL),
	m_mainModel(other.m_mainModel)
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
	if (m_fileLines)
	{
		m_fileLines->release();
	}
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

void LDLModel::getRGBA(int colorNumber, int& r, int& g, int& b, int& a)
{
	if (colorNumberIsTransparent(colorNumber))
	{
		a = 110;
	}
	else
	{
		a = 255;
	}
	switch (colorNumber)
	{
		case 0: // Black
			r = 51;		//48;
			g = 51;		//48;
			b = 51;		//48;
			break;
		case 1: // Blue
			r = 0;
			g = 51;		//0;
			b = 178;	//128;
			break;
		case 2: // Green
			r = 0;
			g = 127;	//128;
			b = 51;		//0;
			break;
		case 3: // Dark Cyan
			r = 0;
			g = 181;	//128;
			b = 166;	//128;
			break;
		case 4: // Red
			r = 204;	//128;
			g = 0;
			b = 0;
			break;
		case 5: // Magenta
		case 26: // Dark Pink
			r = 255;	//128;
			g = 51;		//0;
			b = 153;	//128;
			break;
		case 6: // Brown
			r = 102;	//112;
			g = 51;		//71;
			b = 0;		//44;
			break;
		case 7: // Grey
			r = 153;	//128;
			g = 153;	//128;
			b = 153;	//128;
			break;
		case 8: // Dark Grey
			r = 102;	//96;
			g = 102;	//96;
			b = 88;		//96;
			break;
		case 9: // Light Blue
			r = 0;
			g = 128;	//0;
			b = 255;
			break;
		case 10: // Light Green
			r = 51;		//0;
			g = 255;
			b = 102;	//0;
			break;
		case 11: // Cyan
			r = 171;	//0;
			g = 253;	//255;
			b = 249;	//255;
			break;
		case 12: // Light Red
			r = 255;
			g = 0;
			b = 0;
			break;
		case 13: // Pink
			r = 255;
			g = 176;	//128;
			b = 204;	//128;
			break;
		case 14: // Yellow
			r = 255;
			g = 229;	//255;
			b = 0;
			break;
		case 15: // White
			r = 255;
			g = 255;
			b = 255;
			break;
		case 17: // Pastel Green
			r = 102;
			g = 240;
			b = 153;
			break;
		case 18: // Light Yellow
			r = 255;
			g = 255;
			b = 128;
			break;
		case 19: // Tan
		case 28: // Tan
		case 382: // Tan
			r = 204;
			g = 170;
			b = 102;
			break;
		case 20: // Light Purple
			r = 224;
			g = 204;
			b = 240;
			break;
		case 21: // Glow in the dark
			r = 224;
			g = 255;
			b = 176;
			break;
		case 22: // Purple
			r = 153;
			g = 51;
			b = 153;
			break;
		case 23: // Purple Blue
			r = 76;
			g = 0;
			b = 204;
			break;
		case 25: // Orange
			r = 255;
			g = 102;
			b = 0;
			break;
		case 27: // Lime Green
			r = 173;
			g = 221;
			b = 80;
			break;
		case 32: // Transparent Dark Grey
			r = 102;
			g = 102;
			b = 102;
			break;
		case 33: // Transparent Dark Blue
			r = 0;
			g = 0;
			b = 153;
			break;
		case 34: // Transparent Green
			r = 0;
			g = 80;
			b = 24;
			break;
		case 35: // Transparent Dark Cyan
			r = 0;
			g = 181;
			b = 166;
			break;
		case 36: // Transparent Red
			r = 204;
			g = 0;
			b = 0;
			break;
		case 37: // Transparent Magenta
			r = 255;
			g = 51;
			b = 153;
			break;
		case 38: // Transparent Brown (RIIIIIIGHT)
			r = 102;
			g = 51;
			b = 0;
			break;
		case 39: // Transparent Grey
			r = 153;
			g = 153;
			b = 153;
			break;
		case 40: // Transparent Dark Grey
			r = 102;
			g = 102;
			b = 88;
			break;
		case 41: // Transparent Light Blue
			r = 153;
			g = 192;
			b = 240;
			break;
		case 42: // Transparent Green-Yellow
			r = 204;
			g = 255;
			b = 0;
			break;
		case 43: // Transparent Cyan
			r = 171;
			g = 253;
			b = 249;
			break;
		case 44: // Transparent Light Red
			r = 255;
			g = 0;
			b = 0;
			break;
		case 45: // Transparent Pink
			r = 255;
			g = 176;
			b = 204;
			break;
		case 46: // Transparent Yellow
			r = 240;
			g = 196;
			b = 0;
			break;
		case 47: // Clear
			r = 255;
			g = 255;
			b = 255;
			break;
		case 57: // Transparent Orange
			r = 255;
			g = 102;
			b = 0;
			break;
		case 334: // Gold
			r = 240;
			g = 176;
			b = 51;
			break;
		case 383: // Chrome
			r = 204;
			g = 204;
			b = 204;
			break;
		case 494: // Electrical Contacts
			r = 204;
			g = 204;
			b = 204;
			break;
		default:
			if (colorNumber >= 256 && colorNumber < 512)
			{
				int colorNumber1;
				int colorNumber2;
				int r1, g1, b1;
				int r2, g2, b2;

				colorNumber -= 256;
				colorNumber1 = colorNumber / 16;
				colorNumber2 = colorNumber % 16;
				getRGBA(colorNumber1, r1, g1, b1, a);
				getRGBA(colorNumber2, r2, g2, b2, a);
				r = (r1 + r2) / 2;
				g = (g1 + g2) / 2;
				b = (b1 + b2) / 2;
			}
			else if (colorNumber >= 0x2000000 && colorNumber < 0x4000000)
			{
				r = (colorNumber & 0xFF0000) >> 16;
				g = (colorNumber & 0xFF00) >> 8;
				b = (colorNumber & 0xFF);
			}
			else if (colorNumber >= 0x4000000 && colorNumber < 0x5000000)
			{
				r = (((colorNumber & 0xF00000) >> 20) * 17 +
					((colorNumber & 0xF00) >> 8) * 17) / 2;
				g = (((colorNumber & 0xF0000) >> 16) * 17 +
					((colorNumber & 0xF0) >> 4) * 17) / 2;
				b = (((colorNumber & 0xF000) >> 12) * 17 +
					(colorNumber & 0xF) * 17) / 2;
			}
			else if ((colorNumber >= 0x5000000 && colorNumber < 0x6000000) ||
				(colorNumber >= 0x7000000 && colorNumber < 0x8000000))
			{
				r = ((colorNumber & 0xF00000) >> 20) * 17;
				g = ((colorNumber & 0xF0000) >> 16) * 17;
				b = ((colorNumber & 0xF000) >> 12) * 17;
				if (colorNumber >= 0x7000000 && colorNumber < 0x8000000)
				{
					a = 0;
				}
			}
			else if (colorNumber >= 0x6000000 && colorNumber < 0x7000000)
			{
				r = ((colorNumber & 0xF00) >> 8) * 17;
				g = ((colorNumber & 0xF0) >> 4) * 17;
				b = (colorNumber & 0xF) * 17;
			}
			else
			{
				r = 255;
				g = 128;
				b = 0;
			}
			break;
	}
}

bool LDLModel::colorNumberIsTransparent(int colorNumber)
{
	return (colorNumber >= 32 && colorNumber < 48) ||
		(colorNumber >= 0x3000000 && colorNumber < 0x4000000) ||
		(colorNumber >= 0x5000000 && colorNumber < 0x7000000) ||
		(colorNumber >= 0x7000000 && colorNumber < 0x8000000) ||
		colorNumber == 57;
}

TCULong LDLModel::colorForRGBA(int r, int g, int b, int a)
{
	return (r & 0xFF) << 24 | (g & 0xFF) << 16 | (b & 0xFF) << 8 | (a & 0xFF);
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

FILE* LDLModel::openSubModelNamed(const char* subModelName, char* subModelPath)
{
	FILE* subModelFile;

	strcpy(subModelPath, subModelName);
	if ((subModelFile = fopen(subModelPath, "rb")) != NULL)
	{
		return subModelFile;
	}
	sprintf(subModelPath, "%s/P/%s", lDrawDir(), subModelName);
	if ((subModelFile = fopen(subModelPath, "rb")) != NULL)
	{
		return subModelFile;
	}
	sprintf(subModelPath, "%s/PARTS/%s", lDrawDir(), subModelName);
	if ((subModelFile = fopen(subModelPath, "rb")) != NULL)
	{
		m_flags.loadingPart = true;
		return subModelFile;
	}
	sprintf(subModelPath, "%s/MODELS/%s", lDrawDir(), subModelName);
	if ((subModelFile = fopen(subModelPath, "rb")) != NULL)
	{
		return subModelFile;
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
	}
	if (subModelFile && !subModel->load(subModelFile))
	{
		subModelDict->removeObjectForKey(dictName);
		return false;
	}
	subModel->setName(dictName);
	return true;
}

const char* LDLModel::lDrawDir(void)
{
	if (!sm_systemLDrawDir)
	{
		sm_systemLDrawDir = copyString(getenv("LDRAWDIR"));
		if (!sm_systemLDrawDir)
		{
			sm_systemLDrawDir = copyString("C:\\LDRAW");
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
			if (!getLoadedModels()->objectForKey(filename))
			{
				LDLModel *subModel = new LDLModel;

				subModel->setFilename(m_filename);
				initializeNewSubModel(subModel, filename);
			}
		}
		else
		{
			m_flags.mainModelLoaded = true;
		}
	}
	else if (commentLine->isPartMeta())
	{
		m_flags.part = true;
	}
}

bool LDLModel::read(FILE *file)
{
	char buf[2048];
	int lineNumber = 1;
	bool done = false;
	bool retValue = true;

	m_fileLines = new LDLFileLineArray;
	while (!done)
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
			done = true;
		}
	}
	fclose(file);
	return retValue;
}

bool LDLModel::load(FILE *file)
{
	if (!read(file))
	{
		return false;
	}
	return parse();
}

int LDLModel::parseComment(int index, LDLCommentLine *commentLine)
{
	char filename[1024];
	int i = index + 1;
	int count = m_fileLines->getCount();

	if (commentLine->getMPDFilename(filename, sizeof(filename)))
	{
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
			if (subModel && !subModel->m_fileLines)
			{
				subModel->m_fileLines = new LDLFileLineArray(count - index - 1);
				for (i = index + 1; i < count; i++)
				{
					LDLFileLine *fileLine = (*m_fileLines)[i];

					subModel->m_fileLines->addObject(fileLine);
				}
				if (!subModel->parse())
				{
					return -1;
				}
			}
			else
			{
				i = count;
			}
		}
		else
		{
			m_flags.mainModelParsed = true;
		}
	}
	else if (commentLine->isBFCMeta())
	{
		if (m_flags.bfcCertify == BFCUnknownState)
		{
			if (commentLine->containsBFCCommand("NOCERTIFY"))
			{
				m_flags.bfcCertify = BFCOffState;
			}
			else
			{
				// Unless a NOCERTIFY is present, CERTIFY gets turned on by
				// default for any BFC command.
				m_flags.bfcCertify = BFCOnState;
				m_flags.bfcClip = true;
			}
		}
		else
		{
			if (commentLine->containsBFCCommand("CERTIFY"))
			{
				if (m_flags.bfcCertify == BFCOnState)
				{
					reportError(LDLEBFCWarning, *commentLine,
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
				if (m_flags.bfcCertify == BFCOnState)
				{
					reportError(LDLEBFCError, *commentLine,
						"NOCERTIFY command after CERTIFY command.");
				}
				else
				{
					reportError(LDLEBFCWarning, *commentLine,
						"Repeat NOCERTIFY command.");
				}
			}
		}
		if (m_flags.bfcCertify == BFCOnState)
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
	}
	return i - index - 1;
}

void LDLModel::processModelLine(LDLModelLine *modelLine)
{
	m_flags.started = true;
	modelLine->setBFCSettings(m_flags.bfcCertify, m_flags.bfcClip,
		m_flags.bfcWindingCCW, m_flags.bfcInvertNext);
	m_flags.bfcInvertNext = false;
}

bool LDLModel::parse(void)
{
	if (m_fileLines)
	{
		int i;
		int count = m_fileLines->getCount();

		for (i = 0; i < count; i++)
		{
			LDLFileLine *fileLine = (*m_fileLines)[i];
			bool checkInvertNext = true;

			if (!fileLine->parse())
			{
				reportError(fileLine->getError());
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
						i += skippedLines;
					}
					else
					{
						return false;
					}
				}
				break;
			case LDLLineTypeModel:
				processModelLine((LDLModelLine *)fileLine);
				break;
			case LDLLineTypeLine:
				m_flags.started = true;
				break;
			case LDLLineTypeTriangle:
				m_flags.started = true;
				break;
			case LDLLineTypeQuad:
				m_flags.started = true;
				break;
			case LDLLineTypeConditionalLine:
				m_flags.started = true;
				break;
			case LDLLineTypeEmpty:
				checkInvertNext = false;
				break;
			case LDLLineTypeUnknown:
				checkInvertNext = false;
				break;
			}
			if (checkInvertNext && m_flags.bfcInvertNext)
			{
				reportError(LDLEBFCError, *fileLine,
					"Line following BFC INVERTNEXT isn't a linetype 1.");
			}
		}
		return true;
	}
	else
	{
		// This is in an MPD, and has not yet been fully initialized.
		return true;
	}
}

void LDLModel::reportError(LDLError *error)
{
	if (error)
	{
		printf("Error: %s\n", error->getMessage());
	}
}

void LDLModel::reportError(LDLErrorType type, const LDLFileLine &fileLine,
						   const char* format, ...)
{
	va_list argPtr;
	LDLError *error;

	va_start(argPtr, type);
	error = newError(type, fileLine, format, argPtr);
	va_end(argPtr);
	reportError(error);
	error->release();
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

	va_start(argPtr, type);
	return newError(type, fileLine, format, argPtr);
	va_end(argPtr);
}

