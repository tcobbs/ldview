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
	bool getNonUniformFlag(void) { return m_flags.nonUniform != false; }
	virtual void print(int indent) const;
	virtual LDLLineType getLineType(void) const { return LDLLineTypeModel; }
	virtual bool isXZPlanar(void) const;
	virtual bool isXZPlanar(const float *matrix) const;
protected:
	LDLModelLine(LDLModel *parentModel, const char *line, int lineNumber,
		const char *originalLine = NULL);
	LDLModelLine(const LDLModelLine &other);
	virtual ~LDLModelLine(void);
	virtual void dealloc(void);
	virtual void setTransformation(float x, float y, float z,
		float a, float b, float c,
		float d, float e, float f,
		float g, float h, float i);
	virtual float tryToFixPlanarMatrix(void);

	LDLModel *m_highResModel;
	LDLModel *m_lowResModel;
	float m_matrix[16];
	TCULong m_color;
	int m_colorNumber;
	struct
	{
		// Private flags
		bool nonUniform:1;
	} m_flags;

	friend class LDLFileLine; // Needed because constructors are protected.
};

#endif // __LDLMODELLINE_H__
