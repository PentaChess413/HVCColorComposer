/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef _H_MCLicense
#define _H_MCLicense

extern "C"
{

// This is called when the plugin first starts up.  This is where you would allocate any data structure for use with licensing.
// IMPORTANT:  the value you return from this function must be non-zero to indicate success.  
// The value you return will be passed to all licensing functions as the user data parameter.  If you return null,
// none of the other licensing routines will be called.
void*	LicenseStartup();

// be sure to deallocate any memory you allocated in LicenseStartup
void	LicenseShutdown( void* userData );

// this is called when the plugin needs to know if the software is licensed.
bool	LicenseValid( void* userData );

// this is called when the user clicks the purchase button. NOTE: the plugin will call LicenseValid to determine if the purchase occurred.
void	LicensePurchse( void* userData );

}

class	License_Handler
{
protected:
					License_Handler()					{ }
	virtual			~License_Handler() = 0;

public:
	virtual void	do_licensed_setup(const char* owner_name) = 0;
	virtual void	do_unlicensed_setup() = 0;

	ASUInt32		license_check();

static long			do_load_license(unsigned char* buffer);
static long			do_save_license(unsigned char* license, const long license_length);
};

inline License_Handler::~License_Handler()				{ }


#endif	// !_H_MCLicense

// EOF

