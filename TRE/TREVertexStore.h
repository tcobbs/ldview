#ifndef __TREVERTEXSTORE_H__
#define __TREVERTEXSTORE_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedValueArray.h>

struct TREVertex;
class TREVertexArray;
class Vector;

class TREVertexStore : public TCObject
{
public:
	TREVertexStore(void);
	TREVertexStore(const TREVertexStore &other);
	virtual bool activate(void);
	virtual int addVertices(Vector *points, int count);
	virtual int addVertices(TCULong color, Vector *points, int count);
	TCObject *copy(void);
	virtual void setup(void);
	virtual void setupColored(void);
	TREVertexArray *getVertices(void) { return m_vertices; }
	TREVertexArray *getNormals(void) { return m_normals; }
	TCULongArray *getColors(void) { return m_colors; }
	static void initVertex(TREVertex &vertex, Vector &point);
	static Vector calcNormal(Vector *points, bool normalize = true);
protected:
	virtual ~TREVertexStore(void);
	virtual void dealloc(void);
	virtual int addVertices(TREVertexArray *vertices, Vector *points,
		int count);

	TREVertexArray *m_vertices;
	TREVertexArray *m_normals;
	TCULongArray *m_colors;
	static TREVertexStore *sm_activeVertexStore;
};

#endif // __TREVERTEXSTORE_H__
