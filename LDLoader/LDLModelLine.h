#ifndef __LDLMODELLINE_H__
#define __LDLMODELLINE_H__

#include <LDLoader/LDLActionLine.h>

class LDLModelLine : public LDLActionLine
{
public:
	virtual TCObject *copy(void);
	virtual bool parse(void);
	virtual LDLModel *getModel(void) const;
	float *getMatrix(void) { return m_matrix; }
	TCULong getColor(void) { return m_color; }
	int getColorNumber(void) { return m_colorNumber; }
	virtual void print(int indent) const;
	virtual LDLLineType getLineType(void) const { return LDLLineTypeModel; }
protected:
	LDLModelLine(LDLModel *parentModel, const char *line, int lineNumber);
	LDLModelLine(const LDLModelLine &other);
	virtual ~LDLModelLine(void);
	virtual void dealloc(void);
	virtual bool setTransformation(float x, float y, float z,
		float a, float b, float c,
		float d, float e, float f,
		float g, float h, float i);

	LDLModel *m_highResModel;
	LDLModel *m_lowResModel;
	float m_matrix[16];
	TCULong m_color;
	int m_colorNumber;

	friend LDLFileLine; // Needed because constructors are protected.
};

#endif __LDLMODELLINE_H__
