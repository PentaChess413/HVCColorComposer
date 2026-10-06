
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1998-2004 FOInc.  All rights reserved.  $Revision: 1.5 $
//      This material is the confidential trade secret and proprietary
//      information of FOInc.  It may not be reproduced, used,
//      sold or transferred to any third party without the prior written
//      consent of FOInc.  All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////

/*	History:

16feb06	alx	Modified for MasterColors.
02mar04	alx	Updated
02jun98	alx	Created

*/


#ifndef _H_SpTypes
#define _H_SpTypes

#ifdef _WIN32
#include <Windows.h>
#else
// Mac includes
#include <MacTypes.h>
#include <Events.h>
//#include <altivec.h>
#endif

#include <stdio.h>
#include <limits.h>
#include <xmmintrin.h> // open source-ified: pragma error triggers otherwise as __m128 not defined
// map platform types to our basic types...
typedef wchar_t				WChar16;
typedef char				Int8;
typedef unsigned char		UInt8;
typedef short				Int16;
typedef unsigned short		UInt16;
typedef long				Int32;
typedef unsigned long		UInt32;
typedef long long			Int64;
typedef unsigned long long	UInt64;
typedef float				SFloat32;
typedef double				SFloat64;
//typedef double double		SFloat128;


#define SPKIT_INT16_MIN SHRT_MIN
#define SPKIT_INT16_MAX SHRT_MAX

#define RV_INT16_MIN SHRT_MIN
#define RV_INT16_MAX SHRT_MAX

#define RV_FLOAT_TO_INT16( a ) ((a) * SHRT_MAX)

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef NULL
#define NULL 0L
#endif

typedef SFloat32 SpSample;
typedef SFloat32 SpFloat;
typedef SFloat64 SpDouble;
typedef Int32 	 SpLong;
typedef UInt32 	 SpUlong;
typedef Int64 	 SpLong64;
typedef UInt64 	 SpUlong64;
typedef UInt8	 SpByte;
typedef Int8	 SpChar;
typedef WChar16	 SpWChar;
typedef UInt16	 SpUshort;
typedef Int16	 SpShort;
typedef UInt32 	 SpFixed;

// pointer types...
typedef UInt8* BytePtr;

typedef SpSample* SpSamplePtr;
typedef const SpSamplePtr ConstSpSamplePtr;

typedef SpFloat* SpFloatPtr;
typedef const SpFloatPtr ConstSpFloatPtr;

typedef SpDouble* SpDoublePtr;
typedef const SpDoublePtr ConstSpDoublePtr;

typedef SpLong* SpLongPtr;
typedef const SpLongPtr ConstSpLongPtr;

typedef SpUlong* SpUlongPtr;
typedef const SpUlongPtr ConstSpUlongPtr;

typedef SpByte* SpBytePtr;
typedef const SpBytePtr ConstSpBytePtr;

typedef SpChar* SpCharPtr;
typedef const SpCharPtr ConstSpCharPtr;

typedef SpWChar* SpWCharPtr;
typedef const SpWCharPtr ConstSpWCharPtr;

typedef SpUshort* SpUshortPtr;
typedef const SpUshortPtr ConstSpUshortPtr;

typedef SpShort* SpShortPtr;
typedef const SpShortPtr ConstSpShortPtr;

typedef SpFixed* SpFixedPtr;
typedef const SpFixedPtr ConstSpFixedPtr;

typedef void* SpVoidPtr;
typedef const void* ConstSpVoidPtr;

typedef FILE* SpFilePtr;



// only works for Altivec
#if defined( __VEC__ )
// special types used for aligning data without pragmas
typedef union
{
	SpFloat 		f[4];	// this forces the data to be quadword aligned
	vector float 	vf;		// this is an altivec specific type (vector float)
} SpVFloat;
#elif defined( __SSE2__ )
typedef union
{
	SpFloat 		f[4];	// this forces the data to be quadword aligned
	__m128			vf;		// this is an SSE specific type (vector float)
} SpVFloat;
#else
typedef union // this is 2026, we can reasonably assume we're building this on modern hardware for now
{
	SpFloat 		f[4];	// this forces the data to be quadword aligned
	__m128			vf;		// this is an SSE specific type (vector float)
} SpVFloat;
// #error "sorry don't know what to do here about vector types..."
#endif


typedef struct tagSpPoint2f
{
	SpFloat x;
	SpFloat y;
} SpPoint2f, * SpPoint2fPtr;

typedef struct tagSpPoint3f
{
	SpFloat x;
	SpFloat y;
	SpFloat z;
} SpPoint3f, * SpPoint3fPtr;

typedef struct tagSpPoint2
{
	SpLong x;
	SpLong y;
} SpPoint2, * SpPoint2Ptr;

typedef struct tagSpPoint3
{
	SpLong x;
	SpLong y;
	SpLong z;
} SpPoint3, * SpPoint3Ptr;

typedef const SpPoint2fPtr ConstSpPoint2fPtr;
typedef const SpPoint3fPtr ConstSpPoint3fPtr;
typedef const SpPoint2Ptr ConstSpPoint2Ptr;
typedef const SpPoint3Ptr ConstSpPoint3Ptr;

#if WIN32
typedef RECT		RVRect;
#else
typedef EventRef	RVEventRef;
typedef EventRecord RVEventRecord;
typedef CGrafPtr	RVGrafPtr;
typedef Rect		RVRect;
#endif


#endif	// !_H_SpTypes

// EOF
