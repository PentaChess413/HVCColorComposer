
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1998-2004 FOInc.  All rights reserved.  $Revision: 1.10 $
//      This material is the confidential trade secret and proprietary
//      information of FOInc.  It may not be reproduced, used,
//      sold or transferred to any third party without the prior written
//      consent of FOInc.  All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////

/*	History:

04oct26 lma FOSS: Reimplemented the macros. I haven't tested them in PPC, so expect bugs there.
16feb06	alx	Modified for MasterColors.
18oct04	alx	Line endings changed to UNIX
02feb02	alx	Created

*/


#ifndef _H_SpDebug
#include <stdio.h>
#define _H_SpDebug
#endif
#ifdef _MSC_VER
#define XDEBUG_PRINT(fmt)			OutputDebugString(TEXT(fmt))
#define XDEBUG_PRINT3(fmt, a, b, c)	do { char buffer[256]; _snprintf(buffer, sizeof(buffer), fmt, a, b, c); OutputDebugString(TEXT(buffer)); } while(0)
#elif DEFINED(__APPLE__)
static void MacLog(char* fmt); // gonna have to do it like this because objc shenanigans
#define XDEBUG_PRINT(fmt)		MacLog(fmt)
#define XDEBUG_PRINT3(fmt, a, b, c)	do { char buffer[256]; snprintf(buffer, sizeof(buffer), fmt, a, b, c); OutputDebugString(TEXT(buffer)); } while(0)
#endif
// EOF