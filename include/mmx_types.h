// mmx_types.h: interface for the MMX classes.
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
//							SIMD::MMX (Scalar and vector)
//							*********
//
// Current version: 0.01 ALPHA (21/07/2002)
//
// Comment: Include the basic types for all SIMD classes and the 
//          scaffholding for a future MMX support. 
//          Though, it currently offers no MMX support.
//
// History: - 0.01 (21/07/2002) - First public draft 
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <mmintrin.h>	// MMX intrinsics



// namespace simd
namespace simd {



// Declare classes
class zero;
class char8;



// Equivalent to simd(0)
class zero { };



// Base class for MMX 64bits types
class mmx64
{
public:

	// Conversion operators
	operator __m64() const	{ return m_m64; }

protected:

	mmx64 () { }
	mmx64 (const __m64 & m) : m_m64(m) { }

	__m64	m_m64;
};



// 8 packed 8bits integers numbers: c0/c1/c2/c3/c4/c5/c6/c7
class char8 : public mmx64
{
public:

	// Constructors/Destructors
	char8 ();
	char8 (const zero &);
	char8 (const __m64 & m);
	char8 (char c);
	char8 (const char C[8]);
	char8 (char c0, char c1, char c2, char c3, char c4, char c5, char c6, char c7);

	// Assignement operators
	char8 & operator = (const zero &);
	char8 & operator = (const __m64 & m);
	char8 & operator = (char c);
	char8 & operator = (const char C[8]);
	char8 & Set (const zero &);
	char8 & Set (const __m64 & m);
	char8 & Set (char c);
	char8 & Set (const char C[8]);
	char8 & Set (char c0, char c1, char c2, char c3, char c4, char c5, char c6, char c7);

	// Subscript operators
	char & operator [] (size_t i);
	const char & operator [] (size_t i) const;

	// Logical operators
	char8 operator ! () const;
	char8 operator & (const char8 & C8) const;
	char8 operator | (const char8 & C8) const;
	char8 operator ^ (const char8 & C8) const;
	char8 & operator &= (const char8 & C8);
	char8 & operator |= (const char8 & C8);
	char8 & operator ^= (const char8 & C8);

	// Arithmetic operators
	char8 operator + (const char8 & C8) const;
	char8 operator - (const char8 & C8) const;
	char8 operator * (const char8 & C8) const;
	char8 operator / (const char8 & C8) const;
	char8 & operator += (const char8 & C8);
	char8 & operator -= (const char8 & C8);
	char8 & operator *= (const char8 & C8);
	char8 & operator /= (const char8 & C8);

	// Comparison operators
	char8 operator < (const char8 & C8) const;
	char8 operator > (const char8 & C8) const;
	char8 operator == (const char8 & C8) const;
	char8 operator != (const char8 & C8) const;
	char8 operator <= (const char8 & C8) const;
	char8 operator >= (const char8 & C8) const;

	// Conditional operators (same as (* this) ? (C8A) : (C8B) )
	// (* this) must be the result of a comparison operator, otherwise Select behavior is undefined
	char8 Select (const char8 & C8A, const char8 & C8B) const;

	// Advanced Arithmetic operators

	// Miscellaneous MMX Functions
	char8 UnPackHigh (const char8 & C8) const;
	char8 UnPackLow (const char8 & C8) const;
};




// simd namespace constant
const zero Zero;




}; // namespace simd