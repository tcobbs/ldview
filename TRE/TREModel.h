#ifndef __TREMODEL_H__
#define __TREMODEL_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedObjectArray.h>

struct TREVertex;
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
	virtual void draw(void);
	virtual void setPart(bool part) { m_flags.part = part; }
	virtual bool isPart(void) { return m_flags.part; }
	virtual void flatten(void);

	static void multMatrix(float* left, float* right, float* result);
	static void transformVertex(TREVertex &vertex, float *matrix);
	static void transformNormal(TREVertex &normal, float *matrix);
	static float determinant(float* matrix);
	static void invertMatrix(float* matrix, float* inverseMatrix);
protected:
	virtual ~TREModel(void);
	virtual void dealloc(void);
	virtual void setup(void);
	virtual void setupColored(void);
	virtual void flatten(TREModel *model, float *matrix, TCULong color,
		bool colorSet, bool includeShapes);
	virtual void flattenShapes(TREShapeGroup *dstShapes,
		TREShapeGroup *srcShapes, float *matrix, TCULong color, bool colorSet);
	static void glNormalize(bool value);

	char *m_name;
	TREMainModel *m_mainModel;
	TRESubModelArray *m_subModels;
	TREShapeGroup *m_shapes;
	TREColoredShapeGroup *m_coloredShapes;
	struct
	{
		bool part:1;
	} m_flags;

	static bool sm_normalizeOn;
};

#endif // __TREMODEL_H__
