//#include <math.h>
//#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include "TCVector.h"
#include "TCMacros.h"
#include "mystring.h"
//#include "simMacros.h"

//static int vectorCount = 0;

// TCVector::TCVector(void) -- Default Constructor
TCVector::TCVector(void)
{
#ifdef _LEAK_DEBUG
	className[0] = 'V';
	className[1] = 'e';
	className[2] = 'c';
	className[3] = 0;
#endif
	vector[0] = (float)0.0;
	vector[1] = (float)0.0;
	vector[2] = (float)0.0;
//	vectorCount++;
}

// TCVector::TCVector(float, float, float) -- Constructor
// Expects:
//		x, y, z	: The coordinates to store in the TCVector
TCVector::TCVector(float x, float y, float z)
{
#ifdef _LEAK_DEBUG
	className[0] = 'V';
	className[1] = 'e';
	className[2] = 'c';
	className[3] = 0;
#endif
	vector[0] = x;
	vector[1] = y;
	vector[2] = z;
//	vectorCount++;
}

// TCVector::TCVector(const TCVector&) -- Copy Constructor
// Expects:
//		v	: The TCVector to copy.
TCVector::TCVector(const TCVector& v)
{
#ifdef _LEAK_DEBUG
	className[0] = 'V';
	className[1] = 'e';
	className[2] = 'c';
	className[3] = 0;
#endif
	vector[0] = v.vector[0];
	vector[1] = v.vector[1];
	vector[2] = v.vector[2];
//	vectorCount++;
}

TCVector::~TCVector(void)
{
//	vectorCount--;
//	if (vectorCount < 10 || vectorCount % 1000 == 0)
//	{
//		printf("~TCVector: %d\n", vectorCount);
//	}
}

// TCVector::length(void) -- Member Function
// Returns:
//		The length of the TCVector.
float TCVector::length(void) const
{
	return (float)sqrt(sqr(vector[0]) + sqr(vector[1]) + sqr(vector[2]));
}

// TCVector::dot(const TCVector&) -- Member Function
// Expects:
//		right	: The right hand side of the dot product.
// Returns:
//		The dot product of "*this" dot "right".
float TCVector::dot(const TCVector& right) const
{
	return vector[0] * right.vector[0] + vector[1] * right.vector[1] +
			 vector[2] * right.vector[2];
}

// TCVector::operator*(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the cross product.
// Returns:
//		The cross product of "*this" cross "right".
TCVector TCVector::operator*(const TCVector& right) const
{
	// <a, b, c> x <d, e, f> = <bf-ce, cd-af, ae-bd>
	// <x1, y1, z1> x <x2, y2, z2> = <y1*z2-y2*z1, z1*x2-z2*x1, x1*y2-x2*y1>
	return TCVector(vector[1] * right.vector[2] - vector[2] * right.vector[1],
					  vector[2] * right.vector[0] - vector[0] * right.vector[2],
					  vector[0] * right.vector[1] - vector[1] * right.vector[0]);
}

// TCVector::operator*(float) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the scalar multiply.
// Returns:
//		The TCVector multiplied by the scalar "right".
TCVector TCVector::operator*(float right) const
{
	return TCVector(vector[0]*right, vector[1]*right, vector[2]*right);
}

// TCVector::operator/(float) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the scalar divide.
// Returns:
//		The TCVector divided by the scalar "right".
TCVector TCVector::operator/(float right) const
{
	float mult = 1.0f / right;

	return TCVector(vector[0]*mult, vector[1]*mult, vector[2]*mult);
}

// TCVector::operator+(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the addition
//	Returns:
//		The TCVector added value "*this" + "right".
TCVector TCVector::operator+(const TCVector& right) const
{
	return TCVector(vector[0] + right.vector[0], vector[1] + right.vector[1],
					  vector[2] + right.vector[2]);
}

// TCVector::operator-(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the subtraction
//	Returns:
//		The TCVector subtracted value "*this" - "right".
TCVector TCVector::operator-(const TCVector& right) const
{
	return TCVector(vector[0] - right.vector[0], vector[1] - right.vector[1],
					  vector[2] - right.vector[2]);
}

// TCVector::operator-(void) -- Overloaded Operator
// Returns:
//		The TCVector -"*this".
// Results:
//		This is the unary minus operator
TCVector TCVector::operator-(void) const
{
	return TCVector(-vector[0], -vector[1], -vector[2]);
}

// TCVector::operator*=(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the cross product *=
// Returns:
//		*this, after it has been set to be the cross product of "*this" cross
//		"right"
TCVector& TCVector::operator*=(const TCVector& right)
{
	vector[0] = vector[1] * right.vector[2] - vector[2] * right.vector[1];
	vector[1] = vector[2] * right.vector[0] - vector[0] * right.vector[2];
	vector[2] = vector[0] * right.vector[1] - vector[1] * right.vector[0];
	return *this;
}

// TCVector::operator+=(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the +=
// Returns:
//		*this, after "right" has been added to it.
TCVector& TCVector::operator+=(const TCVector& right)
{
	vector[0] += right.vector[0];
	vector[1] += right.vector[1];
	vector[2] += right.vector[2];
	return *this;
}

// TCVector::operator-=(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the -=
// Returns:
//		*this, after "right" has been subtracted from it.
TCVector& TCVector::operator-=(const TCVector& right)
{
	vector[0] -= right.vector[0];
	vector[1] -= right.vector[1];
	vector[2] -= right.vector[2];
	return *this;
}

// TCVector::operator*=(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the scalar *=
// Returns:
//		*this, after it has been multiplied by the scalar "right".
TCVector& TCVector::operator*=(float right)
{
	vector[0] *= right;
	vector[1] *= right;
	vector[2] *= right;
	return *this;
}

// TCVector::operator/=(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the scalar /=
// Returns:
//		*this, after it has been divided by the scalar "right".
TCVector& TCVector::operator/=(float right)
{
	return *this *= 1.0f / right;
/*
	vector[0] /= right;
	vector[1] /= right;
	vector[2] /= right;
	return *this;
*/
}

// TCVector::operator=(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The vector copy the value of.
// Returns:
//		*this, which has the value of "right".
TCVector& TCVector::operator=(const TCVector& right)
{
	vector[0] = right.vector[0];
	vector[1] = right.vector[1];
	vector[2] = right.vector[2];
	return *this;
}

// TCVector::operator==(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the equality test.
// Returns:
//		If the contents of "*this" equal the contents of "right", then 1.
//		Otherwise, 0.  The contents are defined to be equal if they are within a
//		very small distance from each other.
int TCVector::operator==(const TCVector& right) const
{
	return fEq(vector[0], right.vector[0]) &&
			 fEq(vector[1], right.vector[1]) &&
			 fEq(vector[2], right.vector[2]);
}

// TCVector::operator!=(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the inequality test.
// Returns:
//		If the contents of "*this" do not equal the contents of "right", then 1.
//		Otherwise, 0.  The contents are defined to be inequal if they are within
//		a very small distance from each other.
int TCVector::operator!=(const TCVector& right) const
{
	return !(*this == right);
}

// TCVector::operator<(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the < test.
// Returns:
//		If the contents of "*this" < the contents of "right", then 1.  Otherwise,
//		0.
int TCVector::operator<(const TCVector& right) const
{
	return length() < right.length();
}

// TCVector::operator>(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the > test.
// Returns:
//		If the contents of "*this" > the contents of "right", then 1.  Otherwise,
//		0.
int TCVector::operator>(const TCVector& right) const
{
	return length() > right.length();
}

// TCVector::operator<=(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the <= test.
// Returns:
//		If the contents of "*this" <= the contents of "right", then 1.
//		Otherwise, 0.
int TCVector::operator<=(const TCVector& right) const
{
	return length() <= right.length();
}

// TCVector::operator>=(const TCVector&) -- Overloaded Operator
// Expects:
//		right	: The right hand side of the >= test.
// Returns:
//		If the contents of "*this" >= the contents of "right", then 1.
//		Otherwise, 0.
int TCVector::operator>=(const TCVector& right) const
{
	return length() >= right.length();
}

// TCVector::print(FILE*) -- Member Function
// Expects:
//		outFile	: File to output to.
// Results:
//		Prints the TCVector.  Does NOT print a trailing line feed.
void TCVector::print(FILE* outFile) const
{
	fprintf(outFile, "%f %f %f", vector[0], vector[1], vector[2]);
}

void TCVector::print(char* buffer) const
{
	sprintf(buffer, "%.3g %.3g %.3g", vector[0], vector[1], vector[2]);
}

// operator*(float, TCVector&) -- Overloaded Non-Member Operator
// Expects:
//		left	: The left hand side of the scalar multiply.
//		right	: The right hand side of the scalar multiply.
// Returns:
//		The TCVector resulting from "right" being multiplied by the scalar "left".
TCVector operator*(float left, const TCVector& right)
{
	return right * left;
}

TCVector TCVector::mult2(float* matrix) const
{
	TCVector tempVec;

	tempVec[0] = vector[0]*matrix[0] + vector[0]*matrix[1] +
	 vector[0]*matrix[2] + matrix[3];
	tempVec[1] = vector[1]*matrix[4] + vector[1]*matrix[5] +
	 vector[1]*matrix[6] + matrix[7];
	tempVec[2] = vector[2]*matrix[8] + vector[2]*matrix[9] +
	 vector[2]*matrix[10] + matrix[11];
	return tempVec;
}


TCVector TCVector::mult(float* matrix) const
{
	TCVector tempVec;

	tempVec[0] = vector[0]*matrix[0] + vector[1]*matrix[1] +
	 vector[2]*matrix[2] + matrix[3];
	tempVec[1] = vector[0]*matrix[4] + vector[1]*matrix[5] +
	 vector[2]*matrix[6] + matrix[7];
	tempVec[2] = vector[0]*matrix[8] + vector[1]*matrix[9] +
	 vector[2]*matrix[10] + matrix[11];
	return tempVec;
}

float TCVector::determinant(float* matrix)
{
	float det;

	det = matrix[0] * (matrix[5] * matrix[10] - matrix[6] * matrix[9]);
	det -= matrix[4] * (matrix[1] * matrix[10] - matrix[2] * matrix[9]);
	det += matrix[8] * (matrix[1] * matrix[6] - matrix[2] * matrix[5]);
	return det;
}

void TCVector::invertMatrix(float* matrix, float* inverseMatrix)
{
	float det = determinant(matrix);

	if (fEq(det, 0.0f))
	{
		debugPrintf("Error: Matrix inversion failed.\n");
	}
	else
	{
		det = 1.0f / det;

		inverseMatrix[0]  =  (matrix[5] * matrix[10] - matrix[6] * matrix[9]) *
			det;
		inverseMatrix[1]  = -(matrix[1] * matrix[10] - matrix[2] * matrix[9]) *
			det;
		inverseMatrix[2]  =  (matrix[1] * matrix[6]  - matrix[2] * matrix[5]) *
			det;
		inverseMatrix[4]  = -(matrix[4] * matrix[10] - matrix[6] * matrix[8]) *
			det;
		inverseMatrix[5]  =  (matrix[0] * matrix[10] - matrix[2] * matrix[8]) *
			det;
		inverseMatrix[6]  = -(matrix[0] * matrix[6]  - matrix[2] * matrix[4]) *
			det;
		inverseMatrix[8]  =  (matrix[4] * matrix[9]  - matrix[5] * matrix[8]) *
			det;
		inverseMatrix[9]  = -(matrix[0] * matrix[9]  - matrix[1] * matrix[8]) *
			det;
		inverseMatrix[10] =  (matrix[0] * matrix[5]  - matrix[1] * matrix[4]) *
			det;

		inverseMatrix[12] = -(matrix[12] * matrix[0] + matrix[13] * matrix[4] +
							matrix[14] * matrix[8]);
		inverseMatrix[13] = -(matrix[12] * matrix[1] + matrix[13] * matrix[5] +
							matrix[14] * matrix[9]);
		inverseMatrix[14] = -(matrix[12] * matrix[2] + matrix[13] * matrix[6] +
							matrix[14] * matrix[10]);

		inverseMatrix[3] = inverseMatrix[7] = inverseMatrix[11] = 0.0;
		inverseMatrix[15] = 1.0;
	}
}

void TCVector::multMatrix(float* left, float* right, float* result)
{
	result[0] = left[0] * right[0] + left[4] * right[1] +
		left[8] * right[2] + left[12] * right[3];
	result[1] = left[1] * right[0] + left[5] * right[1] +
		left[9] * right[2] + left[13] * right[3];
	result[2] = left[2] * right[0] + left[6] * right[1] +
		left[10] * right[2] + left[14] * right[3];
	result[3] = left[3] * right[0] + left[7] * right[1] +
		left[11] * right[2] + left[15] * right[3];
	result[4] = left[0] * right[4] + left[4] * right[5] +
		left[8] * right[6] + left[12] * right[7];
	result[5] = left[1] * right[4] + left[5] * right[5] +
		left[9] * right[6] + left[13] * right[7];
	result[6] = left[2] * right[4] + left[6] * right[5] +
		left[10] * right[6] + left[14] * right[7];
	result[7] = left[3] * right[4] + left[7] * right[5] +
		left[11] * right[6] + left[15] * right[7];
	result[8] = left[0] * right[8] + left[4] * right[9] +
		left[8] * right[10] + left[12] * right[11];
	result[9] = left[1] * right[8] + left[5] * right[9] +
		left[9] * right[10] + left[13] * right[11];
	result[10] = left[2] * right[8] + left[6] * right[9] +
		left[10] * right[10] + left[14] * right[11];
	result[11] = left[3] * right[8] + left[7] * right[9] +
		left[11] * right[10] + left[15] * right[11];
	result[12] = left[0] * right[12] + left[4] * right[13] +
		left[8] * right[14] + left[12] * right[15];
	result[13] = left[1] * right[12] + left[5] * right[13] +
		left[9] * right[14] + left[13] * right[15];
	result[14] = left[2] * right[12] + left[6] * right[13] +
		left[10] * right[14] + left[14] * right[15];
	result[15] = left[3] * right[12] + left[7] * right[13] +
		left[11] * right[14] + left[15] * right[15];
}

void TCVector::multMatrixd(double* left, double* right, double* result)
{
	result[0] = left[0] * right[0] + left[4] * right[1] +
		left[8] * right[2] + left[12] * right[3];
	result[1] = left[1] * right[0] + left[5] * right[1] +
		left[9] * right[2] + left[13] * right[3];
	result[2] = left[2] * right[0] + left[6] * right[1] +
		left[10] * right[2] + left[14] * right[3];
	result[3] = left[3] * right[0] + left[7] * right[1] +
		left[11] * right[2] + left[15] * right[3];
	result[4] = left[0] * right[4] + left[4] * right[5] +
		left[8] * right[6] + left[12] * right[7];
	result[5] = left[1] * right[4] + left[5] * right[5] +
		left[9] * right[6] + left[13] * right[7];
	result[6] = left[2] * right[4] + left[6] * right[5] +
		left[10] * right[6] + left[14] * right[7];
	result[7] = left[3] * right[4] + left[7] * right[5] +
		left[11] * right[6] + left[15] * right[7];
	result[8] = left[0] * right[8] + left[4] * right[9] +
		left[8] * right[10] + left[12] * right[11];
	result[9] = left[1] * right[8] + left[5] * right[9] +
		left[9] * right[10] + left[13] * right[11];
	result[10] = left[2] * right[8] + left[6] * right[9] +
		left[10] * right[10] + left[14] * right[11];
	result[11] = left[3] * right[8] + left[7] * right[9] +
		left[11] * right[10] + left[15] * right[11];
	result[12] = left[0] * right[12] + left[4] * right[13] +
		left[8] * right[14] + left[12] * right[15];
	result[13] = left[1] * right[12] + left[5] * right[13] +
		left[9] * right[14] + left[13] * right[15];
	result[14] = left[2] * right[12] + left[6] * right[13] +
		left[10] * right[14] + left[14] * right[15];
	result[15] = left[3] * right[12] + left[7] * right[13] +
		left[11] * right[14] + left[15] * right[15];
}

bool TCVector::approxEquals(const TCVector &right, float epsilon) const
{
	return fEq2(vector[0], right.vector[0], epsilon) &&
		fEq2(vector[1], right.vector[1], epsilon) &&
		fEq2(vector[2], right.vector[2], epsilon);
}
