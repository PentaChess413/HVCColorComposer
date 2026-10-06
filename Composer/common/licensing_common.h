/*����������������������������������������������������������������������������������������

	licensing_common

	� 2005-2006 TransWarp Technologies, LP.

����������������������������������������������������������������������������������������*/
/* Remove ASAP
#ifndef _licensing_common_h
	#define _licensing_common_h

#ifndef noErr
	#define noErr	0
#endif

#ifndef SInt32
	typedef signed long	SInt32;
#endif

#ifndef UInt32
	typedef unsigned long	UInt32;
#endif

#ifndef UInt16
	typedef unsigned short	UInt16;
#endif

#ifndef UInt8
	typedef unsigned char	UInt8;
#endif

#ifndef OSStatus
	typedef long	OSStatus;
#endif

#ifndef Ptr
	typedef char*	Ptr;
#endif

#define Throw_If_Err(err)	{ OSStatus _err = (err); if (noErr != _err) { throw _err; } }

#endif
*/