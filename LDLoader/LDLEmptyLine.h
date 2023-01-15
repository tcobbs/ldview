#ifndef __LDLEMPTYLINE_H__
#define __LDLEMPTYLINE_H__

#include <LDLoader/LDLFileLine.h>

class LDLEmptyLine : public LDLFileLine
{
public:
	virtual TCObject *copy(void) const;
	virtual bool parse(void);
	virtual LDLLineType getLineType(void) const { return LDLLineTypeEmpty; }
	virtual void updateStatistics(LDLStatistics& statistics) const
	{
		++statistics.empties;
	}
protected:
	LDLEmptyLine(LDLModel *parentModel, const char *line, size_t lineNumber,
		const char *originalLine = NULL);
	LDLEmptyLine(const LDLEmptyLine &other);
	virtual ~LDLEmptyLine(void);

	friend class LDLFileLine; // Needed because constructors are protected.
};

#endif // __LDLEMPTYLINE_H__
