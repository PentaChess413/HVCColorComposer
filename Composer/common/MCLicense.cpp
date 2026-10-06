/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "PhotoshopSDK.h"

#include "MCLicense.h"



#if WIN32
#include "License.h"
#include "License_Files.h"
#include "rsa_c.h"
#include "gmp.h"

class	Check_License : public License_Handler
{
public:
					Check_License()					: m_licensed(false) { }

	bool			licensed() const				{ return m_licensed; }

	virtual void	do_licensed_setup(const char*)	{ m_licensed = true; }
	virtual void	do_unlicensed_setup()			{ m_licensed = false; }

private:
	bool	m_licensed;
};

// Dummied out licensing code!

void* LicenseStartup()
{
	return (void*)1;
}

// be sure to deallocate any memory you allocated in LicenseStartup
void LicenseShutdown( void* )
{
}

// this is called when the plugin needs to know if the software is licensed.
bool LicenseValid( void* )
{
	return true;
}

// this is called when the user clicks the purchase button. 
// NOTE: the plugin will call LicenseValid to determine if the purchase occurred.
void LicensePurchse( void* )
{
}


#endif