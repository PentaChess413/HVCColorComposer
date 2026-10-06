/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Warning_Color_Display_h
#define Warning_Color_Display_h

#include "Simple_Color_Display.h"


class Warning_Color_Display : public Simple_Color_Display
{
	typedef Simple_Color_Display base;

	public:
		// WARNING: This class handles 2 controls, indexes picture_id and picture_id + 1
		Warning_Color_Display( Dialog_Runner& dialog_runner, const ASInt32 picture_id, MCColorRef p_color );

	private:
		virtual ASBoolean HandleTracking( ADMTrackerRef tracker );
};

#endif

// EOF
