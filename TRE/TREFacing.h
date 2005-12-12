#ifndef __TREFACING_H__
#define __TREFACING_H__

#include <TCFoundation/TCVector.h>

typedef enum Axis {XAxis, YAxis, ZAxis} SimAxis;

class TREFacing: public TCVector
{
	private:
		static float glMatrix[16];
	public:
		TREFacing(void);
		TREFacing(TCVector& a, float phi);
		~TREFacing(void) {}

		// this creates a gl-compatible 4x4 matrix that you can use to
		// do rotations
		float* getMatrix(void);

		// this gets the inverse matrix -- it _should_ rotate away for
		//		the object.  (This might be used in camera translations)
		void getInverseMatrix(float *inverseMatrix);

		// setFacing sets a vector to be a rotation around vector v by phi 
		//		radians.
		void setFacing(TCVector& v, float phi);

		// this multiplies two facings together.
		TREFacing operator+(const TREFacing& otherFacing);

		// normalize a facing.  Facings should automatically be
		//		normalized, but the user might want to call this every
		//		so often to get rid of floating point round off.
		TREFacing& normalize(void);

		// getVector returns a vector pointing in the direction of the facing.
		//		< 0, 0, 1 > is asumed to be a rotation of 0.
		TCVector getVector(void);

		float getRotation(void) { return rotation; }

		// this gets the angle between two facings, ignoring the top of head.
		//		Not fully tested at this time.
		float angleBetween(TREFacing &f2);

		// this returns the inverse facing.
		TREFacing inverse(void);

		// mult multiplies two facings together.  You get the same
		//		result as if you multiplied the two rotation matricies
		//		together.
		TREFacing mult(const TREFacing& f2);

		// I dont' know why dot returns a facing -- this was in 
		//		the code I grabbed from graphicsGems.  Don't try this
		//		before checking the answer.
		TREFacing dot(TREFacing& f2);

		// This negates a facing.  I don't know the difference geometrically
		//		between a negate and an inverse -- but there is a difference.
		TREFacing negate(void);

		// this THEORETICALLY works.  Give it a vector, and this SHOULD
		//		make the current rotation point at the vector, maintaining
		//		the top of head.  Note, SHOULD.  I tried testing, and
		//		this seemed to work.
		void pointAt(TCVector &v2);

		// this function returns the difference between two facings
		TCVector difference(TREFacing from);

		// this prints a facing's data.  Use mostly for bug testing.
		void print(FILE* = stdout);

	protected:
		float* invertMatrix(float*);
		void swapMatrixRows(float*, int, int);
		// this is the 4th number in the 4 float array of a quaternion.
		// (the other 3 are in the private vector superclass.
		float rotation;
};

#endif // __TREFACING_H__
