#include "TREFacing.h"
#include <TCFoundation/TCMacros.h>
#include <string.h>
#include <TCFoundation/mystring.h>

float TREFacing::glMatrix[16] =
{
	(float)1.0, (float)0.0, (float)0.0, (float)0.0,
	(float)0.0, (float)1.0, (float)0.0, (float)0.0,
	(float)0.0, (float)0.0, (float)1.0, (float)0.0,
	(float)0.0, (float)0.0, (float)0.0, (float)1.0
};
static float emptyMatrix[] =
{
	(float)0.0, (float)0.0, (float)0.0, (float)0.0,
	(float)0.0, (float)0.0, (float)0.0, (float)0.0,
	(float)0.0, (float)0.0, (float)0.0, (float)0.0,
	(float)0.0, (float)0.0, (float)0.0, (float)0.0
};
static float identityMatrix[] =
{
	(float)1.0, (float)0.0, (float)0.0, (float)0.0,
	(float)0.0, (float)1.0, (float)0.0, (float)0.0,
	(float)0.0, (float)0.0, (float)1.0, (float)0.0,
	(float)0.0, (float)0.0, (float)0.0, (float)1.0
};

TREFacing::TREFacing(void)
	:TCVector((float)0.0, (float)0.0, (float)0.0)
{
	//TCVector tempVec(1, 0, 0);

	//setFacing(tempVec, 0);
	rotation = (float)1.0;
}

TREFacing::TREFacing(TCVector &a, float phi)
{
	setFacing(a, phi);
}

TREFacing TREFacing::operator+(const TREFacing& otherFacing) 
{
	return mult(otherFacing);
}

float TREFacing::angleBetween(TREFacing &f2)
{
  //t = (q[0]*r[0])+(q[1]*r[1])+(q[2]*r[2])+(q[3]*r[3]);
  return (this->vector[0]*f2.vector[0]) +
	  (this->vector[1]*f2.vector[1]) +
	  (this->vector[2]*f2.vector[2]) +
	  (this->rotation*f2.rotation);
}

TREFacing TREFacing::inverse(void)
{
	TREFacing newFacing;

	newFacing.vector[0] = -vector[0];
	newFacing.vector[1] = -vector[1];
	newFacing.vector[2] = -vector[2];
	newFacing.rotation = rotation;

	return newFacing;
}

TREFacing TREFacing::negate(void)
{
	TREFacing newFacing;

	newFacing.vector[0] = -vector[0];
	newFacing.vector[1] = -vector[1];
	newFacing.vector[2] = -vector[2];
//	newFacing.rotation = -rotation;

	return newFacing;
}

TREFacing TREFacing::mult(const TREFacing& f2)
{
	TREFacing temp;

	temp.vector[0] = this->rotation*f2.vector[0] +
		this->vector[0]*f2.rotation +
		this->vector[1]*f2.vector[2] -
		this->vector[2]*f2.vector[1];
	temp.vector[1] = this->rotation*f2.vector[1] +
		this->vector[1]*f2.rotation +
		this->vector[2]*f2.vector[0] -
		this->vector[0]*f2.vector[2];
	temp.vector[2] = this->rotation*f2.vector[2] +
		this->vector[2]*f2.rotation +
		this->vector[0]*f2.vector[1] -
		this->vector[1]*f2.vector[0];
	temp.rotation = this->rotation*f2.rotation -
		this->vector[0]*f2.vector[0] -
		this->vector[1]*f2.vector[1] -
		this->vector[2]*f2.vector[2];

	return temp;
}

TREFacing TREFacing::dot(TREFacing& f2)
{
	TREFacing answer;
	double temp;
	int i;

	answer.rotation = (this->rotation * f2.rotation) -
		((this->vector[0] * f2.vector[0]) +
		(this->vector[1] * f2.vector[1]) +
		(this->vector[2] * f2.vector[2]));
	answer.vector[0] = (this->rotation * f2.vector[0]) +
		(f2.rotation * this->vector[0]) +
		((this->vector[1] * f2.vector[2]) -
		(this->vector[2] * f2.vector[1]));
	answer.vector[1] = (this->rotation * f2.vector[1]) +
		(f2.rotation * this->vector[1]) +
		((this->vector[2] * f2.vector[0]) -
		(this->vector[0] * f2.vector[2]));
	answer.vector[2] = (this->rotation * f2.vector[2]) +
		(f2.rotation * this->vector[2]) +
		((this->vector[0] * f2.vector[1]) -
		(this->vector[1] * f2.vector[0]));

	/* make sure the resulting quaternion is a unit quaternion */
	temp = sqrt((double)(answer.vector[0]*answer.vector[0] +
		answer.vector[1]*answer.vector[1] +
		answer.vector[2]*answer.vector[2] +
		answer.rotation*answer.rotation));
	for (i=0; i<4; i++) 
	{
		answer.vector[i] = (float)(answer.vector[i]/temp);
	}
	return answer;
}

void TREFacing::setFacing(TCVector &a, float phi)
{
	double phiOver2 = phi / 2.0;

	a.normalize(); /* Normalize axis */
	//(TCVector)(*this) = a;
	(*this)[0] = a[0];
	(*this)[1] = a[1];
	(*this)[2] = a[2];

	//vscale(e, sin(phi/2.0));
	(*this)[0] *= (float)sin(phiOver2);
	(*this)[1] *= (float)sin(phiOver2);
	(*this)[2] *= (float)sin(phiOver2);

	this->rotation = (float)cos(phiOver2);
	//printf("TREFacing set to %f, %f, %f, %f\n", (*this)[0], (*this)[1], 
	// (*this)[2], this->rotation);
}

TREFacing& TREFacing::normalize(void)
{
	int which, i;
	float gr;

	which = 0;
	gr = (*this)[which];
	for (i = 1 ; i < 4 ; i++)
	{
		if (fabs((*this)[i]) > fabs(gr))
		{
			gr = (*this)[i];
			which = i;
		}
	}
	(*this)[which] = (float)0.0;   /* So it doesn't affect next operation */

	(*this)[which] = (float)(sqrt(1.0 - ((*this)[0]*(*this)[0] +
		(*this)[1]*(*this)[1] +
		(*this)[2]*(*this)[2] +
		this->rotation*this->rotation)));

	/* Check to see if we need negative square root */
	if (gr < 0.0)
	{
		(*this)[which] = -(*this)[which];
	}

	return *this;

}

float* TREFacing::getMatrix(void)
{
//	return glMatrix;

	glMatrix[0] = (float)(1 - 2.0 * ((*this)[1] * (*this)[1] + (*this)[2] * (*this)[2]));
	glMatrix[1] = (float)(2.0 * ((*this)[0] * (*this)[1] - (*this)[2] * this->rotation));
	glMatrix[2] = (float)(2.0 * ((*this)[2] * (*this)[0] + (*this)[1] * this->rotation));
	glMatrix[3] = (float)0.0;

	glMatrix[4] = (float)(2.0 * ((*this)[0] * (*this)[1] + (*this)[2] * this->rotation));
	glMatrix[5] = (float)(1 - 2.0 * ((*this)[2] * (*this)[2] + (*this)[0] * (*this)[0]));
	glMatrix[6] = (float)(2.0 * ((*this)[1] * (*this)[2] - (*this)[0] * this->rotation));
	glMatrix[7] = (float)0.0;

	glMatrix[8] = (float)(2.0 * ((*this)[2] * (*this)[0] - (*this)[1] * this->rotation));
	glMatrix[9] = (float)(2.0 * ((*this)[1] * (*this)[2] + (*this)[0] * this->rotation));
	glMatrix[10] = (float)(1 - 2.0 * ((*this)[1] * (*this)[1] + (*this)[0] * (*this)[0]));
	glMatrix[11] = (float)0.0;

	glMatrix[12] = (float)0.0;
	glMatrix[13] = (float)0.0;
	glMatrix[14] = (float)0.0;
	glMatrix[15] = (float)1.0;

	return glMatrix;
}

void TREFacing::getInverseMatrix(float *inverseMatrix)
{
	TCVector::invertMatrix(getMatrix(), inverseMatrix);

//	glMatrix[0]=-(1-2.0 * ((*this)[1] * (*this)[1] + (*this)[2] * (*this)[2]));
//	glMatrix[1]=-(2.0*((*this)[0] * (*this)[1] - (*this)[2] * this->rotation));
//	glMatrix[2]=-(2.0*((*this)[2] * (*this)[0] + (*this)[1] * this->rotation));
//	glMatrix[3]= 0.0;
//
//	glMatrix[4]=-(2.0*((*this)[0] * (*this)[1] + (*this)[2] * this->rotation));
//	glMatrix[5]=-(1-2.0 * ((*this)[2] * (*this)[2] + (*this)[0] * (*this)[0]));
//	glMatrix[6]=-(2.0*((*this)[1] * (*this)[2] - (*this)[0] * this->rotation));
//	glMatrix[7]= 0.0;
//
//	glMatrix[8]=-(2.0*((*this)[2] * (*this)[0] - (*this)[1] * this->rotation));
//	glMatrix[9]=-(2.0*((*this)[1] * (*this)[2] + (*this)[0] * this->rotation));
//	glMatrix[10]=-(1-2.0 * ((*this)[1] * (*this)[1] + (*this)[0] * (*this)[0]));
//	glMatrix[11]= 0.0;
//
//	glMatrix[12] = 0.0;
//	glMatrix[13] = 0.0;
//	glMatrix[14] = 0.0;
//	glMatrix[15] = 1.0;
//
//	return glMatrix;
}

TCVector TREFacing::getVector(void)
{
	return TCVector(
	 (float)(2.0*((*this)[2] * (*this)[0] - (*this)[1] * this->rotation)),
	 (float)(2.0*((*this)[1] * (*this)[2] + (*this)[0] * this->rotation)),
	 (float)(1-2.0 * ((*this)[1] * (*this)[1] + (*this)[0] * (*this)[0]))).normalize();
	//vec[0] = (*this)[0];
	//vec[1] = (*this)[1];
	//vec[2] = (*this)[2];

	//return vec;
}

void TREFacing::pointAt(TCVector &v2)
{
	TCVector tempVec = v2;
	tempVec.normalize();
	TCVector axis = v2 * this->getVector();
	float radians = (float)acos(tempVec.dot(this->getVector()));
	this->setFacing(axis, radians);
}

void TREFacing::print(FILE* outFile)
{
	TCVector::print(outFile);
	fprintf(outFile, " %f", rotation);
}

TCVector TREFacing::difference(TREFacing from)
{
	TCVector newVec(from.getVector());
	newVec.mult(getMatrix());
	return newVec;
}

void TREFacing::swapMatrixRows(float* m, int r1, int r2)
{
	float tmpRow[4];

	memcpy(tmpRow, m + r1*4, 4*sizeof(float));
	memmove(m + r1*4, m + r2*4, 4*sizeof(float));
	memmove(m + r2*4, tmpRow, 4*sizeof(float));
}

float* TREFacing::invertMatrix(float* inM)
{
	float* inv;
	float m[16];
	int i, j;
	float tmp;

//	inv = new float[16];
	memcpy(m, inM, sizeof(m));
	inv = glMatrix;
	memcpy(inv, identityMatrix, sizeof(identityMatrix));
	if (fEq(m[0], 0.0))
	{
		for (i = 1; i < 4 && fEq(m[i*4], 0); i++)
			;
		if (i == 4)
		{
			debugPrintf("Error: Matrix inversion failed.\n");
//			exit(1);
		}
		swapMatrixRows(m, 0, i);
		swapMatrixRows(inv, 0, i);
	}
	tmp = m[0];
	if (!fEq(tmp, 1.0))
	{
		for (i = 0; i < 4; i++)
		{
			m[i] /= tmp;
			inv[i] /= tmp;
		}
	}
	for (i = 1; i < 4; i++)
	{
		if (!fEq(m[i*4], 0.0))
		{
			tmp = m[i*4];
			for (j = 0; j < 4; j++)
			{
				m[i*4+j] -= tmp * m[j];
				inv[i*4+j] -= tmp * inv[j];
			}
		}
	}
	if (fEq(m[5], 0.0))
	{
		for (i = 2; i < 4 && fEq(m[i*4+1], 0); i++)
			;
		if (i == 4)
		{
			debugPrintf("Error: Matrix inversion failed.\n");
//			exit(1);
		}
		swapMatrixRows(m, 1, i);
		swapMatrixRows(inv, 1, i);
	}
	tmp = m[5];
	if (!fEq(tmp, 1.0))
	{
		for (i = 4; i < 8; i++)
		{
			m[i] /= tmp;
			inv[i] /= tmp;
		}
	}
	for (i = 2; i < 4; i++)
	{
		if (!fEq(m[i*4+1], 0.0))
		{
			tmp = m[i*4+1];
			for (j = 0; j < 4; j++)
			{
				m[i*4+j] -= tmp * m[j+4];
				inv[i*4+j] -= tmp * inv[j+4];
			}
		}
	}
	if (fEq(m[10], 0.0))
	{
		if (fEq(m[14], 0.0))
		{
			debugPrintf("Error: Matrix inversion failed.\n");
//			exit(1);
		}
		swapMatrixRows(m, 2, 3);
		swapMatrixRows(inv, 2, 3);
	}
	tmp = m[10];
	if (!fEq(tmp, 1.0))
	{
		for (i = 8; i < 12; i++)
		{
			m[i] /= tmp;
			inv[i] /= tmp;
		}
	}
	if (!fEq(m[14], 0.0))
	{
		tmp = m[14];
		for (j = 0; j < 4; j++)
		{
			m[12+j] -= tmp * m[j+8];
			inv[12+j] -= tmp * inv[j+8];
		}
	}
	if (fEq(m[15], 0.0))
	{
		debugPrintf("Error: Matrix inversion failed.\n");
//		exit(1);
	}
	tmp = m[15];
	if (!fEq(tmp, 1.0))
	{
		for (i = 12; i < 16; i++)
		{
			m[i] /= tmp;
			inv[i] /= tmp;
		}
	}
	tmp = m[1];
	for (i = 0; i < 4; i++)
	{
		m[i] -= m[i+4]*tmp;
		inv[i] -= inv[i+4]*tmp;
	}
	tmp = m[2];
	for (i = 0; i < 4; i++)
	{
		m[i] -= m[i+8]*tmp;
		inv[i] -= inv[i+8]*tmp;
	}
	tmp = m[3];
	for (i = 0; i < 4; i++)
	{
		m[i] -= m[i+12]*tmp;
		inv[i] -= inv[i+12]*tmp;
	}
	tmp = m[6];
	for (i = 0; i < 4; i++)
	{
		m[i+4] -= m[i+8]*tmp;
		inv[i+4] -= inv[i+8]*tmp;
	}
	tmp = m[7];
	for (i = 0; i < 4; i++)
	{
		m[i+4] -= m[i+12]*tmp;
		inv[i+4] -= inv[i+12]*tmp;
	}
	tmp = m[11];
	for (i = 0; i < 4; i++)
	{
		m[i+8] -= m[i+12]*tmp;
		inv[i+8] -= inv[i+12]*tmp;
	}
	return inv;
}
