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
	virtual bool activate(bool displayLists);
	virtual void deactivate(void);
	virtual int addVertices(const TCVector *points, int count);
	virtual int addControlPoints(const TCVector *points, int count);
	virtual int addVertices(const TCVector *points, const TCVector *normals,
		int count);
	virtual int addVertices(TCULong color, const TCVector *points, int count);
	virtual int addVertices(TCULong color, const TCVector *points,
		const TCVector *normals, int count);
	virtual void setup(void);
	virtual void setupColored(void);
	virtual void setupConditional(void);
	TREVertexArray *getVertices(void) { return m_vertices; }
	TREVertexArray *getNormals(void) { return m_normals; }
	TREVertexArray *getControlPoints(void) { return m_controlPoints; }
	TCULongArray *getColors(void) { return m_colors; }
	void setLightingFlag(bool value);
	bool getLightingFlag(void) { return m_flags.lighting != false; }
	void setTwoSidedLightingFlag(bool value)
	{
		m_flags.twoSidedLighting = value;
	}
	bool getTwoSidedLightingFlag(void)
	{
		return m_flags.twoSidedLighting != false;
	}

	static void initVertex(TREVertex &vertex, const TCVector &point);
	static TCVector calcNormal(const TCVector *points, bool normalize = true);
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
	static void setGlBindBufferARB(PFNGLBINDBUFFERARBPROC value)
	{
		glBindBufferARB = value;
	}
	static void setGlDeleteBuffersARB(PFNGLDELETEBUFFERSARBPROC value)
	{
		glDeleteBuffersARB = value;
	}
	static void setGlGenBuffersARB(PFNGLGENBUFFERSARBPROC value)
	{
		glGenBuffersARB = value;
	}
	static void setGlBufferDataARB(PFNGLBUFFERDATAARBPROC value)
	{
		glBufferDataARB = value;
	}
protected:
	virtual ~TREVertexStore(void);
	virtual void dealloc(void);
	virtual int addVertices(TREVertexArray *vertices, const TCVector *points,
		int count);
	virtual void setupVAR(void);
	virtual void setupVBO(void);

	TREVertexArray *m_vertices;
	TREVertexArray *m_normals;
	TREVertexArray *m_controlPoints;
	TCULongArray *m_colors;
	TCULong m_verticesOffset;
	TCULong m_normalsOffset;
	TCULong m_colorsOffset;
	GLuint m_vbo;
	struct
	{
		bool varTried:1;
		bool varFailed:1;
		bool vboTried:1;
		bool vboFailed:1;
		bool lighting:1;
		bool twoSidedLighting:1;
	} m_flags;

	static TREVertexStore *sm_activeVertexStore;

	static PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV;
	static PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV;
	static PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV;

	static PFNGLBINDBUFFERARBPROC glBindBufferARB;
	static PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
	static PFNGLGENBUFFERSARBPROC glGenBuffersARB;
	static PFNGLBUFFERDATAARBPROC glBufferDataARB;

	static TCByte *sm_varBuffer;
	static int sm_varSize;
};

#endif // __TREVERTEXSTORE_H__
