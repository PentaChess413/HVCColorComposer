/*����������������������������������������������������������������������������������������

	License_Files

	� 2004-2006 TransWarp Technologies, LP.

����������������������������������������������������������������������������������������*/
/* Remove ASAP
#ifndef _License_Files_h
	#define _License_Files_h

#ifndef _License_h
	#include "License.h"
#endif

extern "C"
{

struct	RSAPublicKey;

}

namespace Licensing
{

enum {	DEMO_ELEMENTS = 2 };

enum {	BUFFER_LENGTH = 1024, BASE_LENGTH = 4, DEMO_COUNT_LENGTH = BASE_LENGTH * DEMO_ELEMENTS,
		KEY_BASE_OFFSET = DEMO_ELEMENTS - 1 };

enum {	COMPANY_KEY = 128, PRODUCT_KEY };

SInt32		load_license(UInt8* buffer);
OSStatus	save_license(UInt8* license, const SInt32 license_length);

void		load_key(RSAPublicKey& key, short res_id);

}

#endif
*/