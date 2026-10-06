/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "MCIcon.h"
#include "Composer_UI.h"

#if PROVERSION

void MCIcon::Draw( ADMDrawerRef environment )
{
	// draw the icon
	ADM_Access::item_suite()->DefaultDraw( native(), environment );
}


ASBoolean MCIcon::HandleTracking( ADMTrackerRef tracker )
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
				GetComposerUI()->SetSortOrder( (ASInt8)(id() - PALETTE_ORDER_0) );
		}
	}

	return true;
}

#endif // PROVERSION

// EOF
