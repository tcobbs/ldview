#ifndef __LDLMODELLINE_H__
#define __LDLMODELLINE_H__

#include <LDLoader/LDLFileLine.h>
#include <LDLoader/LDLModel.h>

class LDLModelLine : public LDLFileLine
{
public:
	virtual TCObject *copy(void);
	virtual bool parse(void);
	virtual LDLModel *getModel(void) const;
	float *getTransformationMatrix(void) { return m_transformationMatrix; }
	TCULong getColor(void) { return m_color; }
	int getColorNumber(void) { return m_colorNumber; }
	virtual void print(int indent);
	virtual LDLLineType getLineType(void) const { return LDLLineTypeModel; }
	virtual void setBFCSettings(BFCState bfcCertify, bool bfcClip,
		bool bfcWindingCCW, bool bfcInvert);
protected:
	LDLModelLine(LDLModel *mainModel, const char *line, int lineNumber);
	LDLModelLine(const LDLModelLine &other);
	virtual ~LDLModelLine(void);
	virtual void dealloc(void);
	virtual void setTransformation(float x, float y, float z,
		float a, float b, float c,
		float d, float e, float f,
		float g, float h, float i);

	LDLModel *m_highResModel;
	LDLModel *m_lowResModel;
	float m_transformationMatrix[16];
	TCULong m_color;
	int m_colorNumber;
	struct
	{
		// Public flags
		BFCState bfcCertify:2;
		bool bfcClip:1;
		bool bfcWindingCCW:1;
		bool bfcInvert:1;
	} m_flags;

	friend LDLFileLine; // Needed because constructors are protected.
};

#endif __LDLMODELLINE_H__
