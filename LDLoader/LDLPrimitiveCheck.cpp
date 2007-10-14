#include "LDLPrimitiveCheck.h"

#include <string.h>

#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLShapeLine.h>
#include <LDLoader/LDLModelLine.h>
#include <LDLoader/LDLConditionalLineLine.h>
#include <LDLoader/LDLPalette.h>
#include <TCFoundation/mystring.h>
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/TCVector.h>
#include <TCFoundation/TCLocalStrings.h>
#include <ctype.h>

static const int LO_NUM_SEGMENTS = 8;
static const int HI_NUM_SEGMENTS = 16;

LDLPrimitiveCheck::LDLPrimitiveCheck(void):
	m_curveQuality(2)
{
	m_flags.primitiveSubstitution = true;
	m_flags.noLightGeom = false;
}

LDLPrimitiveCheck::~LDLPrimitiveCheck(void)
{
}

void LDLPrimitiveCheck::dealloc(void)
{
	TCObject::dealloc();
}

TCFloat LDLPrimitiveCheck::startingFraction(const char *filename)
{
	if (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\"))
	{
		filename += 3;
	}
	sscanf(filename, "%d", &m_filenameNumerator);
	sscanf(filename + 2, "%d", &m_filenameDenom);
	return (TCFloat)m_filenameNumerator / (TCFloat)m_filenameDenom;
}

bool LDLPrimitiveCheck::startsWithFraction(const char *filename)
{
	return isdigit(filename[0]) && filename[1] == '-' && isdigit(filename[2]) &&
		!isdigit(filename[3]);
}

bool LDLPrimitiveCheck::startsWithFraction2(const char *filename)
{
	return isdigit(filename[0]) && filename[1] == '-' && isdigit(filename[2]) &&
		isdigit(filename[3]) && !isdigit(filename[4]);
}

bool LDLPrimitiveCheck::isPrimitive(const char *filename, const char *suffix,
								bool *is48)
{
	int fileLen = strlen(filename);
	int suffixLen = strlen(suffix);

	if (is48 != NULL)
	{
		*is48 = false;
	}
	if (((fileLen == suffixLen + 3 && startsWithFraction(filename)) ||
		(suffixLen <= 8 && fileLen == suffixLen + 4 &&
		startsWithFraction2(filename))) &&
		stringHasCaseInsensitiveSuffix(filename, suffix))
	{
		return true;
	}
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return isPrimitive(filename + 3, suffix, NULL);
	}
	return false;
}

bool LDLPrimitiveCheck::isCyli(const char *filename, bool *is48)
{
	return isPrimitive(filename, "cyli.dat", is48);
}

bool LDLPrimitiveCheck::isCyls(const char *filename, bool *is48)
{
	return isPrimitive(filename, "cyls.dat", is48);
}

bool LDLPrimitiveCheck::isCyls2(const char *filename, bool *is48)
{
	return isPrimitive(filename, "cyls2.dat", is48);
}

bool LDLPrimitiveCheck::isChrd(const char *filename, bool *is48)
{
	return isPrimitive(filename, "chrd.dat", is48);
}

bool LDLPrimitiveCheck::isDisc(const char *filename, bool *is48)
{
	return isPrimitive(filename, "disc.dat", is48);
}

bool LDLPrimitiveCheck::isNdis(const char *filename, bool *is48)
{
	return isPrimitive(filename, "ndis.dat", is48);
}

bool LDLPrimitiveCheck::isEdge(const char *filename, bool *is48)
{
	return isPrimitive(filename, "edge.dat", is48);
}

bool LDLPrimitiveCheck::is1DigitCon(const char *filename, bool *is48)
{
	if (is48 != NULL)
	{
		*is48 = false;
	}
	if (strlen(filename) == 11 && startsWithFraction(filename) &&
		stringHasCaseInsensitivePrefix(filename + 3, "con") &&
		isdigit(filename[6]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat"))
	{
		return true;
	}
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return is1DigitCon(filename + 3, NULL);
	}
	return false;
}

bool LDLPrimitiveCheck::is2DigitCon(const char *filename, bool *is48)
{
	if (is48 != NULL)
	{
		*is48 = false;
	}
	if (strlen(filename) == 12 && startsWithFraction(filename) &&
		stringHasCaseInsensitivePrefix(filename + 3, "con") &&
		isdigit(filename[6]) && isdigit(filename[7]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat"))
	{
		return true;
	}
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return is1DigitCon(filename + 3, NULL);
	}
	return false;
}

bool LDLPrimitiveCheck::isCon(const char *filename, bool *is48)
{
	return is1DigitCon(filename, is48) || is2DigitCon(filename, is48);
}

bool LDLPrimitiveCheck::isOldRing(const char *filename, bool *is48)
{
	int len = strlen(filename);

	if (is48 != NULL)
	{
		*is48 = false;
	}
	if (len >= 9 && len <= 12 &&
		stringHasCaseInsensitivePrefix(filename, "ring") &&
		stringHasCaseInsensitiveSuffix(filename, ".dat"))
	{
		int i;

		for (i = 4; i < len - 5; i++)
		{
			if (!isdigit(filename[i]))
			{
				return false;
			}
		}
		return true;
	}
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return isOldRing(filename + 3, NULL);
	}
	else
	{
		return false;
	}
}

bool LDLPrimitiveCheck::isRing(const char *filename, bool *is48)
{
	if (is48 != NULL)
	{
		*is48 = false;
	}
	if (strlen(filename) == 12 && startsWithFraction(filename) &&
		stringHasCaseInsensitivePrefix(filename + 3, "ring") &&
		isdigit(filename[7]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat"))
	{
		return true;
	}
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return isRing(filename + 3, NULL);
	}
	else
	{
		return false;
	}
}

bool LDLPrimitiveCheck::isRin(const char *filename, bool *is48)
{
	if (is48 != NULL)
	{
		*is48 = false;
	}
	if (strlen(filename) == 12 && startsWithFraction(filename) &&
		stringHasCaseInsensitivePrefix(filename + 3, "rin") &&
		isdigit(filename[6]) && isdigit(filename[7]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat"))
	{
		return true;
	}
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return isRin(filename + 3, NULL);
	}
	else
	{
		return false;
	}
}

bool LDLPrimitiveCheck::isTorus(const char *filename, bool *is48)
{
	if (is48 != NULL)
	{
		*is48 = false;
	}
	if (strlen(filename) == 12 && toupper(filename[0]) == 'T' &&
		isdigit(filename[1]) && isdigit(filename[2]) &&
		isdigit(filename[4]) && isdigit(filename[7]) && isdigit(filename[6]) &&
		isdigit(filename[7]) &&
		stringHasCaseInsensitiveSuffix(filename, ".dat"))
	{
		return true;
	}
	else if (is48 != NULL && (stringHasCaseInsensitivePrefix(filename, "48/") ||
		stringHasCaseInsensitivePrefix(filename, "48\\")))
	{
		*is48 = true;
		return isTorus(filename + 3, NULL);
	}
	else
	{
		return false;
	}
}

bool LDLPrimitiveCheck::isTorusO(const char *filename, bool *is48)
{
	if (isTorus(filename, is48))
	{
		if (is48 != NULL && *is48)
		{
			return toupper(filename[6]) == 'O';
		}
		else
		{
			return toupper(filename[3]) == 'O';
		}
	}
	else
	{
		return false;
	}
}

bool LDLPrimitiveCheck::isTorusI(const char *filename, bool *is48)
{
	if (isTorus(filename, is48))
	{
		if (is48 != NULL && *is48)
		{
			return toupper(filename[6]) == 'I';
		}
		else
		{
			return toupper(filename[3]) == 'I';
		}
	}
	else
	{
		return false;
	}
}

bool LDLPrimitiveCheck::isTorusQ(const char *filename, bool *is48)
{
	if (isTorus(filename, is48))
	{
		if (is48 != NULL && *is48)
		{
			return toupper(filename[6]) == 'Q';
		}
		else
		{
			return toupper(filename[3]) == 'Q';
		}
	}
	else
	{
		return false;
	}
}

TCFloat LDLPrimitiveCheck::getTorusFraction(int size)
{
	int i;

	for (i = 0; i < 10; i++)
	{
		if (size == i + i * 10 + i * 100 + i * 1000)
		{
			return (TCFloat)i / 9.0f;
		}
	}
	return (TCFloat)size / 10000.0f;
}

int LDLPrimitiveCheck::getUsedCircleSegments(int numSegments, TCFloat fraction)
{
	return (int)(numSegments * fraction + 0.000001);
}

int LDLPrimitiveCheck::getNumCircleSegments(TCFloat fraction, bool is48)
{
	int retValue = m_curveQuality * LO_NUM_SEGMENTS;

	if (is48 && retValue < 48)
	{
		retValue = 48;
	}
	if (fraction != 0.0f)
	{
		int i;
		
		for (i = m_curveQuality; !fEq(fraction * retValue,
			(TCFloat)getUsedCircleSegments(retValue, fraction)) && i < 12; i++)
		{
			int newValue = (i + 1) * LO_NUM_SEGMENTS;

			if (newValue > retValue)
			{
				retValue = newValue;
			}
		}
	}
	return retValue;
}

bool LDLPrimitiveCheck::performPrimitiveSubstitution(
	LDLModel *ldlModel,
	bool bfc)
{
	m_modelName = ldlModel->getName();

	if (getPrimitiveSubstitutionFlag())
	{
		bool is48;

		if (!m_modelName)
		{
			return false;
		}
		if (strcasecmp(m_modelName, "LDL-LOWRES:stu2.dat") == 0)
		{
			return substituteStu2();
		}
		else if (strcasecmp(m_modelName, "LDL-LOWRES:stu22.dat") == 0)
		{
			return substituteStu22(false, bfc);
		}
		else if (strcasecmp(m_modelName, "LDL-LOWRES:stu22a.dat") == 0)
		{
			return substituteStu22(true, bfc);
		}
		else if (strcasecmp(m_modelName, "LDL-LOWRES:stu23.dat") == 0)
		{
			return substituteStu23(false, bfc);
		}
		else if (strcasecmp(m_modelName, "LDL-LOWRES:stu23a.dat") == 0)
		{
			return substituteStu23(true, bfc);
		}
		else if (strcasecmp(m_modelName, "LDL-LOWRES:stu24.dat") == 0)
		{
			return substituteStu24(false, bfc);
		}
		else if (strcasecmp(m_modelName, "LDL-LOWRES:stu24a.dat") == 0)
		{
			return substituteStu24(true, bfc);
		}
		else if (strcasecmp(m_modelName, "stud.dat") == 0)
		{
			return substituteStud();
		}
		else if (strcasecmp(m_modelName, "1-8sphe.dat") == 0)
		{
			return substituteEighthSphere(bfc);
		}
		else if (strcasecmp(m_modelName, "48/1-8sphe.dat") == 0 ||
			strcasecmp(m_modelName, "48\\1-8sphe.dat") == 0)
		{
			return substituteEighthSphere(bfc, true);
		}
		else if (strcasecmp(m_modelName, "1-8sphc.dat") == 0)
		{
			return substituteEighthSphereCorner(bfc);
		}
		else if (strcasecmp(m_modelName, "48/1-8sphc.dat") == 0 ||
			strcasecmp(m_modelName, "48\\1-8sphc.dat") == 0)
		{
			return substituteEighthSphereCorner(bfc, true);
		}
		else if (isCyli(m_modelName, &is48))
		{
			return substituteCylinder(startingFraction(m_modelName),
				bfc, is48);
		}
		else if (isCyls(m_modelName, &is48))
		{
			return substituteSlopedCylinder(startingFraction(m_modelName), bfc,
				is48);
		}
		else if (isCyls2(m_modelName, &is48))
		{
			return substituteSlopedCylinder2(startingFraction(m_modelName), bfc,
				is48);
		}
		else if (isChrd(m_modelName, &is48))
		{
			return substituteChrd(startingFraction(m_modelName), bfc,
				is48);
		}
		else if (isDisc(m_modelName, &is48))
		{
			return substituteDisc(startingFraction(m_modelName), bfc,
				is48);
		}
		else if (isNdis(m_modelName, &is48))
		{
			return substituteNotDisc(startingFraction(m_modelName),
				bfc, is48);
		}
		else if (isEdge(m_modelName, &is48))
		{
			return substituteCircularEdge(startingFraction(m_modelName), is48);
		}
		else if (isCon(m_modelName, &is48))
		{
			int size;
			int offset = 0;

			if (is48)
			{
				offset = 3;
			}
			sscanf(m_modelName + 6 + offset, "%d", &size);
			return substituteCone(startingFraction(m_modelName), size,
				bfc, is48);
		}
		else if (isOldRing(m_modelName, &is48))
		{
			int size;
			int offset = 0;

			if (is48)
			{
				offset = 3;
			}
			sscanf(m_modelName + 4 + offset, "%d", &size);
			return substituteRing(1.0f, size, bfc, is48, true);
		}
		else if (isRing(m_modelName, &is48))
		{
			int size;
			int offset = 0;

			if (is48)
			{
				offset = 3;
			}
			sscanf(m_modelName + 7 + offset, "%d", &size);
			return substituteRing(startingFraction(m_modelName), size,
				bfc, is48);
		}
		else if (isRin(m_modelName, &is48))
		{
			int size;
			int offset = 0;

			if (is48)
			{
				offset = 3;
			}
			sscanf(m_modelName + 6 + offset, "%d", &size);
			return substituteRing(startingFraction(m_modelName), size,
				bfc, is48);
		}
		else if (isTorus(m_modelName, &is48))
		{
			int size;
			TCFloat fraction;
			int offset = 0;

			if (is48)
			{
				offset = 3;
			}
			sscanf(m_modelName + 1 + offset, "%d", &m_filenameNumerator);
			m_filenameDenom = 16;
			sscanf(m_modelName + 4 + offset, "%d", &size);
			fraction = (TCFloat)m_filenameNumerator / (TCFloat)m_filenameDenom;
			if (isTorusO(m_modelName, &is48))
			{
				return substituteTorusIO(false, fraction, size, bfc, is48);
			}
			else if (isTorusI(m_modelName, &is48))
			{
				return substituteTorusIO(true, fraction, size, bfc, is48);
			}
			else if (isTorusQ(m_modelName, &is48))
			{
				return substituteTorusQ(fraction, size, bfc, is48);
			}
		}
	}
	if (getNoLightGeomFlag())
	{
		if (m_modelName && strcasecmp(m_modelName, "light.dat") == 0)
		{
			// Don't draw any geometry for light.dat.
			return true;
		}
	}
	return false;
}

