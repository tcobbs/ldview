#include "LDLCommentLine.h"
#include "LDLPalette.h"
#include "LDLMainModel.h"
#include <stdio.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCStringArray.h>
#include <TCFoundation/TCLocalStrings.h>

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

TCObject *LDLCommentLine::copy(void)
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
		if (strcasecmp((*m_words)[0], "ldraw_org") == 0 ||
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

	if (numWords >= 2 && (strcasecmp((*m_words)[0], "unofficial") == 0 ||
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

bool LDLCommentLine::isMovedToMeta(void) const
{
	if (stringHasCaseInsensitivePrefix(m_processedLine, "0 ~moved to "))
	{
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


