/*
 ******************************************************************************
 * SimStarIV
 * Travis Cobbs, Chris Connley, Rusty Larner
 * CSC 480
 *
 * FILE:  TCVector.h
 ******************************************************************************
 */
#ifndef __TCVECTOR_H__
#define __TCVECTOR_H__

#include <stdio.h>
//#include <windows.h>
//#include <GL/gl.h>

// All overload * operators with 2 vectors are cross product.
// dot product is not an overloaded operator, simply dot.

typedef float* GlPt;

class TCVector
{
	public:
		// Constructors
		TCVector(void);
		TCVector(float, float, float);
		TCVector(const TCVector&);

		// Destructor
		~TCVector(void);

		// Member Functions
		void print(FILE* = stdout) const;
		void print(char* buffer, int precision = 3) const;
		float length(void) const;
		float lengthSquared(void) const;
		float dot(const TCVector&) const;
		TCVector& normalize(void) {return *this *= 1.0f/length();}
		float get(int i) const { return vector[i];}
		bool approxEquals(const TCVector &right, float epsilon) const;

		// Overloaded Operators
		TCVector operator*(const TCVector&) const;
		TCVector operator*(float) const;
		TCVector operator/(float) const;
		TCVector operator+(const TCVector&) const;
		TCVector operator-(const TCVector&) const;
		TCVector operator-(void) const;
		TCVector& operator*=(const TCVector&);
		TCVector& operator*=(float);
		TCVector& operator/=(float);
		TCVector& operator+=(const TCVector&);
		TCVector& operator-=(const TCVector&);
		TCVector& operator=(const TCVector&);
		int operator==(const TCVector&) const;
		int operator!=(const TCVector&) const;
		int operator<(const TCVector& right) const;
		int operator>(const TCVector& right) const;
		int operator<=(const TCVector& right) const;
		int operator>=(const TCVector& right) const;
		float& operator[](int i) {return vector[i];}
		operator GlPt(void) {return vector;}
		TCVector mult(float* matrix) const;
		TCVector mult2(float* matrix) const;

		static float determinant(float *matrix);
		static void multMatrix(float*, float*, float*);
		static void multMatrixd(double*, double*, double*);
		static void invertMatrix(float*, float*);
		static void initIdentityMatrix(float*);
	protected:
#ifdef _LEAK_DEBUG
		char className[4];
#endif
		float vector[3];
};

// Overloaded Operator with non-TCVector as first argument
TCVector operator*(float, const TCVector&);

#endif // __TCVECTOR_H__
