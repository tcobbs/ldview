#ifndef __TREVERTEXSTORE_H__
#define __TREVERTEXSTORE_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedValueArray.h>
#include <TRE/TREGL.h>

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
	static void setWglAllocateMemoryNV(PFNWGLALLOCATEMEMORYNVPROC value)
	{
		wglAllocateMemoryNV = value;
	}
	static void setWglFreeMemoryNV(PFNWGLFREEMEMORYNVPROC value)
	{
		wglFreeMemoryNV = value;
	}
	static void setGlVertexArrayRangeNV(PFNGLVERTEXARRAYRANGENVPROC value)
	{
		glVertexArrayRangeNV = value;
	}
protected:
	virtual ~TREVertexStore(void);
	virtual void dealloc(void);
	virtual int addVertices(TREVertexArray *vertices, Vector *points,
		int count);
	virtual void setupVAR(void);

	TREVertexArray *m_vertices;
	TREVertexArray *m_normals;
	TCULongArray *m_colors;
	TCULong m_varVerticesOffset;
	TCULong m_varNormalsOffset;
	TCULong m_varColorsOffset;
	bool m_varTried;
	bool m_varFailed;

	static TREVertexStore *sm_activeVertexStore;
	static PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV;
	static PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV;
	static PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;
	static TCByte *sm_varBuffer;
	static int sm_varSize;
};

#endif // __TREVERTEXSTORE_H__
