/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#ifndef Selected_Color_Display_h
#define Selected_Color_Display_h

#include "Custom_Draw_Control.h"
#include "Selected_Color.h"


class Selected_Color_Display : public Custom_Draw_Control
{
	typedef Custom_Draw_Control	base;

	public:
		Selected_Color_Display( Dialog_Runner& dialog_runner, const ASInt32 control_id, SelectedColorRef colorRef ) :
			base( dialog_runner, control_id ),
			mColor( colorRef )
			{}

	private:
		// overrides
		virtual ASBoolean	HandleTracking( ADMTrackerRef tracker );
		virtual void		Draw( ADMDrawerRef environment );
		
		// data
		SelectedColorRef	mColor;
};

#endif

// EOF
