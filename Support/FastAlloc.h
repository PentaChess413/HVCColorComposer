
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2005-2006 Little Labs DSP.  All rights reserved.  $Revision: 1.2 $
//      This material is the confidential trade secret and proprietary
//      information of Little Labs DSP.  It may not be reproduced, used,
//      sold or transferred to any third party without the prior written
//      consent of Little Labs DSP.  All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////

/*	History:
04oct26 lma FOSS: Deleted superfluous methods I didn't implement for the replacement memory allocator
02may05	alx	Added CopyIntoCircularBuffer
18oct04	alx	Line endings changed to UNIX
26jan04	alx	Updated copyright, port to OS X and change stream interface

*/

// FastAlloc.h
// stubbed for now.

#include "PhotoshopSDK.h"

#ifndef _H_FastAlloc
#define _H_FastAlloc


typedef void* SpMemoryPtr;

class FastAlloc
{
 public:

  static SpMemoryPtr New( SpUlong size );
  static SpMemoryPtr NewClear( SpUlong size );
  static void Delete( SpMemoryPtr ptr );

  static SpUlong GetSize( SpMemoryPtr ptr);
  static SpMemoryPtr Resize( SpMemoryPtr ptr, SpUlong newSize );
 
  static void ClearNumBytes( SpMemoryPtr ptr, SpUlong numToClear, SpByte pattern = 0 );
};

#endif // !_H_FastAlloc

// EOF
