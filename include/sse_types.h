// sse_types.h: interface for the SSE classes.
//
//////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002 Tanguy Fautré.
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any damages
//  arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any purpose,
//  including commercial applications, and to alter it and redistribute it
//  freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and must not be
//     misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source distribution.
//
//  Tanguy Fautré
//  softdev@pandora.be
//
//////////////////////////////////////////////////////////////////////
//
//							SIMD::SSE (Scalar and vector)
//							*********
//
// Current version: 1.00 BETA 3 (22/07/2002)
//
// Comment: Currently only supports SSE instructions. 
//          MMX isn't supported yet.
//
// History: - 1.00 BETA 3 (22/07/2002) - Added memory functions
//                                     - Added SSE/MMX headers
//          - 1.00 BETA 2 (13/05/2002) - Improved portability
//          - 1.00 BETA 1 (27/04/2002) - First public release
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <xmmintrin.h>	// SSE intrinsics
#include "mmx_types.h"	// SIMD::MMX header 



// namespace simd
namespace simd {



// Declare classes
class sse128f;
class float1;
class float4;



// Base class for SSE 128bits types
class sse128f
{
public:

	// Conversion operators
	operator __m128() const	{ return m_m128; }

	// Free Store operators
	void * operator new (size_t);
	void * operator new[] (size_t);
	void operator delete (void *, size_t);
	void operator delete[] (void *);

protected:

	sse128f () { }
	sse128f (const __m128 & m) : m_m128(m) { }

	__m128	m_m128;
};



// 1 scalar single precision 32bits floating point numbers: f0/x/x/x
class float1 : public sse128f
{
public:

	// Constructors/Destructors
	float1 ();
	float1 (const __m128 & m);
	float1 (const float & f);

	// Conversion operators
	operator float() const;

	// Assignement operators (see constructors comments)
	float1 & operator = (const __m128 & m);
	float1 & operator = (const float & f);
	float1 & operator = (const float1 & F1);
	float1 & Set (const __m128 & m);
	float1 & Set (const float & f);
	float1 & Set (const float1 & F1);
	const float1 & Store (float & f) const;

	// Logical operators
	float1 operator ! () const;
	float1 operator & (const float1 & F1) const;
	float1 operator | (const float1 & F1) const;
	float1 operator ^ (const float1 & F1) const;
	float1 & operator &= (const float1 & F1);
	float1 & operator |= (const float1 & F1);
	float1 & operator ^= (const float1 & F1);

	// Arithmetic operators
	float1 operator + (const float1 & F1) const;
	float1 operator - (const float1 & F1) const;
	float1 operator * (const float1 & F1) const;
	float1 operator / (const float1 & F1) const;
	float1 & operator += (const float1 & F1);
	float1 & operator -= (const float1 & F1);
	float1 & operator *= (const float1 & F1);
	float1 & operator /= (const float1 & F1);

	// Comparison operators
	int operator < (const float1 & F1) const;
	int operator > (const float1 & F1) const;
	int operator == (const float1 & F1) const;
	int operator != (const float1 & F1) const;
	int operator <= (const float1 & F1) const;
	int operator >= (const float1 & F1) const;
	float1 CmpLT (const float1 & F1) const;			// <
	float1 CmpGT (const float1 & F1) const;			// >
	float1 CmpEq (const float1 & F1) const;			// ==
	float1 CmpNE (const float1 & F1) const;			// !=
	float1 CmpLE (const float1 & F1) const;			// <=
	float1 CmpGE (const float1 & F1) const;			// >=
	float1 CmpOrd (const float1 & F1) const;		// Ordered
	float1 CmpUnord (const float1 & F1) const;		// Unordered

	// Conditional operators (same as (* this) ? (F1A) : (F1B) )
	// (* this) must be the result of a Cmp?() function, otherwise Select behavior is undefined
	float1 Select (const float1 & F1A, const float1 & F1B) const;

	// Advanced Arithmetic operators (using SSE low precision)
	float1 Rcp () const;					// Reciprocal:  rn = rcp(fn)
	float1 RSqrt () const;					// Reciprocal Square Root:  rn = rsqrt(fn)
	float1 Sqrt () const;					// Square Root:  rn = sqrt(fn)
};

// Additional C-like functions related float4
float1 andnot (const float1 & F1A, const float1 & F1B);
float1 ordered (const float1 & F1A, const float1 & F1B);
float1 select (const float1 & F1Cond, const float1 & F1A, const float1 & F1B);
float1 simd_min (const float1 & F1A, const float1 & F1B);
float1 simd_max (const float1 & F1A, const float1 & F1B);
float1 rcp (const float1 & F1);
float1 rsqrt (const float1 & F1);
float1 sqrt (const float1 & F4);
float1 unordered (const float1 & F1A, const float1 & F1B);



// 4 packed single precision 32bits floating point numbers: f0/f1/f2/f3
class float4 : public sse128f
{
public:

	// Constructors/Destructors
	float4 ();
	float4 (const zero &);
	float4 (const __m128 & m);
	float4 (const float & f);
	float4 (const float F[4]);
	float4 (const float1 & F1); 
	float4 (const float & f0, const float & f1, const float & f2, const float & f3);

	// Assignement operators
	float4 & operator = (const zero &);
	float4 & operator = (const __m128 & m);
	float4 & operator = (const float & f);
	float4 & operator = (const float F[4]);
	float4 & operator = (const float1 & F1);
	float4 & LoadA (const float F[4]);
	float4 & LoadU (const float F[4]);
	float4 & Set (const zero &);
	float4 & Set (const __m128 & m);
	float4 & Set (const float & f);
	float4 & Set (const float F[4]);
	float4 & Set (const float1 & F1);
	float4 & Set (const float & f0, const float & f1, const float & f2, const float & f3);
	const float4 & StoreA (float F[4]) const;
	const float4 & StoreU (float F[4]) const;
	const float4 & StoreNTA (float4 & F4) const;

	// Subscript operators
	float & operator [] (size_t i);
	const float & operator [] (size_t i) const;

	// Logical operators
	float4 operator ! () const;
	float4 operator & (const float4 & F4) const;
	float4 operator | (const float4 & F4) const;
	float4 operator ^ (const float4 & F4) const;
	float4 & operator &= (const float4 & F4);
	float4 & operator |= (const float4 & F4);
	float4 & operator ^= (const float4 & F4);

	// Arithmetic operators
	float4 operator + (const float4 & F4) const;
	float4 operator - (const float4 & F4) const;
	float4 operator * (const float4 & F4) const;
	float4 operator / (const float4 & F4) const;
	float4 & operator += (const float4 & F4);
	float4 & operator -= (const float4 & F4);
	float4 & operator *= (const float4 & F4);
	float4 & operator /= (const float4 & F4);

	// Comparison operators
	float4 operator < (const float4 & F4) const;
	float4 operator > (const float4 & F4) const;
	float4 operator == (const float4 & F4) const;
	float4 operator != (const float4 & F4) const;
	float4 operator <= (const float4 & F4) const;
	float4 operator >= (const float4 & F4) const;
	float4 CmpOrd (const float4 & F4) const;		// a ord b => both a and b aren't NaN or an undefined format
	float4 CmpUnord (const float4 & F4) const;		// a unord b => a or b is NaN or an undefined format

	// Conditional operators (same as (* this) ? (F4A) : (F4B) )
	// (* this) must be the result of a comparison operator, otherwise Select behavior is undefined
	float4 Select (const float4 & F4A, const float4 & F4B) const;

	// Advanced Arithmetic operators (using SSE low precision)
	float4 Rcp () const;					// Reciprocal:  rn = rcp(fn)
	float4 RSqrt () const;					// Reciprocal Square Root:  rn = rsqrt(fn)
	float4 Sqrt () const;					// Square Root:  rn = sqrt(fn)

	// Miscellaneous SSE functions
	int GetSignMask () const;									// = MoveMask = _mm_movemask_ps
	template <int i> float4 Shuffle () const;					// = shuffle<i>((* this), (* this))
	template <int i> float4 Shuffle (const float4 & F4) const;	// = shuffle<i>((* this), F4)
	float4 MoveHL (const float4 & F4) const;
	float4 MoveLH (const float4 & F4) const;
	float4 UnPackHigh (const float4 & F4) const;
	float4 UnPackLow (const float4 & F4) const;
};

// Additional C-like functions related float4:
// Logical functions
float4 andnot (const float4 & F4A, const float4 & F4B);

// Comparaison functions
float4 ordered (const float4 & F4A, const float4 & F4B);
float4 unordered (const float4 & F4A, const float4 & F4B);
float4 select (const float4 & F4Cond, const float4 & F4A, const float4 & F4B);
float4 simd_min (const float4 & F4A, const float4 & F4B);
float4 simd_max (const float4 & F4A, const float4 & F4B);

// Advanced Arithmetic functions
float4 rcp (const float4 & F4);
float4 rsqrt (const float4 & F4);
float4 sqrt (const float4 & F4);

// Miscellaneous functions
int getsignmask (const float4 & F4); 
float4 movehl (const float4 & F4A, const float4 & F4B);
float4 movelh (const float4 & F4A, const float4 & F4B);
float4 unpackhi (const float4 & F4A, const float4 & F4B);
float4 unpacklo (const float4 & F4A, const float4 & F4B);

// _mm_shuffle_ps(F4A, F4B, i) C++ template wrapper
template <int i> float4 shuffle (const float4 & F4A, const float4 & F4B);

// _SSE_SHUFFLE macro
#define _SSE_SHUFFLE(f0, f1, f2, f3)	_MM_SHUFFLE(f3, f2, f1, f0)



// Additional C-like general purpose SSE functions
unsigned int getcsr ();
void setcsr (unsigned int i);
void sfence ();



// enum for prefetch
enum hint {
	T0 = _MM_HINT_T0,
	T1 = _MM_HINT_T1,
	T2 = _MM_HINT_T2,
	NTA = _MM_HINT_NTA
};

// _mm_prefetch(p, i) C++ template wrapper
template <hint i> void prefetch (char * p);



// "C-like" memory functions for aligned boundaries
void  * simd_malloc (size_t NbBytes, size_t Alignement = 16);
void simd_free (void * Ptr);



// Additional Advanced Arithmetic (template) functions (using multiple instructions)
float1 addhorizontal (const float4 & F4);
template <class T> T rcpnr (const T & F);		// Newton-Raphson Reciprocal:  rn = rcpnr(fn)  ([2 * rcp(f) - (f * rcp(f) * rcp(f))])
template <class T> T rsqrtnr (const T & F);		// Newton-Raphson Reciprocal Square Root:  rn = rsqrtnr(fn)  ([0.5 * rsqrt(f) * (3 - f * rsqrt(f) * rsqrt(f))])




//////////////////////////////////////////////////////////////////////
// simd::sse128f Inline Functions
//////////////////////////////////////////////////////////////////////

inline void * sse128f::operator new (size_t s) {
	void * p = simd_malloc(s);
	if (p == NULL)
		return NULL;
	else
		return p;
}

inline void * sse128f::operator new[] (size_t s) {
	void * p = simd_malloc(s);
	if (p == NULL)
		return NULL;
	else
		return p;
}

inline void sse128f::operator delete (void * p, size_t /*s*/) {
	simd_free(p);
}

inline void sse128f::operator delete[] (void * p) {
	simd_free(p);
}



//////////////////////////////////////////////////////////////////////
// simd::float1 Inline Functions
//////////////////////////////////////////////////////////////////////

inline float1::float1 () { }
inline float1::float1 (const __m128 & m) : sse128f(m) { }
inline float1::float1 (const float & f) : sse128f(_mm_set_ss(f)) { }


inline float1::operator float() const { float Result; _mm_store_ss(&Result, (* this)); return Result; }


inline float1 & float1::operator = (const __m128 & m) { return Set(m); }
inline float1 & float1::operator = (const float & f) { return Set(f); }
inline float1 & float1::operator = (const float1 & F1) { return Set(F1); }


inline float1 & float1::Set (const __m128 & m) { m_m128 = m; return (* this); }
inline float1 & float1::Set (const float & f) { m_m128 = _mm_set_ss(f); return (* this); }
inline float1 & float1::Set (const float1 & F1) { m_m128 = _mm_move_ss((* this), F1); return (* this); }


inline const float1 & float1::Store (float & f) const { _mm_store_ss(&f, (* this)); return (* this); }


inline float1 float1::operator ! () const { return andnot((* this), (* this)); }
inline float1 float1::operator & (const float1 & F1) const { return _mm_and_ps((* this), F1); }
inline float1 float1::operator | (const float1 & F1) const { return _mm_or_ps((* this), F1); }
inline float1 float1::operator ^ (const float1 & F1) const { return _mm_xor_ps((* this), F1); }
inline float1 & float1::operator &= (const float1 & F1) { return ((* this) = (* this) & F1); }
inline float1 & float1::operator |= (const float1 & F1) { return ((* this) = (* this) | F1); }
inline float1 & float1::operator ^= (const float1 & F1) { return ((* this) = (* this) ^ F1); }


inline float1 float1::operator + (const float1 & F1) const { return _mm_add_ss((* this), F1); }
inline float1 float1::operator - (const float1 & F1) const { return _mm_sub_ss((* this), F1); }
inline float1 float1::operator * (const float1 & F1) const { return _mm_mul_ss((* this), F1); }
inline float1 float1::operator / (const float1 & F1) const { return _mm_div_ss((* this), F1); }
inline float1 & float1::operator += (const float1 & F1) { return ((* this) = (* this) + F1); }
inline float1 & float1::operator -= (const float1 & F1) { return ((* this) = (* this) - F1); }
inline float1 & float1::operator *= (const float1 & F1) { return ((* this) = (* this) * F1); }
inline float1 & float1::operator /= (const float1 & F1) { return ((* this) = (* this) / F1); }


inline int float1::operator < (const float1 & F1) const { return _mm_comilt_ss((* this), F1); }
inline int float1::operator > (const float1 & F1) const { return _mm_comigt_ss((* this), F1); }
inline int float1::operator == (const float1 & F1) const { return _mm_comieq_ss((* this), F1); }
inline int float1::operator != (const float1 & F1) const { return _mm_comineq_ss((* this), F1); }
inline int float1::operator <= (const float1 & F1) const { return _mm_comile_ss((* this), F1); }
inline int float1::operator >= (const float1 & F1) const { return _mm_comige_ss((* this), F1); }


inline float1 float1::CmpLT (const float1 & F1) const { return _mm_cmplt_ss((* this), F1); }
inline float1 float1::CmpGT (const float1 & F1) const { return _mm_cmpgt_ss((* this), F1); }
inline float1 float1::CmpEq (const float1 & F1) const { return _mm_cmpeq_ss((* this), F1); }
inline float1 float1::CmpNE (const float1 & F1) const { return _mm_cmpneq_ss((* this), F1); }
inline float1 float1::CmpLE (const float1 & F1) const { return _mm_cmple_ss((* this), F1); }
inline float1 float1::CmpGE (const float1 & F1) const { return _mm_cmpge_ss((* this), F1); }
inline float1 float1::CmpOrd (const float1 & F1) const { return ordered((* this), F1); }	
inline float1 float1::CmpUnord (const float1 & F1) const { return unordered((* this), F1); }


inline float1 float1::Select(const float1 & F1A, const float1 & F1B) const { return (((* this) & F1A) | (andnot((* this), F1B))); }


inline float1 float1::Rcp() const { return _mm_rcp_ss((* this)); }
inline float1 float1::RSqrt() const { return _mm_rsqrt_ss((* this)); }
inline float1 float1::Sqrt() const { return _mm_sqrt_ss((* this)); }


float1 andnot (const float1 & F1A, const float1 & F1B) { return _mm_andnot_ps(F1A, F1B); }
float1 ordered (const float1 & F1A, const float1 & F1B) { return _mm_cmpord_ss(F1A, F1B); }
float1 select (const float1 & F1Cond, const float1 & F1A, const float1 & F1B) { return F1Cond.Select(F1A, F1B); }
float1 simd_min (const float1 & F1A, const float1 & F1B) { return _mm_min_ss(F1A, F1B); }
float1 simd_max (const float1 & F1A, const float1 & F1B) { return _mm_max_ss(F1A, F1B); }
float1 rcp (const float1 & F1) { return F1.Rcp(); }
float1 rsqrt (const float1 & F1) { return F1.RSqrt(); }
float1 sqrt (const float1 & F1) { return F1.Sqrt(); }
float1 unordered (const float1 & F1A, const float1 & F1B) { return _mm_cmpunord_ss(F1A, F1B); }



//////////////////////////////////////////////////////////////////////
// simd::float4 Inline Functions
//////////////////////////////////////////////////////////////////////

inline float4::float4 () { }
inline float4::float4 (const zero &) : sse128f(_mm_setzero_ps()) { }
inline float4::float4 (const __m128 & m) : sse128f(m) { }
inline float4::float4 (const float & f) : sse128f(_mm_set_ps1(f)) { }
inline float4::float4 (const float F[4]) : sse128f(_mm_loadu_ps(F)) { }
inline float4::float4 (const float1 & F1) : sse128f(F1) { }
inline float4::float4 (const float & f0, const float & f1, const float & f2, const float & f3) : sse128f(_mm_set_ps(f3, f2, f1, f0)) { }


inline float4 & float4::operator = (const zero & Zero) { return Set(Zero); }
inline float4 & float4::operator = (const __m128 & m) { return Set(m); }
inline float4 & float4::operator = (const float & f) { return Set(f); }
inline float4 & float4::operator = (const float F[4]) { return Set(F); }
inline float4 & float4::operator = (const float1 & F1) { return Set(F1); }


inline float4 & float4::LoadA (const float F[4]) { m_m128 = _mm_load_ps(F); return (* this); }
inline float4 & float4::LoadU (const float F[4]) { m_m128 = _mm_loadu_ps(F); return (* this); }


inline float4 & float4::Set (const zero &) { m_m128 = _mm_setzero_ps(); return (* this); }
inline float4 & float4::Set (const __m128 & m) { m_m128 = m; return (* this); }
inline float4 & float4::Set (const float & f) { m_m128 = _mm_set_ps1(f); return (* this); }
inline float4 & float4::Set (const float F[4]) { return LoadU(F); }
inline float4 & float4::Set (const float1 & F1) { m_m128 = _mm_move_ss((* this), F1); return (* this); }
inline float4 & float4::Set (const float & f0, const float & f1, const float & f2, const float & f3) { m_m128 = _mm_set_ps(f3, f2, f1, f0); return (* this); }


inline const float4 & float4::StoreA (float F[4]) const { _mm_store_ps(F, (* this)); return (* this); }
inline const float4 & float4::StoreU (float F[4]) const { _mm_store_ps(F, (* this)); return (* this); }
inline const float4 & float4::StoreNTA (float4 & F4) const { _mm_stream_ps((float *) &(F4.m_m128), (* this)); return (* this); }


inline float & float4::operator [] (size_t i) { return ((float *) &m_m128)[i]; }
inline const float & float4::operator [] (size_t i) const { return ((const float *) &m_m128)[i]; }


inline float4 float4::operator ! () const { return andnot((* this), (* this)); }
inline float4 float4::operator & (const float4 & F4) const { return _mm_and_ps((* this), F4); }
inline float4 float4::operator | (const float4 & F4) const { return _mm_or_ps((* this), F4); }
inline float4 float4::operator ^ (const float4 & F4) const { return _mm_xor_ps((* this), F4); }
inline float4 & float4::operator &= (const float4 & F4) { return ((* this) = (* this) & F4); }
inline float4 & float4::operator |= (const float4 & F4) { return ((* this) = (* this) | F4); }
inline float4 & float4::operator ^= (const float4 & F4) { return ((* this) = (* this) ^ F4); }


inline float4 float4::operator + (const float4 & F4) const { return _mm_add_ps((* this), F4); }
inline float4 float4::operator - (const float4 & F4) const { return _mm_sub_ps((* this), F4); }
inline float4 float4::operator * (const float4 & F4) const { return _mm_mul_ps((* this), F4); }
inline float4 float4::operator / (const float4 & F4) const { return _mm_div_ps((* this), F4); }
inline float4 & float4::operator += (const float4 & F4) { return ((* this) = (* this) + F4); }
inline float4 & float4::operator -= (const float4 & F4) { return ((* this) = (* this) - F4); }
inline float4 & float4::operator *= (const float4 & F4) { return ((* this) = (* this) * F4); }
inline float4 & float4::operator /= (const float4 & F4) { return ((* this) = (* this) / F4); }


inline float4 float4::operator < (const float4 & F4) const { return _mm_cmplt_ps((* this), F4); }
inline float4 float4::operator > (const float4 & F4) const { return _mm_cmpgt_ps((* this), F4); }
inline float4 float4::operator == (const float4 & F4) const { return _mm_cmpeq_ps((* this), F4); }
inline float4 float4::operator != (const float4 & F4) const { return _mm_cmpneq_ps((* this), F4); }
inline float4 float4::operator <= (const float4 & F4) const { return _mm_cmple_ps((* this), F4); }
inline float4 float4::operator >= (const float4 & F4) const { return _mm_cmpge_ps((* this), F4); }
inline float4 float4::CmpOrd (const float4 & F4) const { return ordered((* this), F4); }
inline float4 float4::CmpUnord (const float4 & F4) const { return unordered((* this), F4); }


inline float4 float4::Select (const float4 & F4A, const float4 & F4B) const { return (((* this) & F4A) | (andnot((* this), F4B))); }


inline float4 float4::Rcp() const { return _mm_rcp_ps((* this)); }
inline float4 float4::RSqrt() const { return _mm_rsqrt_ps((* this)); }
inline float4 float4::Sqrt() const { return _mm_sqrt_ps((* this)); }


inline int float4::GetSignMask() const { return _mm_movemask_ps((* this)); }
template <int i> inline float4 float4::Shuffle () const { return shuffle<i>((* this), (* this)); }
template <int i> inline float4 float4::Shuffle (const float4 & F4) const { return shuffle<i>((* this), F4); }
inline float4 float4::MoveHL (const float4 & F4) const { return movehl((* this), F4); }
inline float4 float4::MoveLH (const float4 & F4) const { return movelh((* this), F4); }
inline float4 float4::UnPackHigh (const float4 & F4) const { return unpackhi((* this), F4); }
inline float4 float4::UnPackLow (const float4 & F4) const { return unpacklo((* this), F4); }


inline int getsignmask (const float4 & F4) { return F4.GetSignMask(); }
inline float4 andnot (const float4 & F4A, const float4 & F4B) { return _mm_andnot_ps(F4A, F4B); }
inline float4 movehl (const float4 & F4A, const float4 & F4B) { return _mm_movehl_ps(F4A, F4B); }
inline float4 movelh (const float4 & F4A, const float4 & F4B) { return _mm_movelh_ps(F4A, F4B); }
inline float4 ordered (const float4 & F4A, const float4 & F4B) { return _mm_cmpord_ps(F4A, F4B); }
inline float4 select (const float4 & F4Cond, const float4 & F4A, const float4 & F4B) { return F4Cond.Select(F4A, F4B); }
inline float4 simd_min (const float4 & F4A, const float4 & F4B) { return _mm_min_ps(F4A, F4B); }
inline float4 simd_max (const float4 & F4A, const float4 & F4B) { return _mm_max_ps(F4A, F4B); }
inline float4 rcp (const float4 & F4) { return F4.Rcp(); }
inline float4 rsqrt (const float4 & F4) { return F4.RSqrt(); }
inline float4 sqrt (const float4 & F4) { return F4.Sqrt(); }
inline float4 unordered (const float4 & F4A, const float4 & F4B) { return _mm_cmpunord_ps(F4A, F4B); }
inline float4 unpackhi (const float4 & F4A, const float4 & F4B) { return _mm_unpackhi_ps(F4A, F4B); }
inline float4 unpacklo (const float4 & F4A, const float4 & F4B) { return _mm_unpacklo_ps(F4A, F4B); }


template <int i> inline float4 shuffle (const float4 & F4A, const float4 & F4B) { return _mm_shuffle_ps(F4A, F4B, i); }


inline unsigned int getcsr () { return _mm_getcsr(); }
inline void setcsr (unsigned int i) { _mm_setcsr(i); }
inline void sfence () { _mm_sfence(); }


template <hint i> inline void prefetch (char * p) { _mm_prefetch(p, i); }


inline void * simd_malloc (size_t NbBytes, size_t Alignement) { return malloc(NbBytes)/*_mm_malloc(NbBytes, Alignement)*/; }
inline void simd_free (void * Ptr) { free(Ptr);/*_mm_free(Ptr);*/ }

/*
inline float1 addhorizontal (const float4 & F4) {
	return (float1(F4.Shuffle<0>()) + float1(F4.Shuffle<1>()) + float1(F4.Shuffle<2>()) + float1(F4.Shuffle<3>()));
}
*/

template <class T> inline T rcpnr (const T & F) {
	T R = Rcp();
	return ((R + R) - (F * R * R));
}

template <class T> inline T rsqrtnr (const T & F) {
	static const T f0pt5(0.5f);
	static const T f3pt0(3.0f);
	const T R = F.RSqrt();
	return (f0pt5 * R * (f3pt0 - (F * R * R)));
}




}; // namespace simd
