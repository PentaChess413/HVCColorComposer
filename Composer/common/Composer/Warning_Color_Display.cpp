/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Warning_Color_Display.h"
#include "Composer_UI.h"
#include "Custom_UI.h"

#include "admTracker.h"


// WARNING: this code makes an assumption about item numbers!!@
Warning_Color_Display::Warning_Color_Display( Dialog_Runner& dialog_runner, const ASInt32 picture_id, MCColorRef p_color ) :
	base( dialog_runner, picture_id + 1, p_color )
{
	Control_Interface picture_control( dialog_runner.get_native_item( picture_id ) );
	
	// we need there to be no user data in here...  
	if( picture_control.user_data() )
		throw ASErr( kBadParameterErr );

	// NOTE: we are taking over control of the "picture item" here...
	picture_control.set_user_data( this );
	picture_control.set_proc( TrackingCallback );
}

ASBoolean Warning_Color_Display::HandleTracking( ADMTrackerRef tracker )
{
	if( ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonUpAction ) )
	{
		const ASRect	control_bounds = bounds();
		ASPoint			mouse_point;

		ADM_Access::tracking_suite()->GetPoint( tracker, &mouse_point );

		if( 0 <= mouse_point.v && 0 <= mouse_point.h 
		&& control_bounds.bottom - control_bounds.top >= mouse_point.v 
		&& control_bounds.right - control_bounds.left >= mouse_point.h )
		{
			// we try the composer UI first... 
			if( GetComposerUI() )
				GetComposerUI()->SelectNewColor( GetColorRef() );
			
			// we don't select a new color when clicked on in the Custom dialog so don't call select new color...
		}
	}

	return true;
}


// EOF
