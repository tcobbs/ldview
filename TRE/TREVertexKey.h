#ifndef __TREVERTEXKEY_H__
#define __TREVERTEXKEY_H__

//#include <TCFoundation/TCVector.h>
#include <TRE/TREVertexArray.h>

#define TRE_VERTEX_KEY_PRECISION 100.0f

class TREVertexKey
{
public:
	TREVertexKey(void)
	{
		x = 0;
		y = 0;
		z = 0;
	}
	TREVertexKey(const TREVertex &vertex)
	{
		x = (long)(vertex.v[0] * TRE_VERTEX_KEY_PRECISION);
		y = (long)(vertex.v[1] * TRE_VERTEX_KEY_PRECISION);
		z = (long)(vertex.v[2] * TRE_VERTEX_KEY_PRECISION);
	}
/*
	TREVertexKey(const TCVector &vector)
	{
		x = (long)(vector.get(0) * TRE_VECTOR_KEY_PRECISION);
		y = (long)(vector.get(1) * TRE_VECTOR_KEY_PRECISION);
		z = (long)(vector.get(2) * TRE_VECTOR_KEY_PRECISION);
	}
*/
	TREVertexKey(const TREVertexKey &other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}
	bool operator<(const TREVertexKey &other) const
	{
		if (x < other.x)
		{
			return true;
		}
		else if (x > other.x)
		{
			return false;
		}
		else if (y < other.y)
		{
			return true;
		}
		else if (y > other.y)
		{
			return false;
		}
		else if (z < other.z)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	TREVertexKey &operator=(const TREVertexKey &other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}
private:
	long x, y, z;
};

#endif // __TREVERTEXKEY_H__
