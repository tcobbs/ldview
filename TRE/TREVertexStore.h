#ifndef __TREVERTEXSTORE_H__
#define __TREVERTEXSTORE_H__

#include <TCFoundation/TCObject.h>
#include <TCFoundation/TCTypedValueArray.h>
#include <TRE/TREGL.h>

struct TREVertex;
class TREVertexArray;
class TCVector;

class TREVertexStore : public TCObject
{
public:
	TREVertexStore(void);
	TREVertexStore(const TREVertexStore &other);
	virtual TCObject *copy(void);
	virtual bool activate(void);
	virtual int addVertices(TCVector *points, int count);
	virtual int addVertices(TCVector *points, TCVector *normals, int count);
	virtual int addVertices(TCULong color, TCVector *points, int count);
	virtual int addVertices(TCULong color, TCVector *points, TCVector *normals,
		int count);
	virtual void setup(void);
	virtual void setupColored(void);
	TREVertexArray *getVertices(void) { return m_vertices; }
	TREVertexArray *getNormals(void) { return m_normals; }
	TCULongArray *getColors(void) { return m_colors; }
	void setTwoSidedLightingFlag(bool value)
	{
		m_flags.twoSidedLighting = value;
	}
	bool getTwoSidedLightingFlag(void) { return m_flags.twoSidedLighting; }

	static void initVertex(TREVertex &vertex, TCVector &point);
	static TCVector calcNormal(TCVector *points, bool normalize = true);
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
	virtual int addVertices(TREVertexArray *vertices, TCVector *points,
		int count);
	virtual void setupVAR(void);

	TREVertexArray *m_vertices;
	TREVertexArray *m_normals;
	TCULongArray *m_colors;
	TCULong m_varVerticesOffset;
	TCULong m_varNormalsOffset;
	TCULong m_varColorsOffset;
	struct
	{
		bool varTried:1;
		bool varFailed:1;
		bool twoSidedLighting:1;
	} m_flags;

	static TREVertexStore *sm_activeVertexStore;
	static PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV;
	static PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV;
	static PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;
	static TCByte *sm_varBuffer;
	static int sm_varSize;
};

#endif // __TREVERTEXSTORE_H__
