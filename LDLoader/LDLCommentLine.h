#ifndef __LDLCOMMENTLINE_H__
#define __LDLCOMMENTLINE_H__

#include <LDLoader/LDLFileLine.h>

class LDLCommentLine : public LDLFileLine
{
public:
	TCObject *copy(void);
	virtual bool parse(void);
	virtual LDLLineType getLineType(void) const { return LDLLineTypeComment; }
	virtual bool getMPDFilename(char *filename, int maxLength) const;
	virtual bool isPartMeta(void) const;
	virtual bool isBFCMeta(void) const;
	virtual bool containsBFCCommand(const char *command) const;
protected:
	LDLCommentLine(LDLModel *parentModel, const char *line, int lineNumber,
		const char *originalLine = NULL);
	LDLCommentLine(const LDLCommentLine &other);
	~LDLCommentLine(void);
	virtual void dealloc(void);
	void setupProcessedLine(void);

	char *m_processedLine;
	TCStringArray *m_words;

	friend class LDLFileLine; // Needed because constructors are protected.
};

#endif // __LDLCOMMENTLINE_H__
