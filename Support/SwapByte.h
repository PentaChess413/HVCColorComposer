
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1998-2004 FOInc.  All rights reserved.  $Revision: 1.1.1.1 $
//      This material is the confidential trade secret and proprietary
//      information of FOInc.  It may not be reproduced, used,
//      sold or transferred to any third party without the prior written
//      consent of FOInc.  All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////

/*	History:

04oct26 lma FOSS: Changed routines to use platform intrinsics instead of proprietary trade secrets
11feb04	alx	Updated code with newer versions of swapping routines...
06may02	alx	Better optimization for PowerPC which is safer

*/

#ifndef _H_SwapByte
#define _H_SwapByte

#ifndef _H_SpTypes
#include "SpTypes.h"
#endif

#if defined(__APPLE__)
#include <libkern/OSByteOrder.h> // for optimized swapping
#endif

inline void SwapShort(SpShort& theShort)
{
	#if defined(_MSC_VER)
		theShort = _byteswap_ushort(theShort);
	#elif defined(__APPLE__)
		theShort = _OSSwapInt16(theShort);
	#endif
}

inline void SwapShort(SpUshort& theShort)
{
	#if defined(_MSC_VER)
		theShort = _byteswap_ushort(theShort);
	#elif defined(__APPLE__)
		theShort = _OSSwapInt16(theShort);
	#endif
}


inline void SwapLong(SpLong& theLong)
{
	#if defined(_MSC_VER)
		theLong = _byteswap_ulong(theLong);
	#elif defined(__APPLE__)
		theLong = _OSSwapInt32(theLong);
	#endif
}

inline void SwapLong(SpUlong& theLong)
{
	#if defined(_MSC_VER)
		theLong = _byteswap_ulong(theLong);
	#elif defined(__APPLE__)
		theLong = _OSSwapInt32(theLong);
	#endif
}

#endif	// !_H_SwapByte

// EOF
