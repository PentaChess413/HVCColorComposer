
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1998-2005 FOInc.  All rights reserved.  $Revision: 1.6 $
//      This material is the confidential trade secret and proprietary
//      information of FOInc.  It may not be reproduced, used,
//      sold or transferred to any third party without the prior written
//      consent of FOInc.  All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////

/*	History:

16feb06	alx	Modified for MasterColors.
28nov05	alx	Changed line ending to allow CVS to properly merge file and added another error constant
28apr04	alx	Cleaned up and added some more constants, also made strings readonly
26jan04	alx	Created

*/

// SpError.h

#ifndef _H_SpError
#define _H_SpError


#include "SpTypes.h"

//typedef Int32 SpError;	// we'll bring this back for performance reasons if necessary...

// error codes -- NOTE: don't forget to update codes below in kDebugErrorToStringMap
typedef enum
{
	kSpErr_NoErr = 0,			// no error at all, happy days man!
	kSpErr_BadParam,			// lowlevel error, a bad parameter was passed to some random routine
	
	kSpErr_NoMoreSamples,		// this signals the end of a process, not really an error...
	kSpErr_NoSampleData,		// no samples to process...

	kSpErr_TooManyInputs,		// we can't handle this many inputs...
	kSpErr_NoInput,				// don't have an input and obviously need one!
	kSpErr_NoOutput,			// don't have an output and well read the line above.

	kSpErr_BadAudioChain,		// corrupted audio processing chain...
	kSpErr_BadAudioData,		// bad file possibly?

	kSpErr_OutOfRange,			// value is out of range
	kSpErr_InvalidData,			// Invalid point/data/whatever
	kSpErr_DataExists,			// data already exists
	
	kSpErr_UnsupportedFormat,	// the image or audio data provided is not supported...sorry.
	kSpErr_ResourceUnavailable,	// the requested resource is unavailable, may not be found, etc...
	kSpErr_CantLoad,			// can't load something	

	kSpErr_NoMemory,			// no more memory available
	kSpErr_InvalidSamplingRate,	// we don't have a sampling rate!

	kSpErr_NotImplemented,		// we shouldn't be here at all!!!
	
	// please leave last
	kSpErr_Unknown = 0x7FFFFFFF	// -1 in hex, force this to be an Int32
} SpError;

// define the error table for now, this is for debug code...
typedef struct tagErrorStringMap
{
	SpError 	errNumber;
	SpCharPtr	errString;
} SpErrorStringMap;

// This table maps error numbers to strings for user consumption...
const SpErrorStringMap kDebugErrorToStringMap[] =
{
	{ kSpErr_NoErr,					"No Error" },
	{ kSpErr_BadParam,				"Bad function parameter(s)" },

	{ kSpErr_NoMoreSamples,			"No more samples to process" },
	{ kSpErr_NoSampleData,			"No sample data to process at all!" },

	{ kSpErr_TooManyInputs, 		"Too many inputs" },
	{ kSpErr_NoInput,				"No input connection" },
	{ kSpErr_NoOutput,				"No output connection" },

	{ kSpErr_BadAudioChain,			"Corrupt audio chain" },
	{ kSpErr_BadAudioData,			"Corrupt audio data - bad file format?" },

	{ kSpErr_OutOfRange,			"Value is out of range" },
	{ kSpErr_InvalidData,			"You gave me garbage input, thanks..." },
	{ kSpErr_DataExists,			"Data already exists..." },


	{ kSpErr_UnsupportedFormat, 	"Unsupported file format (image or audio)" },
	{ kSpErr_ResourceUnavailable,	"Unavailable resource" },
	{ kSpErr_CantLoad,				"Can't load!" },

	{ kSpErr_NoMemory,				"Out of memory!" },

	{ kSpErr_InvalidSamplingRate,	"Invalid Sampling Rate!" },

	{ kSpErr_NotImplemented,		"Not implemented, what are we doing here?!" },
	
	// please leave last
	{ kSpErr_Unknown,				"Generic unknown Error" }
};


#endif	// !_H_SpError

// EOF
