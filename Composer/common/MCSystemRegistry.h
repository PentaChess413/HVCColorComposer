/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */


#ifndef _H_MCSystemRegistry
#define _H_MCSystemRegistry


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Includes
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include "MCColorbook.h"


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Defines
//
//////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////
//
// Class
//
//////////////////////////////////////////////////////////////////////////////////////////////

class MCSystemRegistry
{
	public:
#if WIN32	
		MCSystemRegistry( HKEY whichReg );
#else
		MCSystemRegistry();
#endif		
		virtual ~MCSystemRegistry();

		// raison d'etre -- you must copy the string
		// as it will only live as long as this object is around...
		ConstSpCharPtr GetPathToHost( ConstSpCharPtr hostRegName );

	protected:
#if WIN32	
		// get a specific key
		HKEY OpenRegistry( ConstSpCharPtr path );
		void CloseRegistry( HKEY key );
#endif
		// find host version
		bool GetHostVersionString( SpCharPtr buffer, size_t bufSize );
		
	private:
#if WIN32	
		HKEY	mRootKey;
#endif		
		SpChar	mHostPath[4096];
};

#endif // !_H_MCColorLibraries

// EOF
