#ifndef __TRESUBMODEL_H__
#define __TRESUBMODEL_H__

#include <TCFoundation/TCObject.h>
#include <TRE/TREShapeGroup.h>

class TREModel;

class TCVector;

class TRESubModel : public TCObject
{
public:
	TRESubModel(void);
	TRESubModel(const TRESubModel &other);
	virtual TCObject *copy(void);
	virtual void setModel(TREModel *model);
	virtual TREModel *getModel(void) const { return m_model; }
	virtual void setMatrix(float *matrix);
	virtual float *getMatrix(void) { return m_matrix; }
	virtual void setColor(TCULong color, TCULong edgeColor);
	virtual TCULong getColor(void);
	virtual TCULong getEdgeColor(void);
	virtual bool isColorSet(void) { return m_colorSet; }
//	virtual void draw(void);
	virtual void drawColored(void);
	virtual void drawDefaultColor(void);
	virtual void drawDefaultColorLines(void);
	virtual void drawColoredLines(void);
	virtual void drawEdgeLines(void);
	virtual void drawColoredEdgeLines(void);
	virtual void getMinMax(TCVector& min, TCVector& max, float* matrix);
	virtual void getMaxRadiusSquared(const TCVector &center, float &rSquared,
		float *matrix);
	virtual void scanPoints(TCObject *scanner,
		TREScanPointCallback scanPointCallback, float *matrix);
	virtual void shrink(float amount);
protected:
	virtual ~TRESubModel(void);
	virtual void dealloc(void);

	TREModel *m_model;
	float m_matrix[16];
	TCULong m_color;
	TCULong m_edgeColor;
	bool m_colorSet;
};

#endif // __TRESUBMODEL_H__
