#ifndef __TRESUBMODEL_H__
#define __TRESUBMODEL_H__

#include <TCFoundation/TCObject.h>

class TREModel;

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
	virtual void setColor(TCULong color);
	virtual TCULong getColor(void);
	virtual bool isColorSet(void) { return m_colorSet; }
	virtual void draw(void);
	virtual void drawColored(void);
	virtual void drawDefaultColor(void);
protected:
	virtual ~TRESubModel(void);
	virtual void dealloc(void);

	TREModel *m_model;
	float m_matrix[16];
	TCULong m_color;
	bool m_colorSet;
};

#endif // __TRESUBMODEL_H__
