#include "LDLCommentLine.h"
#include "LDLPalette.h"
#include "LDLMainModel.h"
#include <stdio.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCLocalStrings.h>

#ifdef WIN32
#if defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif // _DEBUG
#endif // WIN32

LDLCommentLine::LDLCommentLine(LDLModel *parentModel, const char *line,
							   int lineNumber, const char *originalLine)
	:LDLFileLine(parentModel, line, lineNumber, originalLine),
	m_processedLine(NULL),
	m_words(NULL)
{
	setupProcessedLine();
}

LDLCommentLine::LDLCommentLine(const LDLCommentLine &other)
	:LDLFileLine(other),
	m_processedLine(copyString(other.m_processedLine)),
	m_words((TCStringArray *)TCObject::copy(other.m_words))
{
}

LDLCommentLine::~LDLCommentLine(void)
{
}

void LDLCommentLine::dealloc(void)
{
	delete m_processedLine;
	TCObject::release(m_words);
	LDLFileLine::dealloc();
}

bool LDLCommentLine::parse(void)
{
	if (isMovedToMeta())
	{
		if (m_parentModel)
		{
			char *newName = m_processedLine + strlen("0 ~moved to ");
			char partName[1024];
			const char *filename = m_parentModel->getFilename();
			const char *nameSpot = strrchr(filename, '/');
			const char *tmp = strrchr(filename, '\\');

			if (tmp > nameSpot)
			{
				nameSpot = tmp;
			}
			strcpy(partName, nameSpot + 1);
			tmp = strchr(partName, '.');
			if (tmp)
			{
				partName[tmp - partName] = 0;
			}
			UCSTR ucPartName = mbstoucstring(partName);
			UCSTR ucNewName = mbstoucstring(newName);
			setWarning(LDLEMovedTo,
				TCLocalStrings::get(_UC("LDLComPartRenamed")), ucPartName,
				ucNewName);
			delete ucPartName;
			delete ucNewName;
		}
		else
		{
			setWarning(LDLEMovedTo,
				TCLocalStrings::get(_UC("LDLComUnknownPartRenamed")));
		}
	}
	if (m_parentModel)
	{
		LDLMainModel *mainModel = m_parentModel->getMainModel();

		if (mainModel)
		{
			LDLPalette *palette = mainModel->getPalette();

			if (palette)
			{
				if (palette->isColorComment(m_processedLine))
				{
					palette->parseColorComment(m_processedLine);
				}
			}
		}
	}
	return true;
}

TCObject *LDLCommentLine::copy(void) const
{
	return new LDLCommentLine(*this);
}

bool LDLCommentLine::getMPDFilename(char *filename, int maxLength) const
{
//	if (stringHasCaseInsensitivePrefix(m_processedLine, "0 FILE ") &&
	if (stringHasPrefix(m_processedLine, "0 FILE ") &&
		strlen(m_processedLine) > 7)
	{
		if (maxLength)
		{
			strncpy(filename, m_processedLine + 7, maxLength);
			filename[maxLength - 1] = 0;
		}
		return true;
	}
	return false;
}

void LDLCommentLine::setupProcessedLine(void)
{
	if (!m_processedLine)
	{
		char* spot;
		m_processedLine = copyString(m_line);

		// First, strip all leading and trailing whitespace.
		stripLeadingWhitespace(m_processedLine);
		stripTrailingWhitespace(m_processedLine);
		// Next, convert all tabs to spaces.
		while ((spot = strchr(m_processedLine, '\t')) != NULL)
		{
			spot[0] = ' ';
		}
		// Finally, replace all multi-spaces with single spaces.
		spot = m_processedLine;
		while ((spot = strstr(spot, "  ")) != NULL)
		{
			memmove(spot, spot + 1, strlen(spot));
		}
	}
	if (strlen(m_processedLine) > 2)
	{
		int numWords;
		char **words = componentsSeparatedByString(m_processedLine + 2, " ",
			numWords);

		m_words = new TCStringArray(words, numWords);
		deleteStringArray(words, numWords);
	}
	else
	{
		m_words = new TCStringArray(0);
	}
}

bool LDLCommentLine::isNoShrinkMeta(void) const
{
	int numWords = m_words->getCount();

	if (numWords >= 2 && strcasecmp((*m_words)[0], "~lsynth") == 0 &&
		strcasecmp((*m_words)[1], "constraint") == 0)
	{
		return true;
	}
	return false;
}

bool LDLCommentLine::isPrimitiveMeta(void) const
{
	int word = 0;
	int numWords = m_words->getCount();

	if (numWords == 2 && strcasecmp((*m_words)[0], "!ldraw_org") == 0 &&
		(strcasecmp((*m_words)[1], "primitive") ||
		strcasecmp((*m_words)[1], "48_primitive") ||
		strcasecmp((*m_words)[1], "unofficial_primitive") ||
		strcasecmp((*m_words)[1], "unofficial_48_primitive")))
	{
		return true;
	}
	if (numWords >= 2 && (strcasecmp((*m_words)[0], "unofficial") == 0 ||
		strcasecmp((*m_words)[0], "un-official") == 0 ||
		strcasecmp((*m_words)[0], "ldraw_org") == 0 ||
		strcasecmp((*m_words)[0], "original") == 0))
	{
		if (strcasecmp((*m_words)[1], "ldraw") == 0)
		{
			word = 2;
		}
		else
		{
			word = 1;
		}
	}
	else if (numWords >= 3 && strcasecmp((*m_words)[0], "original") == 0 &&
		strcasecmp((*m_words)[1], "ldraw") == 0 &&
		strcasecmp((*m_words)[2], "primitive") == 0)
	{
		return true;
	}
	if (word && strcasecmp((*m_words)[word], "primitive") == 0)
	{
		return true;
	}
	return false;
}

bool LDLCommentLine::isOfficialPartMeta(bool partMeta) const
{
	if (partMeta)
	{
		if ((strcasecmp((*m_words)[0], "!ldraw_org") == 0 &&
			strcasecmp((*m_words)[1], "part") == 0) ||
			strcasecmp((*m_words)[0], "ldraw_org") == 0 ||
			strcasecmp((*m_words)[0], "official") == 0 ||
			strcasecmp((*m_words)[0], "original") == 0)
		{
			return true;
		}
	}
	return false;
}

bool LDLCommentLine::isPartMeta(void) const
{
	int word = 0;
	int numWords = m_words->getCount();

	if (numWords == 2 && strcasecmp((*m_words)[0], "!ldraw_org") == 0 &&
		(strcasecmp((*m_words)[1], "part") ||
		strcasecmp((*m_words)[1], "unofficial_part")))
	{
		return true;
	}
	else if (numWords >= 2 && (strcasecmp((*m_words)[0], "unofficial") == 0 ||
		strcasecmp((*m_words)[0], "un-official") == 0 ||
		strcasecmp((*m_words)[0], "ldraw_org") == 0 ||
		strcasecmp((*m_words)[0], "custom") == 0))
	{
		if (strcasecmp((*m_words)[1], "ldraw") == 0)
		{
			word = 2;
		}
		else
		{
			word = 1;
		}
	}
	else if (numWords >= 3 && strcasecmp((*m_words)[0], "official") == 0 &&
		strcasecmp((*m_words)[1], "lcad") == 0)
	{
		if (strcasecmp((*m_words)[2], "update") == 0)
		{
			return true;
		}
		word = 2;
	}
	else if (numWords >= 3 && strcasecmp((*m_words)[0], "original") == 0 &&
		strcasecmp((*m_words)[1], "ldraw") == 0 &&
		strcasecmp((*m_words)[2], "part") == 0)
	{
		return true;
	}
	if (word && (strcasecmp((*m_words)[word], "part") == 0 ||
		strcasecmp((*m_words)[word], "element") == 0))
	{
		return true;
	}
	return false;
}

bool LDLCommentLine::isBFCMeta(void) const
{
//	if (stringHasCaseInsensitivePrefix(m_processedLine, "0 BFC "))
	if (stringHasPrefix(m_processedLine, "0 BFC "))
	{
		return true;
	}
	return false;
}

bool LDLCommentLine::isBIMeta(void) const
{
	return(stringHasPrefix(m_processedLine, "0 BI "));
}

// BI

// 0 BI SET <token>
// 0 BI UNSET <token>
// 0 BI NEXT <color> [IFSET <token>|IFNSET <token>] 
// 0 BI START <color> [IFSET <token>|IFNSET <token>]
// 0 BI END
LDLCommentLine::BICommand LDLCommentLine::getBICommand(void) const
{
	BICommand result = BICERROR;

	if (m_words->getCount() > 1)
	{
		if (strcasecmp((*m_words)[1], "set") == 0)
		{
			result = BICSet;
		}
		else if (strcasecmp((*m_words)[1], "unset") == 0)
		{
			result = BICUnset;
		}
		else if (strcasecmp((*m_words)[1], "next") == 0)
		{
			result = BICNext;
		}
		else if (strcasecmp((*m_words)[1], "start") == 0)
		{
			result = BICStart;
		}
		else if (strcasecmp((*m_words)[1], "end") == 0)
		{
			result = BICEnd;
		}
	}
	return result;
}

bool LDLCommentLine::hasBIConditional(void) const
{
	BICommand cmd = getBICommand();

	if (cmd == BICNext || cmd == BICStart)
	{
		// 0 BI NEXT <color> <IFcommand> <token>
		// 0 BI START <color> <IFcommand> <token>
		if (m_words->getCount() > 3)
		{
			return (strcasecmp((*m_words)[3], "ifset") == 0)
				|| (strcasecmp((*m_words)[3], "ifnset") == 0);
		}
	}
	return false;
}

bool LDLCommentLine::getBIConditional(void) const
{
	BICommand cmd = getBICommand();

	if (cmd == BICNext || cmd == BICStart)
	{
		// 0 BI NEXT <color> <IFcommand>
		// 0 BI START <color> <IFcommand>
		if (m_words->getCount() > 3)
		{
			if (strcasecmp((*m_words)[3], "ifset") == 0)
				return true;
			else if (strcasecmp((*m_words)[3], "ifnset") == 0)
				return false;
		}
	}
	// error condition here
	return true;
}

bool LDLCommentLine::hasBIToken(void) const
{
	BICommand cmd = getBICommand();

	if ((cmd == BICSet || cmd == BICUnset) && m_words->getCount() > 2)
	{
		return true;
	}
	if ((cmd == BICNext || cmd == BICStart) && m_words->getCount() > 3)
	{
		return true;
	}
	return false;
}

const char *LDLCommentLine::getBIToken(void) const
{
	if (hasBIToken())
	{
		BICommand cmd = getBICommand();

		// 0 BI SET <token>
		if (cmd == BICSet || cmd == BICUnset)
		{
			return (*m_words)[2];
		}
		// 0 BI START <color> IFSET <token>
		// 0 BI START <color> IFNSET <token>
		// 0 BI NEXT <color> IFSET <token>
		// 0 BI NEXT <color> IFNSET <token>
		if (cmd == BICNext || cmd == BICStart)
		{
			return (*m_words)[4];
		}
	}
	return 0;
}

int LDLCommentLine::getBIColorNumber(void) const
{
	int result = 0;

	// 0 BI NEXT <color> [IF...]
	if (m_words->getCount() > 2)
	{
		result = atoi((*m_words)[2]);
	}
	return result;
}


/* bool LDLCommentLine::isBICommandConditional(void) const
{
	bool result = false;

	BICommand cmd = getBICommand();

	if (BICommand == BICNext || BICommand == BICStart)
	{
		// 0 BI NEXT <color> <IFcommand>
		// 0 BI START <color> <IFcommand>
		if (m_words->getCount() > 3)
		{
			result = (strcasecmp((*m_words[3], "ifset") == 0)
				|| (strcasecmp((*m_words[3], "ifnset") == 0));
		}
	}

	return result;
}
*/

// /BI

bool LDLCommentLine::isMovedToMeta(void) const
{
	if (stringHasCaseInsensitivePrefix(m_processedLine, "0 ~moved to "))
	{
		return true;
	}
	return false;
}

bool LDLCommentLine::isStepMeta(void) const
{
	if (stringHasCaseInsensitivePrefix(m_processedLine, "0 step"))
	{
		return true;
	}
	return false;
}

bool LDLCommentLine::getAuthor(char *author, int maxLength) const
{
	const char *authorSpot = NULL;
	if (stringHasCaseInsensitivePrefix(m_processedLine, "0 author "))
	{
		authorSpot = &m_processedLine[strlen("0 author ")];
	}
	else if (stringHasCaseInsensitivePrefix(m_processedLine, "0 author: "))
	{
		authorSpot = &m_processedLine[strlen("0 author: ")];
	}
	if (authorSpot)
	{
		strncpy(author, authorSpot, maxLength);
		author[maxLength - 1] = 0;
		return true;
	}
	return false;
}

bool LDLCommentLine::containsBFCCommand(const char *command) const
{
	if (isBFCMeta())
	{
		int i;
		int numWords = m_words->getCount();

		for (i = 1; i < numWords; i++)
		{
//			if (strcasecmp((*m_words)[i], command) == 0)
			if (strcmp((*m_words)[i], command) == 0)
			{
				return true;
			}
		}
	}
	return false;
}


