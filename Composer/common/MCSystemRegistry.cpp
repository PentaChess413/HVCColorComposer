/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

//////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include "PhotoShopSDK.h"

#include "MCSystemRegistry.h"
#include "PIUGet.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//
// Defines
//
//////////////////////////////////////////////////////////////////////////////////////////////

// !!@ fix me !!@
#ifndef MAX_PATH
#define MAX_PATH 1024
#endif


const SpCharPtr kPathToAdobeProducts = "SOFTWARE\\Adobe\\%s\\%s\\";
const SpCharPtr kAppPathKeyString	 = "ApplicationPath";


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
//////////////////////////////////////////////////////////////////////////////////////////////

#if WIN32
MCSystemRegistry::MCSystemRegistry( HKEY whichRegistry ) : mRootKey( whichRegistry )
{
}
#else
MCSystemRegistry::MCSystemRegistry()
{
}
#endif

MCSystemRegistry::~MCSystemRegistry()
{
}


// find the path to a given Adobe product...  Only tested string is "Photoshop"...
ConstSpCharPtr MCSystemRegistry::GetPathToHost( ConstSpCharPtr hostRegName )
{
	if( !hostRegName )
		return NULL;
		
	// first we need to get the version number for this host
	SpChar hostVersionString[MAX_PATH] = {0};
	if( !GetHostVersionString( hostVersionString, MAX_PATH ) )
		return NULL;
	
	// now we need to get to the "application path" but the version number needs to
	// be in there first...  The format is "HKEY_LOCAL_MACHINE\SOFTWARE\Adobe\hostRegName\X.Y\ApplicationPath\"
	SpChar keyString[MAX_PATH] = {0};
	::sprintf( keyString, kPathToAdobeProducts, hostRegName, hostVersionString );

#if WIN32
	// okay now lookup that key and get the path string from it...
	HKEY appKey = OpenRegistry( keyString );
	if( !appKey )
		return NULL;
		
	// grab string from key...
	SpCharPtr resultPtr = NULL;
	DWORD pathSize = sizeof( mHostPath );
	if( ::RegQueryValueEx( appKey, kAppPathKeyString, 0, NULL, (LPBYTE)mHostPath, &pathSize ) == ERROR_SUCCESS )
		resultPtr = mHostPath;
		
	// we are done, clean up
	CloseRegistry( appKey );

	return resultPtr;
#endif
	
	// !!@ implement for mac!
	return NULL;
}

#if WIN32
HKEY MCSystemRegistry::OpenRegistry( ConstSpCharPtr path )
{
	HKEY result = NULL;

	if( ::RegOpenKeyEx( mRootKey, path, 0, KEY_READ, &result ) == ERROR_SUCCESS )
		return result;

	return NULL;
}

void MCSystemRegistry::CloseRegistry( HKEY key )
{
	::RegCloseKey( key );
}
#endif



bool MCSystemRegistry::GetHostVersionString( SpCharPtr buffer, size_t bufSize )
{
	// !!@ this is kinda bogus...
	if( !buffer || bufSize < 6 )
		return false;
		
	int32 major = 0;
	int32 minor = 0;
	int32 fix   = 0;

#if DO_PREFS
	if( !::PIGetHostVersion( major, minor, fix ) )
	{
		// NOTE: this is a dangerous way to do things as a buffer overrun could occur though very unlikely. !!@
		::sprintf( buffer, "%ld.%ld", major, minor );
		return true;
	}
#else
	// this is insane -- test code !!@
	::sprintf( buffer, "%ld.%ld", 9, 0 );
	return true;
#endif

	return false;
}


// EOF
