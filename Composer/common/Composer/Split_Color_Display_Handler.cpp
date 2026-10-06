/*
 *
 *	Copyright 2006 Master Colors.
 *
 *	This entire source code listing is the proprietary property of Master Colors
 *
 *	U.S. and Foreign patent applications pending
 *
 */

#include "Split_Color_Display_Handler.h"
#include "Composer_UI.h"



void Split_Color_Display_Handler::Draw( ADMDrawerRef environment )
{
	ASRect	control_bounds = bounds( true );

	draw_frame( environment, control_bounds );

	ASInt32 old_right = control_bounds.right;
	
	// draw left side
	control_bounds.right = (control_bounds.right - control_bounds.left) / 2 + control_bounds.left;
	m_left_color.Draw( environment, &control_bounds, false );
	
	// draw the right side
	control_bounds.left = control_bounds.right;
	control_bounds.right = old_right;
	m_right_color.Draw( environment, &control_bounds, false );
}



ASBoolean Split_Color_Display_Handler::HandleTracking( ADMTrackerRef tracker )
{
	ASPoint	mouse_point;

	if( ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonDownAction ) )
	{
		ADM_Access::tracking_suite()->GetPoint( tracker, &mouse_point );

		const ASRect	control_bounds = bounds();

		m_tracking_right = (control_bounds.right - control_bounds.left) >> 1 < mouse_point.h;
	}
	else if( ADM_Access::tracking_suite()->TestAction( tracker, kADMButtonUpAction ) )
	{
		ADM_Access::tracking_suite()->GetPoint( tracker, &mouse_point );

		const ASRect	control_bounds = bounds();
		const ASInt32	width = control_bounds.right - control_bounds.left;
		const ASInt32	half_width = width >> 1;

		if( 0 <= mouse_point.v && (m_tracking_right ? half_width : 0) <= mouse_point.h 
		&& control_bounds.bottom - control_bounds.top >= mouse_point.v 
		&& (m_tracking_right ? width : half_width) >= mouse_point.h )
		{
			// tell everyone that we are selecting a new color...
			GetComposerUI()->SelectNewColor( m_tracking_right ? m_right_color : m_left_color );
		}
	}

	return true;
}

// EOF
