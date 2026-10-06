/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "MCColorSwatch.h"
#include "Composer_UI.h"



ASBoolean MCColorSwatch::HandleTracking( ADMTrackerRef tracker )
{
	if( ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonUpAction ) )
	{
		const ASRect	control_bounds = bounds();
		ASPoint			mouse_point = {0};

		ADM_Access::tracking_suite()->GetPoint( tracker, &mouse_point );

		if( 0 <= mouse_point.v && 0 <= mouse_point.h 
		&& control_bounds.bottom - control_bounds.top >= mouse_point.v 
		&& control_bounds.right - control_bounds.left >= mouse_point.h )
		{
			if( GetComposerUI() )
				GetComposerUI()->SelectNewColor( GetColorRef() );
		}
	}

	return true;
}

// EOF
