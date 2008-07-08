#ifndef __LDLMODELLINE_H__
#define __LDLMODELLINE_H__

#include <LDLoader/LDLActionLine.h>

class LDLModelLine : public LDLActionLine
{
public:
	virtual TCObject *copy(void) const;
	virtual bool parse(void);
	virtual LDLModel *getModel(bool forceHighRes = false) const;
	TCFloat *getMatrix(void) { return m_matrix; }
	bool getNonUniformFlag(void) { return m_flags.nonUniform != false; }
	virtual void print(int indent) const;
	virtual LDLLineType getLineType(void) const { return LDLLineTypeModel; }
	virtual bool isXZPlanar(void) const;
	virtual bool isXZPlanar(const TCFloat *matrix) const;
	virtual void scanPoints(TCObject *scanner,
		LDLScanPointCallback scanPointCallback, const TCFloat *matrix) const;
protected:
	LDLModelLine(LDLModel *parentModel, const char *line, int lineNumber,
		const char *originalLine = NULL);
	LDLModelLine(const LDLModelLine &other);
	virtual ~LDLModelLine(void);
	virtual void dealloc(void);
	virtual void setTransformation(TCFloat x, TCFloat y, TCFloat z,
		TCFloat a, TCFloat b, TCFloat c,
		TCFloat d, TCFloat e, TCFloat f,
		TCFloat g, TCFloat h, TCFloat i);
	virtual TCFloat tryToFixPlanarMatrix(void);
	virtual void fixLine(void);

	LDLModel *m_highResModel;
	LDLModel *m_lowResModel;
	TCFloat m_matrix[16];
	struct
	{
		// Private flags
		bool nonUniform:1;
	} m_flags;

	friend class LDLFileLine; // Needed because constructors are protected.
};

#endif // __LDLMODELLINE_H__
