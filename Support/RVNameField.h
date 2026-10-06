
///////////////////////////////////////////////////////////////////////////////
//
// Copyright 1998-2005 FOInc.  All rights reserved.  $Revision: 1.1.1.1 $
//      This material is the confidential trade secret and proprietary
//      information of FOInc.  It may not be reproduced, used,
//      sold or transferred to any third party without the prior written
//      consent of FOInc.  All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////

/*	History:

04oct26 lma FOSS: Deleted superfluous methods that aren't used by the plugin
30jun05	alx	Created - yanked out of RV3DFile.h

*/


#ifndef _H_RVNameField
#define _H_RVNameField

#ifndef _H_FastAlloc
#include "FastAlloc.h"
#endif

const SpShort kRV3DObject_MaxName = 64;

class RVNameField
{
	public:
	RVNameField() { Clear(); }
	RVNameField( RVNameField& rhs ) { Clear(); Set( rhs.mName ); }
	RVNameField( ConstSpCharPtr ptr ) { Clear(); Set( ptr ); }

	void Clear() { FastAlloc::ClearNumBytes( mName, sizeof( mName ) ); }
	void Set( ConstSpCharPtr ptr ) { if( ptr ) ::strncpy( mName, ptr, kRV3DObject_MaxName ); }

	const RVNameField& operator+=( const SpChar* str )
	{
		::strncat( mName, str, kRV3DObject_MaxName );
		return *this;
	}

	inline operator SpChar*() { return mName; }

	SpChar mName[kRV3DObject_MaxName];
};

class RVWideNameField
{
 public:
  SpWChar mName[kRV3DObject_MaxName];

  RVWideNameField() { Clear(); }
  RVWideNameField( RVWideNameField& rhs ) { Clear(); Set( rhs.mName ); }
  RVWideNameField( ConstSpWCharPtr ptr ) { Clear(); Set( ptr ); }

  void Set( ConstSpWCharPtr ptr ) { if( ptr ) ::wcsncpy( mName, ptr, kRV3DObject_MaxName ); }
  void Clear() { FastAlloc::ClearNumBytes( mName, sizeof( mName ) ); }
  
  inline void ReplaceEscapeCodes()
  {
	  SpWCharPtr ptr = ::wcsstr( mName, L"^R" );
	  if( ptr )
	  {
		  *ptr++ = 0xAE;

		  do
		  {
			*ptr = *(ptr + 1);
		  }
		  while( *ptr++ != NULL );
	  }

	  ptr = ::wcsstr( mName, L"^C" );
	  if( ptr )
	  {
		*ptr++ = 0xA9;

		do
		{
			*ptr = *(ptr + 1);
		}
		while( *ptr++ != NULL );
	  }
  }

  const RVWideNameField& operator=( const RVWideNameField& v )
  {
   if( v.mName )
    ::wcsncpy( mName, v.mName, kRV3DObject_MaxName );

   return *this;
  }

  const RVWideNameField& operator+=( const SpWChar* str )
  {
   ::wcsncat( mName, str, kRV3DObject_MaxName );
   return *this;
  }

  inline operator WChar16*() { return mName; }
  
};

typedef RVWideNameField* RVWideNamePtr;

#endif // !_H_RVNameField

// EOF
