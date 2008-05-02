#ifndef __LDLCOMMENTLINE_H__
#define __LDLCOMMENTLINE_H__

#include <LDLoader/LDLFileLine.h>

class LDLCommentLine : public LDLFileLine
{
public:
	// BI
	enum BICommand
	{
		BICERROR,
		BICSet,
		BICUnset,
		BICNext,
		BICStart,
		BICEnd,
	};
	// /BI

	virtual TCObject *copy(void) const;
	virtual bool parse(void);
	virtual LDLLineType getLineType(void) const { return LDLLineTypeComment; }
	virtual bool getMPDFilename(char *filename, int maxLength) const;
	virtual bool isPartMeta(void) const;
	virtual bool isOfficialPartMeta(bool partMeta) const;
	virtual bool isPrimitiveMeta(void) const;
	virtual bool isNoShrinkMeta(void) const;
	virtual bool isBFCMeta(void) const;

	// BI
	bool isBIMeta(void) const;
	BICommand getBICommand(void) const;
	bool hasBIConditional(void) const;
	bool getBIConditional(void) const;
	bool hasBIToken(void) const;
	const char *getBIToken(void) const;
	int getBIColorNumber(void) const;
	// BI

	virtual bool isMovedToMeta(void) const;
	virtual bool isStepMeta(void) const;
	virtual bool getAuthor(char *author, int maxLength) const;
	virtual bool containsBFCCommand(const char *command) const;
	virtual const char *getProcessedLine(void) const
	{
		return m_processedLine;
	}
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
