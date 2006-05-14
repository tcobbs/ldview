#ifndef __TRETRANSSHAPEGROUP_H__
#define __TRETRANSSHAPEGROUP_H__

#include <TRE/TREColoredShapeGroup.h>
#include <TCFoundation/TCTypedObjectArray.h>

class TRESortedTriangle: public TCObject
{
public:
	TCULong indices[3];
	TCVector center;
	TCFloat depth;
};

typedef TCTypedObjectArray<TRESortedTriangle> TRESortedTriangleArray;

namespace boost
{
class thread;
}

// A shape group for handling sorted transparent shapes.
class TRETransShapeGroup: public TREColoredShapeGroup
{
public:
	TRETransShapeGroup(void);
	TRETransShapeGroup(const TRETransShapeGroup &other);
	virtual void draw(bool sort);
	virtual void sortInBackground(bool sort);
protected:
	~TRETransShapeGroup(void);
	virtual void dealloc(void);
	virtual void sortShapes(void);
	virtual void initSortedTriangles(void);
	virtual void backgroundSort(void);

	TRESortedTriangleArray *m_sortedTriangles;
	bool m_useSortThread;
	boost::thread *m_sortThread;
	TCFloat m_sortMatrix[16];
};

#endif // __TRETRANSSHAPEGROUP_H__

