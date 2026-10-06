/* REMOVE ASAP
 *
 *	Copyright 2005-2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 *

#include "PhotoshopSDK.h"

#include "License_Files.h"
#include "Scripting.h"
#include "Auto_Descriptor.h"

#define LICENSE_DATA	'lcns'

// Load the licensing information from the Photoshop preferences
SInt32 Licensing::load_license(UInt8* buffer)
{
	if (!sSPBasic)	// Invalid Photoshop setup
		return 0;

	try
	{
		// Aquire the function pointer objects
		AutoSuite<PSDescriptorRegistryProcs>	registryProcs(kPSDescriptorRegistrySuite,
																kPSDescriptorRegistrySuiteVersion);
		AutoSuite<PSActionDescriptorProcs>		descriptorProcs(kPSActionDescriptorSuite,
																kPSActionDescriptorSuiteVersion);
		// Get the preference Descriptor
		Auto_Descriptor							descriptor(*registryProcs, *descriptorProcs,
															registrationUniqueID);
		int32									length = 0;

		// Get the licensing information from the Descriptor
		THROW_IF_ERR(descriptorProcs->GetDataLength(descriptor, LICENSE_DATA, &length));
		THROW_IF_ERR(descriptorProcs->GetData(descriptor, LICENSE_DATA, buffer));

		return length;
	}
	catch(...)
	{	// Don't let C++ exceptions escape
	}

	return 0;
}

// Save the licensing information in the Photoshop preferences
OSStatus Licensing::save_license(UInt8* license, const SInt32 license_length)
{
	if (!sSPBasic)	// Invalid Photoshop setup
		return errPlugInHostInsufficient;

	try
	{
		// Aquire the function pointer objects
		AutoSuite<PSDescriptorRegistryProcs>	registryProcs(kPSDescriptorRegistrySuite,
																kPSDescriptorRegistrySuiteVersion);
		AutoSuite<PSActionDescriptorProcs>		descriptorProcs(kPSActionDescriptorSuite,
																kPSActionDescriptorSuiteVersion);
		// Create a new Descriptor
		Auto_Descriptor							descriptor(*descriptorProcs);

		// Save the licensing information
		THROW_IF_ERR(descriptorProcs->PutData(descriptor, LICENSE_DATA, license_length, license));
		// Register it with Photoshop
		THROW_IF_ERR(registryProcs->Register(registrationUniqueID, descriptor, true));
	}
	catch(const OSStatus& err)
	{	// Return any system errors
		return err;
	}
	catch(...)
	{	// Don't let C++ exceptions escape
		return 1;
	}

	return noErr;
}
*/