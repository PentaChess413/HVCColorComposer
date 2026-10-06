/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef _H_MCColorSwatch
#define _H_MCColorSwatch

#include "Simple_Color_Display.h"

class MCColorSwatch : public Simple_Color_Display
{
	typedef Simple_Color_Display base;

	public:
		MCColorSwatch( Dialog_Runner& ui, const ASInt32 control_id, MCColorRef p_color ) :
			base( ui, control_id, p_color )
		{}

	private:
		virtual ASBoolean HandleTracking( ADMTrackerRef tracker );
};

#endif

// EOF
