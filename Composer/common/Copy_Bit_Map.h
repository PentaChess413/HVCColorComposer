/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Copy_Bit_Map_h
	#define Copy_Bit_Map_h

#include "G_World.h"

class	Copy_Bit_Map
{
	protected:
		Copy_Bit_Map( DisplayPixelsProc displayProc ) :
			mDisplayProc( displayProc )
		{
			if( !mDisplayProc )
				throw paramErr;
		}
						
		~Copy_Bit_Map()	{}

		void copy_bit_map( const ASPoint& top_left, void* platformContext = NULL ) { (*mDisplayProc)( &m_bit_map.get(), &m_bit_map.get().bounds, top_left.v, top_left.h, platformContext ); }

	public:
		G_World& bit_map() { return m_bit_map; }

	private:
		DisplayPixelsProc	mDisplayProc;
		G_World				m_bit_map;
};

#endif

// EOF
