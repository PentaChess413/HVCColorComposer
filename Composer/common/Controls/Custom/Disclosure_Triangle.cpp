/*
 *
 *	Copyright 2005 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Disclosure_Triangle.h"

#include "admDrawer.h"


void Disclosure_Triangle::Draw( ADMDrawerRef environment )
{
	ADM_Access::drawing_suite()->Clear( environment );

	ASRect	 control_bounds = bounds( true );
	ADMColor arrowColor     = kADMBlackColor;
	
	// figure out whether or not to draw the arrow greyed out
	if( !IsEnabled() )
		arrowColor = kADMDisabledColor;
	
	ADM_Access::drawing_suite()->SetADMColor( environment, arrowColor );
	
	// figure out which way to draw...
	if( bool_value() )
		ADM_Access::drawing_suite()->DrawDownArrow( environment, &control_bounds );
	else
		ADM_Access::drawing_suite()->DrawRightArrow( environment, &control_bounds );
}



ASBoolean Disclosure_Triangle::HandleTracking( ADMTrackerRef tracker )
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
			// this strange syntax is call to member-function pointer...
			(GetComposerUI()->*mCallback)();
		}
	}

	return true;
}
