/*����������������������������������������������������������������������������������������

	License

	� 2004-2006 TransWarp Technologies, LP.

����������������������������������������������������������������������������������������*/
/*Remove ASAP
#ifndef _License_h
	#define _License_h

#ifndef _licensing_common_h
	#include "licensing_common.h"
#endif

#ifdef __cplusplus
	extern "C"
	{
#endif

enum {	UNKNOWN_ERR = -20000,
		INVALID_REGISTRATION, INVALID_LICENSE,
		ERROR_MESSAGE
};

OSStatus	call_licensed_code(void* callback_data);
OSStatus	register_software(const char* owner_name, const char* purchase_code,
								const char* version, char* custom_buffer,
								int* custom_buffer_length);

OSStatus	setup_licensed(const char* owner_name, void* callback_data);
OSStatus	unlicensed_setup(void* callback_data);

// User defined functions:

const char*	get_registration_url();
const char*	get_license_file_name();

#ifdef __cplusplus
	}
#endif

#endif
*/