#ifndef __TREMODEL_H__
#define __TREMODEL_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedObjectArray.h>

class TRESubModel;
class TREMainModel;
class TREShapeGroup;
class TREColoredShapeGroup;
class Vector;

typedef TCTypedObjectArray<TRESubModel> TRESubModelArray;

class TREModel : public TCObject
{
public:
	TREModel(void);
	TREModel(const TREModel &other);
	TCObject *copy(void);
	virtual void setMainModel(TREMainModel *mainModel)
	{
		m_mainModel = mainModel;
	}
	virtual TREMainModel *getMainModel(void) const { return m_mainModel; }
	virtual void setName(const char *name);
	virtual const char *getName(void) const { return m_name; }
	virtual TREShapeGroup *getShapes(void) { return m_shapes; }
//	virtual TREVertexArray *getVertices(void) { return m_vertices; }
	virtual void addSubModel(float *matrix, TREModel *model);
	virtual void addSubModel(TCULong color, float *matrix, TREModel *model);
	virtual void addTriangle(Vector *vertices);
	virtual void addTriangle(TCULong color, Vector *vertices);
	virtual void addQuad(Vector *vertices);
	virtual void addQuad(TCULong color, Vector *vertices);
	virtual void draw(float *matrix);
protected:
	virtual ~TREModel(void);
	virtual void dealloc(void);
	virtual void setup(void);
	virtual void setupColored(void);

	char *m_name;
	TREMainModel *m_mainModel;
	TRESubModelArray *m_subModels;
	TREShapeGroup *m_shapes;
	TREColoredShapeGroup *m_coloredShapes;
};

#endif // __TREMODEL_H__
